#include <QThread>
#include <QApplication>
#include <QDir>
#include <algorithm>
#include "srvBehav.h"

extern struct rfAmpSetData cur_par;
extern double userMult;

//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------
TSrvBehav::TSrvBehav()
{
  qDebug()<<"Start constructor of object TSrvBehav.";
  hwReady=0;
  QDir appDir;
  appDir.mkpath(QDir::currentPath());
  //QDir::currentPath()
  fileNameCSV=QApplication::applicationDirPath()+"/data/work.csv";
  readSettings(QApplication::applicationDirPath()+"/data/settings.ini");
  dev=new THw(32768,coefTransf,dac1Sh,dac2Sh);
  connect(&tAlrm,SIGNAL(timeout()),this,SLOT(timeAlarm()));
  cntErr=0;
  hw=0;
  qDebug()<<"Constructor of object TSrvBehav was finished.";
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TSrvBehav::~TSrvBehav()
{
  qDebug()<<"Start destructor of object TSrvBehav.";
  mutex.lock();
  abort = true;
  condition.wakeOne();
  mutex.unlock();
  wait(2000);
  if(isRunning()) {
    qDebug()<<"Object TSrvBehav still running... terminate.";
    terminate();
    wait(2000);
  }
  tAlrm.stop();
  if(dev) delete dev;
  qDebug()<<"Destructor of object TSrvBehav was finished.";
}

/*!
 * \brief TSrvBehav::readSettings
 * \param[in] filename QString Имя файла с настройками
 */
void TSrvBehav::readSettings(QString filename)
{
  QSettings setup(filename, QSettings::IniFormat);
  bool ok;
  setup.beginGroup("setup");
  stepTime=setup.value("stepTime",0.1).toDouble(&ok); if(!ok)stepTime=0.1;
  coefTransf=setup.value("coefTransf",-200).toDouble(&ok); if(!ok) coefTransf=-200;
  userMult=setup.value("userMult",0).toDouble(&ok); if(!ok) userMult=0;
  dac1Sh=setup.value("dac1shift",0).toInt(&ok); if(!ok) dac1Sh=0;
  dac2Sh=setup.value("dac2shift",0).toInt(&ok); if(!ok) dac2Sh=0;
  setup.endGroup();
}

/*!
 * \brief TSrvBehav::prepareCSVData
 * \param[in] filename QString Имя CSV файла
 * \return 0 - Ok, 1 - Нет данных во входном массиве, 2 - Ошибка создания CSV
 *
 */
int TSrvBehav::prepareCSVData(QString filename)
{
  double t=0;

// find first none zero data
  int nPoint=0;
  while(dTime[nPoint]<0.001) { //first all time 0
    if(nPoint>ALLVECTORS-1) return 1; ///< Нет данных во входном массиве
    nPoint++;
  }
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return 2;

  QTextStream out(&file);
  out << "t,y1e,y2e,y1o,y2o\n";

  double y=dAmp[nPoint-1];
  double dy;
  if((dTime[nPoint]-dTime[nPoint-1])<0.0001) dy=0;
  else
    dy=(dAmp[nPoint]-dAmp[nPoint-1])*stepTime/(dTime[nPoint]-dTime[nPoint-1]);

  while(t<dTime[ALLVECTORS-1]+0.001) {
    if(t>=dTime[nPoint]-0.001) {
      nPoint++;
      if((dTime[nPoint]-dTime[nPoint-1])<0.0001) dy=0;
      else
        dy=(dAmp[nPoint]-dAmp[nPoint-1])*stepTime/(dTime[nPoint]-dTime[nPoint-1]);
    }
    out.setRealNumberPrecision(5); out<<fixed<<t/TIMECONV<<',';
    out.setRealNumberPrecision(2); out<<y<<','<<y<<','<<y<<','<<y<<'\n';
    y=y+dy;
    t+=stepTime;
  }
// last data must be zero
  y=0;
  out.setRealNumberPrecision(5); out<<fixed<<t/TIMECONV<<',';
  out.setRealNumberPrecision(2); out<<y<<','<<y<<','<<y<<','<<y<<'\n';
  out.flush();
  file.close();
  return 0;
}

// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
  dbg.nospace() <<"\n---------------------------------\nSTATE=";
  switch(t){
  case READY: dbg.space() << "READY" ; break;
  case IDLE: dbg.space() << "IDLE" ; break;
  case INITIAL_STATE: dbg.space() << "INITIAL_STATE" ; break;
  case DEVICE_ERROR_STATE: dbg.space() << "DEVICE_ERROR_STATE"; break;
  case UPDATE_HW_STATE: dbg.space() << "UPDATE_HW_STATE" ; break;
  case UPDATE_STATUS_STATE: dbg.space() << "UPDATE_STATUS_STATE" ; break;
  default:;
  }

  return dbg.nospace() ;//<< endl;;
}
//-----------------------------------------------------------------------------
//--- State all error Status for all stadies
//-----------------------------------------------------------------------------
void TSrvBehav::setErrorSt(short int st)
{
  stErr=st;
}

//-----------------------------------------------------------------------------
//--- timer timeout event. On this event can get data from device
//-----------------------------------------------------------------------------
void TSrvBehav::timeAlarm(void)
{
  if(canTic) {
    allStates[UPDATE_STATUS_STATE]=UPDATE_STATUS_STATE;
    canTic=false;
    condition.wakeOne();
  }
}

//-----------------------------------------------------------------------------
//--- Run process. Main cycle with state machine
//-----------------------------------------------------------------------------
void TSrvBehav::run()
{
  int tic=0;
  qDebug()<<"Run cycle processing of object TSrvBehav";
  QEventLoop *loop ;
  loop=new QEventLoop();
  CPhase deb=READY;//for debug only
  setErrorSt(::CODEERR::NOTREADY);
  abort = false;
  canTic=false;
  for(int i=0;i<ALLREQSTATES;i++) allStates[i]=READY;
  phase = INITIAL_STATE;
  tAlrm.start(SAMPLE_DEVICE); //start timer for sample device
  statusHW=0;

  while(!abort) { // run until destructor not set abort
 //   msleep(1);
    //loop->processEvents();
    mutex.lock();
    if(phase==READY){
      for(int i=0;i<ALLREQSTATES;i++) { // read all states request and run if state!= READY state. high priority has low index
        if(allStates[i]!=READY){
          phase=allStates[i];
          allStates[i]=READY;
          break;
        }
      }
    }
    if(phase==READY) {
      phase=IDLE; condition.wait(&mutex); phase=READY;
    }
    mutex.unlock();
    if(deb!=phase) { qDebug()<<phase;deb=phase;}
    switch(phase) {
      // initial device on begin work
      case INITIAL_STATE: { //
        canTic=false;
        devError.clearDevError(); // clear device error
        syslog(LOG_INFO,"Initialisation of device is begin.");
        qDebug()<<"Initialisation of device is begin.";
        initialDevice();
        stErr=devError.analyseError();
        if(stErr) {
          syslog(LOG_ERR,"Initialisation of device error.");
          qDebug()<<"Initialisation of device error.";
          allStates[INITIAL_STATE]=INITIAL_STATE;
          msleep(1000); //wait some time and try init again
          qDebug()<<"Version"<<hw;
          syslog(LOG_ERR,"Version %d",hw);
          QStringList err=devError.readDevTextErrorList();
          for(int i=0;i<err.size();i++) {
            syslog(LOG_ERR,err.at(i).toAscii());
            qDebug()<<err.at(i);
          }
          if(cntErr++>5) { // fatal HW error finish program
            qDebug()<<"Fatal device error! Program terminated.";
            syslog(LOG_ERR,"Fatal device error! Program terminated.");
            hwReady=1;
            abort=true;
          }
        }
        else {
          cntErr=0;
          allStates[INITIAL_STATE]=READY;
          phase = READY;
          syslog(LOG_INFO,"Initialisation of device was finished successfully.");
          syslog(LOG_INFO,"Hw version %d",hw);
          qDebug()<<"Initialisation of device was finished successfully.";
          qDebug()<<"Hw version"<<hw;
          hwReady=1;
          canTic=true;
        }
        break;
      }// end case INITIAL_STATE:
// --SM Update HW -----------------------------------------------------
      case UPDATE_HW_STATE: {
        canTic=false;
        qDebug()<<"Update data into HW";
        devError.clearDevError();
        updateHW(); // updata data into HW
        allStates[UPDATE_HW_STATE]=READY;
        stErr=devError.analyseError();
        if(stErr) {
          phase=DEVICE_ERROR_STATE;
          syslog(LOG_ERR,"Update data into HW error."); qDebug()<<"Update data into HW error.";
          emit signalErrMsg("Error!");
          statusHW=0; // for set status message
        }
        else {
          syslog(LOG_INFO,"Update data into HW Ok."); qDebug()<<"Update data into HW Ok.";
          emit signalErrMsg("OK!");
          phase=READY;
        }
        canTic=true;
        break;
      }// end case UPDATE_HW_STATE

//  get info about device
      case UPDATE_STATUS_STATE: {
        int sts=getStatus(); // get status & WD reset
        if((sts^statusHW)&PS_SYNC) {
          if(sts&PS_SYNC) { // absent synchronisation
            emit signalStatusMsg("No sync.");
           }
           else {
              if(cur_par.overload) emit signalStatusMsg("Overload");
              else
                emit signalStatusMsg("Normal  ");
            }
        }
        if(sts!=statusHW) {
          //syslog(LOG_ERR,"Change status. Was %x current %x",statusHW,sts);
          qDebug()<<"Change status. Was"<<statusHW<<"current"<<sts;
          statusHW=sts;
        }
        tic++;
        if(tic&1) dev->usr_hostAliveOn(); else dev->usr_hostAliveOff(); // led indication activity of program
        allStates[UPDATE_STATUS_STATE]=READY;
        phase = READY;
        canTic=true;
        break;
      }//end case UPDATE_STATUS_STATE:

// processing hardware errors in device
      case DEVICE_ERROR_STATE:{
        qDebug()<<"Found device error!";
        syslog(LOG_ERR,"Found device error!");
        qDebug()<<"Version"<<hw;
        syslog(LOG_ERR,"Version %d",hw);
        QStringList err=devError.readDevTextErrorList();
        for(int i=0;i<err.size();i++) {
          syslog(LOG_ERR,err.at(i).toAscii());
          qDebug()<<err.at(i);
        }
        phase = READY;
        break;
      }// end case DEVICE_ERROR_STATE:

      default:;
// --SM Start Measure end
    } // End Switch main state machine--------------------------------------------------------------------------------------------------------------
    if(abort) break;
  }
  qDebug()<<"Run() of object TSrvBehav was finished.";
}

void TSrvBehav::endHwReq(void)
{
  while((allStates[UPDATE_HW_STATE]==UPDATE_HW_STATE)||(phase==UPDATE_HW_STATE)) msleep(10);
}

void TSrvBehav::writeHwReq(void)
{
  while(phase!=IDLE) msleep(10);
  allStates[UPDATE_HW_STATE]=UPDATE_HW_STATE;
  condition.wakeOne();
}

//================= DEVICE PART ================================================================================================================
//-----------------------------------------------------------------------------
//--- Initialise device INITIAL_STATE in state machine
//-----------------------------------------------------------------------------
void TSrvBehav::initialDevice(void)
{
  if(dev->initalBBHW())  { devError.setDevError(TDevErrors::GPIO_ERROR); return; } // initial pins and spi
  dev->usr_pdOn();  // power down mode output into 0
  dev->usr_rst();
  hw=dev->usr_getHW(); // get FPGA FW code put in LOG
  if(hw==65535) {
    devError.setDevError(TDevErrors::PS_HW_ERROR);
  }
}

/*!
 * \brief TSrvBehav::writeHWsimple запись данных в аппаратуру без БД. Пишется work.csv в каталоге data
 *
 * \return 0 - Ok 1 - Error
 */
int TSrvBehav::wrHWsimple(void)
{
  int ret=0;
  devError.clearDevError();
  initialDevice();
  stErr=devError.analyseError();
  if(stErr) {
    syslog(LOG_ERR,"Error in simple HW initialisation.");
    qDebug()<<"Error in simple HW initialisation.";
    qDebug()<<"Version"<<hw;
    syslog(LOG_ERR,"Version %d",hw);
    QStringList err=devError.readDevTextErrorList();
    for(int i=0;i<err.size();i++){
      syslog(LOG_ERR,err.at(i).toAscii());
      qDebug()<<err.at(i);
    }
    ret=1;
  }
  else {
    syslog(LOG_INFO,"Initialisation of device in simple mode was finished successfully.");
    syslog(LOG_INFO,"Hw version %d",hw);
    qDebug()<<"Initialisation of device in simple mode was finished successfully.";
    qDebug()<<"Hw version"<<hw;
    if(!dev->usr_waitCanWrite()) { //absent synchronisation
      syslog(LOG_INFO,"Absent synchronisation");
      qDebug()<<"Absent synchronisation";
    }
    dev->usr_writeMode(3);
    if(dev->usr_WrDACFile(fileNameCSV)) {
      syslog(LOG_INFO,"Work CSV file is absent. %s",fileNameCSV.toAscii().data());
      qDebug()<<"Work CSV file is absent"<<fileNameCSV; ret=2;
    }
    dev->usr_pdOff(); // on ps after write
    printf("Write HW OK.\n");
    syslog( LOG_INFO,"Write HW OK..");
    while(1) {
      dev->usr_hostWd();
      msleep(200);
      dev->usr_hostAliveOn();
      msleep(200);
      dev->usr_hostAliveOff();
    }
  }
  return(ret);
}

// 0 0x01-PS1 status On/off from source 1 on, 0 off
// 1 0x02-PS1 status error from source 1 error
// 2 0x04-PS2 status On/off from source 1 on, 0 off
// 3 0x08-PS2 status error from source 1 error
// 4 0x10-synchronisation is absent 1 error not go start pulse
// 5 0x20-window strobe 1 strobe (work PS)
// 6 0x40 -window can read 1 can read/write
// 7 0x80-reserve
int TSrvBehav::getStatus()
{
// get status from HW

// watch dog and feadback processing
  dev->usr_hostWd();
  int s=dev->usr_getWDO(); //qDebug()<<"STS"<<s;
  //if(s==0) devError.setDevError(TDevErrors::FPGA_LOAD_ERROR);
  if(s<0) devError.setDevError(TDevErrors::GPIO_ERROR);

  u_int32_t st=dev->usr_RdStatus()&0xff;

  if(st&PS_SYNC) devError.setDevError(TDevErrors::SYNCHR_ERROR);
  if((st&PS1_ERR)||(st&PS2_ERR)) devError.setDevError(TDevErrors::PS_HW_ERROR);

  return st;
}
/*
QTextCodec - это такая хитрая хрень, в которой хитрости больше, чем смысла.
Хитрость в том, что кодек надо создавать всегда с названием какой-то кодировки, например "cp-1251". Читатель спросит:
есть два рубля? а почему одной? А если я хочу перекодировать из cp1251 в Utf-8?
Идея такая: кодек, который вы создаёте с указанием конкретной кодировки, умеет лишь гонять строку между указанной кодировкой
и юникодом. Юникод - это не utf-8/utf-32, а жирная 2-байтовая таблица кодов; utf-8 - это не юникод, не надо путать, а только
одно из представлений юникода.
Так вот, после того как вы создали кодек (например "cp-1251") он предлагает вам гонять строку в двух направлениях
"UNICODE <---> cp1251".
Чтобы засобачить cp1251 в KOI8, вам надо создать два кодека, потом проделать операцию "cp1251 ---> UNICODE ---> KOI8".
У кодека есть две основные операции:
* toUnicode
* fromUnicode
Это и есть два вышеописанных направления. Когда для кодека "cp1251" вы вызываете "toUnicode()" - это направление
"cp1251 -> UNICODE", а когда fromUnicode(), то наоборот "UNICODE -> cp1251".
toUnicode() всегда возвращает QString, так как QString - это массив двухбайтовых значений - как раз для переноски юникода.
fromUnicode() всегда возващает массив байт QByteArray, так как направление "из UNICODE в КАКУЮ_ЛИБО_КОДИРОВКУ" предполагает
возврат юзеру потенциально диких с точки зрения QString кусков памяти, поэтому проще QByteArray().
*/

// update HW on reqest
void TSrvBehav::updateHW(void)
{
  qDebug()<<"Update HW data";
  for(int i=0;i<ALLVECTORS;i++) {
    dTime[i]=cur_par.time[i];
    dAmp[i]=cur_par.realAmp[i];
  }
  if(prepareCSVData(fileNameCSV)) { devError.setDevError(TDevErrors::INFILE_ERROR);return; }
  if(!fileNameCSV.size()){ devError.setDevError(TDevErrors::INFILE_ERROR);return; }
  if(!dev->usr_waitCanWrite()) { //absent synchronisation
    syslog(LOG_INFO,"Absent synchronisation");
    qDebug()<<"Absent synchronisation";
    devError.setDevError(TDevErrors::SYNCHR_ERROR);
  }
  dev->usr_writeMode(3);
  if(dev->usr_WrDACFile(fileNameCSV)) {
    devError.setDevError(TDevErrors::INFILE_ERROR);
    syslog(LOG_INFO,"Work CSV file is absent. %s",fileNameCSV.toAscii().data());
    qDebug()<<"Work CSV file is absent"<<fileNameCSV;
    return;
  }
  dev->usr_pdOff(); // on ps after write
  getStatus();
  qDebug()<<"Write data request compleated";
}




