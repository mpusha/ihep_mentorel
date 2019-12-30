#include <QThread>
#include <QApplication>
#include <QDir>
#include <algorithm>
#include "srvBehav.h"




//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------
TDtBehav::TDtBehav(QString dirName)
{
  QDir appDir;
  appDir.mkpath(QDir::currentPath());
  //QDir::currentPath()
  workDirName=dirName;
  abort = false;
  dev=new THw(32768);
  connect(&tAlrm,SIGNAL(timeout()),this,SLOT(timeAlarm()));
  for(int i=0;i<ALLREQSTATES;i++) allStates[i]=READY;
  phase = INITIAL_STATE;
  timerAlrm=false;
  tAlrm.start(SAMPLE_DEVICE); //start timer for sample device
  cntErrReq=0; wrHWReq=0; wrDataReq=0; measReq=0;
  powerValue=0;canWR=false;
  powerStatus=-1;
  canUpdateMData=false;
  statusReq="\"Starting\"";
  //statusReq="\"0\"";
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TDtBehav::~TDtBehav()
{
  qDebug()<<"Start device destructor.";
  tAlrm.stop();
  syslog(LOG_INFO,"Finish program.");
  mutex.lock();
  abort = true;
  condition.wakeOne();
  mutex.unlock();
  wait();
  if(dev) delete dev;
  syslog(LOG_INFO,"Device destructor compleate.");
  qDebug()<<"Device destructor compleate.";
}

// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
  dbg.nospace() <<"\n---------------------------------\nSTATE=";
  switch(t){
  case READY: dbg.space() << "READY" ; break;
  case INITIAL_STATE: dbg.space() << "INITIAL_STATE" ; break;
  case DEVICE_ERROR_STATE: dbg.space() << "DEVICE_ERROR_STATE"; break;
  case GETINFO_STATE: dbg.space() << "GETINFO_STATE" ; break;
  case GET_REQ_AND_STATUS_STATE: dbg.space() << "GET_REQ_AND_STATUS_STATE" ; break;
  case UPDATE_HW_STATE: dbg.space() << "UPDATE_HW_STATE" ; break;
  case UPDATE_STATUS_STATE: dbg.space() << "UPDATE_STATUS_STATE" ; break;
  default:;
  }

  return dbg.nospace() ;//<< endl;;
}
//-----------------------------------------------------------------------------
//--- State all error Status for all stadies
//-----------------------------------------------------------------------------
void TDtBehav::setErrorSt(short int st)
{
  stErr=st;
}

//-----------------------------------------------------------------------------
//--- timer timeout event. On this event can get data from device
//-----------------------------------------------------------------------------
void TDtBehav::timeAlarm(void)
{
  if(timerAlrm) {
    allStates[GET_REQ_AND_STATUS_STATE]=GET_REQ_AND_STATUS_STATE;
    timerAlrm=false;
  }
}

//-----------------------------------------------------------------------------
//--- Run process. Main cycle with state machine
//-----------------------------------------------------------------------------
void TDtBehav::run()
{
  CPhase deb=READY;//for debug only
  setErrorSt(::CODEERR::NOTREADY);
  msleep(200); // wait if devise turn on in this time

  while(!abort) { // run until destructor not set abort
    msleep(1);
    mutex.lock();
    if(phase==READY){

      for(int i=0;i<ALLREQSTATES;i++) { // read all statese request and run if state!= READY state. high priority has low index
        if(allStates[i]!=READY){
          phase=allStates[i];
          allStates[i]=READY;
          break;
        }
      }
    }
    mutex.unlock();
    if(deb!=phase) { qDebug()<<phase;deb=phase;}
    switch(phase) {
      case READY: {
        QApplication::processEvents();
        msleep(1);
        break;
      }//end case READY:
      // initial device on begin work
      case INITIAL_STATE: { //
        devError.clearDevError(); // clear device error
        syslog(LOG_INFO,"Initialisation of device is begin.");
        qDebug()<<"Initialisation of device is begin.";
        initialDevice();
        stErr=devError.analyseError();
        if(stErr) {
          timerAlrm=false;
          syslog(LOG_ERR,"Initialisation of device error.");qDebug()<<"Initialisation of device error.";
          allStates[INITIAL_STATE]=INITIAL_STATE;
          msleep(1000); //wait some time and try init another
          phase=DEVICE_ERROR_STATE;
        }
        else {
          cntErrReq=0; wrHWReq=0; wrDataReq=0; measReq=0; canWR=false; powerStatus=0; powerValue=0;
          canUpdateMData=false;
          initialSystem();
          allStates[INITIAL_STATE]=READY;
          phase = GET_REQ_AND_STATUS_STATE;
          syslog(LOG_INFO,"Initialisation of device was finished successfully.");
          syslog(LOG_INFO,"Hw version %d",hw);
          qDebug()<<"Initialisation of device was finished successfully.";
          qDebug()<<"Hw version"<<hw;
          timerAlrm=true;
        }
        break;
      }// end case INITIAL_STATE:
// Sample device request from timer
      case GET_REQ_AND_STATUS_STATE: { // get requests from client and status from HW
        devError.clearDevError();
        dev->usr_hostAliveOn(); // led indication activity of program
        getRequests(); // get request from client for write power/data and measure (get from settings)
        getStatus(); // get status from HW (signature, power, and window status for write data into HW) WD reset and WDO analyse here
        allStates[GET_REQ_AND_STATUS_STATE]=READY; // reset state
        stErr=devError.analyseError(); // set error, if present
        if(stErr>::CODEERR::NONE_ALARM) {
          cntErrReq=0;
          syslog(LOG_ERR,"None alarm error. HW %d Sample %d Strobe %d Status %d",dev->usr_getHW(),dev->usr_RdSample(),dev->usr_RdStrobe(),dev->usr_RdStatus());
          allStates[UPDATE_HW_STATE]=UPDATE_STATUS_STATE;
          phase=READY;
          break;
        }
        if(stErr) {
          //timerAlrm=true; // can get data again
          syslog(LOG_ERR,"Get device status error.");
          qDebug()<<"Get device status error.";
          phase=DEVICE_ERROR_STATE;
          if(cntErrReq++>5) allStates[INITIAL_STATE]=INITIAL_STATE; // initial system again
        }
        else{
          cntErrReq=0;
          allStates[UPDATE_HW_STATE]=UPDATE_HW_STATE;
          phase=READY;
        }
        break;
      }//end case GETPARREQ_STATE:
// --SM Start Measure begin-----------------------------------------------------
      case UPDATE_HW_STATE: {
        devError.clearDevError();
        updateHW(); // updata data into HW
        allStates[UPDATE_HW_STATE]=READY;
        stErr=devError.analyseError();
        if(stErr){
          phase=DEVICE_ERROR_STATE;
          syslog(LOG_ERR,"Update data into HW error."); qDebug()<<"Update data into HW error.";
          allStates[INITIAL_STATE]=INITIAL_STATE; // initial system again
        }
        else {
          allStates[UPDATE_STATUS_STATE]=UPDATE_STATUS_STATE ;
          phase=READY;
        }

        break;
      }// end case STARTRUN_STATE

//  get info about device
      case UPDATE_STATUS_STATE: {
          updateStatus();
          allStates[UPDATE_STATUS_STATE]=READY;
          phase = READY;
          // led indication activity of program
          dev->usr_hostAliveOff();
          timerAlrm=true;
        break;
      }//end case UPDATE_STATUS_STATE:

// processing hardware errors in device
      case DEVICE_ERROR_STATE:{
        syslog(LOG_ERR,"Found device error!"); qDebug()<<"Found device error!";
        QStringList err=devError.readDevTextErrorList();
        for(int i=0;i<err.size();i++){
          syslog(LOG_ERR,err.at(i).toAscii());qDebug()<<err.at(i);
        }
        qDebug()<<"Version"<<dev->usr_getHW();
        syslog(LOG_ERR,"Version %d",dev->usr_getHW());
        phase = UPDATE_STATUS_STATE;
        break;
      }// end case DEVICE_ERROR_STATE:


      default:;
// --SM Start Measure end
    } // End Switch main state machine--------------------------------------------------------------------------------------------------------------
    if(abort) return;
  }
}



//================= DEVICE PART ================================================================================================================
//-----------------------------------------------------------------------------
//--- Initialise device INITIAL_STATE in state machine
//-----------------------------------------------------------------------------
void TDtBehav::initialDevice(void)
{
// load FPGA
 // devError.setDevError(TDevErrors::FPGA_LOAD_ERROR); 
  if(dev->initalBBHW())  { devError.setDevError(TDevErrors::GPIO_ERROR); return; } // initial pins and spi
  dev->usr_pdOn();  // power down mode output into 0
   //{confFPGA("vobler.rbf"); dev->m_sleep(1000);}
  //if(dev->configureFPGA("vobler.rbf")) { devError.setDevError(TDevErrors::FPGA_LOAD_ERROR); return;} // load FPGA configuration
  dev->usr_rst();
  hw=dev->usr_getHW(); // get FPGA FW code put in LOG
  qDebug()<<"Wait 21 sec for synchronization";
  dev->m_sleep(21000); // wait 21 s for synchronisation all times (canread function for property write in pause)
}

void TDtBehav::initialSystem(void)
{
// copy request data file from data into ctrl
  QFile fileOP(workDirName+"data/ctrl/update_power.req");
  if(!fileOP.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream outP(&fileOP);
  outP << "[request]"<<"\n"<<"power=0"<<"\n";
  fileOP.close();
  QFile fileI(workDirName+"data/data/wfile.dat");
  if (!fileI.open(QIODevice::ReadOnly | QIODevice::Text)) { statusReq="\"File absent\""; return; }
  QTextStream in(&fileI);
  QString line = in.readLine();
  fileI.close();

  QFile fileO(workDirName+"data/ctrl/update_data.req");
  if(!fileO.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&fileO);
  out << "[request]"<<"\n"<<"fname="<<line<<"\n";
  fileO.close();

  pulsePS=false;
}

//-------------------------------------------------------------------------------------------------
//--- Get information about current state of device on timer request  after SAMPLE_DEVICE ms
//-------------------------------------------------------------------------------------------------
void TDtBehav::getRequests()
{
  if(QFileInfo(workDirName+"data/ctrl/update_power.req").exists()){
    QSettings setings(workDirName+"data/ctrl/update_power.req",QSettings::IniFormat) ;
    setings.beginGroup("request");
    powerValue=setings.value("power",0).toInt();
    setings.endGroup();
    wrHWReq=true;
    QFile::remove(workDirName+"data/ctrl/update_power.req");
  }
  if(QFileInfo(workDirName+"data/ctrl/update_data.req").exists()){
    QSettings setings(workDirName+"data/ctrl/update_data.req",QSettings::IniFormat) ;
    setings.beginGroup("request");
    selFile=setings.value("fname","").toString();
    if(selFile.size()) wrDataReq=true; else statusReq="\"File Absent\"";
    if(wrDataReq) statusReq="\"Processing\"";
    //if(wrDataReq) statusReq="\"1\"";
    setings.endGroup();
    QFile::remove(workDirName+"data/ctrl/update_data.req");
  }
  //measReq=true;
}

// 0 0x01-PS1 status On/off from source 1 on, 0 off
// 1 0x02-PS1 status error from source 1 error
// 2 0x04-PS2 status On/off from source 1 on, 0 off
// 3 0x08-PS2 status error from source 1 error
// 4 0x10-synchronisation is absent 1 error not go start pulse
// 5 0x20-window strobe 1 strobe (work PS)
// 6 0x40 -window can read 1 can read/write
// 7 0x80-reserve
void TDtBehav::getStatus()
{
// get status from HW
// get power On/Off get can read/write status and signature
  //powerStatus=powerValue;    //for test only

  // watch dog and feadback processing
  dev->usr_hostWd();
  int s=dev->usr_getWDO();
  if(s==0) devError.setDevError(TDevErrors::FPGA_LOAD_ERROR);
  if(s<0) devError.setDevError(TDevErrors::GPIO_ERROR);

  u_int32_t st=dev->usr_RdStatus()&0xff;
  canWR=false;
  if(st&0x40) canWR=true;
  if((st&0x01)&&(st&0x04)) powerStatus=1; else powerStatus=0;
  if(st&0x10) devError.setDevError(TDevErrors::SYNCHR_ERROR);
  if((st&0x02)||(st&0x08)) devError.setDevError(TDevErrors::PS_HW_ERROR);
  if((st&0x40)==0) pulsePS=true;
  if((pulsePS==true)&&((st&0x40)==1)) { measReq=true; pulsePS=false;}
  qDebug()<<"status"<< (void *)st;
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
void TDtBehav::updateStatus()
{
  qDebug()<<"Update ST";
  if(QFileInfo(workDirName+"data/ctrl/status.dat").exists()) return;
  QFile fileO(workDirName+"data/ctrl/status");
  if(!fileO.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&fileO);
  QString pwN,errN;
  qDebug()<<statusReq<<powerStatus;
  if(powerStatus==1) pwN="\"On\"";else if(powerStatus==0) pwN="\"Off\""; else pwN="\"unknown\"";
  //if(powerStatus==1) pwN="\"1\"";else if(powerStatus==0)pwN="\"0\""; else pwN="\"-1\"";
  if(!stErr) errN="\"None\"";
  else
    errN="\""+devError.readDevTextErrorList().at(0)+"\"";
  //int up=0;
  out << "["<<"\n{"<< "'power':"<<pwN<<","<<"'error':"<<errN<<",""'statusReq':"<<statusReq<<","<<"'update':"<<canUpdateMData<<" }\n]\n";
  canUpdateMData=false;
  fileO.close();
  fileO.rename(workDirName+"data/ctrl/status.dat");
}
// update HW on reqest
void TDtBehav::updateHW(void)
{
  qDebug()<<"Update HW data";
  if(wrHWReq) {
// write into HW power on off
    //powerStatus=powerValue;    //for test only
    wrHWReq=false; // request compleated
    if(powerValue) {dev->usr_waitCanRead(); dev->usr_writeMode(3); } else dev->usr_writeMode(0);
    qDebug()<<"Set power on/off request. Set power value "<<powerValue;
  }
  if(wrDataReq and canWR){
// write data csv into HW
// read data automaticaly
    qDebug()<<"Write work file request. Set filename "<<selFile;
    wrDataReadData(true); // write and read data
    wrDataReq=false; // request compleated
    qDebug()<<"Meas request compleated";
  }
  else if(measReq and canWR){
    wrDataReadData(false);  // read measure data
    measReq=false; // request compleated
    qDebug()<<"Meas request compleated";
  }
}
//-------------------------------------------------------------------------------------------------
//--- Get Measure STARTMEASURE_STATE
//-------------------------------------------------------------------------------------------------
void TDtBehav::wrDataReadData(bool withWr)
{

  if(withWr){
    dev->usr_pdOn(); //off ps
    qDebug()<<"Write "<<selFile;
    if(!selFile.size()){  statusReq="\"File absent\""; devError.setDevError(TDevErrors::INFILE_ERROR);return; }

    dev->usr_waitCanRead();
    if(dev->usr_WrDACFile(workDirName+"data/data/"+selFile)) {
      statusReq="\"FileA absent\""; devError.setDevError(TDevErrors::INFILE_ERROR);return;
    }
    qDebug()<<"Writeen "<<selFile;
    dev->usr_pdOff(); // on ps after write
    statusReq="\"Data written\"";
    //statusReq="\"2\"";
  }
  else {
    dev->usr_waitCanRead();
    if(dev->usr_RdADCFile(workDirName+"data/ctrl/meas.dat")) {
        statusReq="\"File absent\""; devError.setDevError(TDevErrors::INFILE_ERROR);return;
      }
    qDebug()<<"Meas compl"<<workDirName+"data/ctrl/meas.dat";
    canUpdateMData=true;
    statusReq="\"Data written\"";
    //statusReq="\"2\"";
  }

}


