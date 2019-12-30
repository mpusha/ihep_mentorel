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
  syslog(LOG_INFO,"Device destructor compleate.");
  qDebug()<<"Device destructor compleate.";
}

// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
  dbg.nospace() <<"STATE=";
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
    //GPIO test
    unsigned int GPIO1_17 = 49;   // GPIO1_28 = (1x32) + 28 = 60
    unsigned int GPIO3_19 = 115;   // GPIO0_15 = (0x32) + 15 = 15
    unsigned int value;
    gpio_export(GPIO1_17);    //
    gpio_export(GPIO3_19);   //
    gpio_set_dir(GPIO1_17, OUTPUT_PIN);   //
    gpio_set_dir(GPIO3_19, INPUT_PIN);   //
    gpio_set_value(GPIO1_17, HIGH);
   // gpio_get_value(GPIO3_19, &value);
    gpio_set_value(GPIO1_17, LOW);
    qDebug()<<"Alrm"<<value;
    gpio_unexport(GPIO1_17);     // unexport
    gpio_unexport(GPIO3_19);  // unexport

    int fd=setSpi(); qDebug()<<"spi init";
static u_int32_t bytes=0;
     u_int32_t awd[1024],ard[1024],*ar,*aw;
     aw=awd; ar=ard;
     for(int j=0;j<16;j++){
     for(int i=0;i<1024;i++){
int d;

     d=rand()%65534;
 //    write(fd,aw,2);
 //    read(fd,ar,2);
 //    if(awd!=ard){ pabort("ERROR");}
     // awd[i]=0xe00000|(0xffff) ;
awd[i]=0xc00000|d;
     //awd[i]=0x5000 ;
     //transfer(fd,aw,ar);
     //qDebug()<<(void *)d<<(void*)(ard&0xff);
    // if(d!=(ard[i]&0xff)){ pabort("ERROR") ; }
   //  transfer(fd,aw,ar);
    // awd=0x0040;awd=ard;
   //  transfer(fd,aw,ar);

    }
     transfer(fd,aw,ar);
qDebug()<<"transfer";

  //for(int i=0;i<1023;i++) //qDebug()<<"Send kwords"<<bytes<<"Wr"<<(awd[i]&0xffff)<<"Rd"<<(ard[i]&0xffff);
    //if((awd[i]&0xffff)!=(ard[i+1]&0xffff)){ pabort("ERROR") ; }
  }
     bytes+=16;
  qDebug()<<"Send kwords"<<bytes<<"Wr"<<(awd[1022]&0xffff)<<"Rd"<<(ard[1023]&0xffff);
  timerAlrm=false;
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
  CPhase deb=phase;//for debug only
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

// Sample device request from timer
      case GET_REQ_AND_STATUS_STATE: { // get requests from client and status from HW
        devError.clearDevError();
        getRequests(); // get request from client for write power/data and measure (get from settings)
        getStatus(); // get status from HW (signature, power, and window status for write data into HW)
        allStates[GET_REQ_AND_STATUS_STATE]=READY; // reset state
        stErr=devError.analyseError(); // set error, if present
        if(stErr) {
          timerAlrm=true; // can get data again
          syslog(LOG_ERR,"Get device status error.");qDebug()<<"Get device status error.";
          phase=DEVICE_ERROR_STATE;
          if(cntErrReq++>10) allStates[INITIAL_STATE]=INITIAL_STATE; // initial system again
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
        }
        else {
          allStates[UPDATE_STATUS_STATE]=UPDATE_STATUS_STATE ;
          phase=READY;
        }
        timerAlrm=true;
        break;
      }// end case STARTRUN_STATE
// connect to device and get simple information
      case INITIAL_STATE: { // connect to USB device
        devError.clearDevError(); // clear device error
        syslog(LOG_INFO,"Initialisation of device is begin."); qDebug()<<"Initialisation of device is begin.";
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
          phase = GET_REQ_AND_STATUS_STATE;
          syslog(LOG_INFO,"Initialisation of device was finished successfully.");qDebug()<<"Initialisation of device was finished successfully.";
        }
        break;
      }// end case INITIAL_STATE:

//  get info about device
      case UPDATE_STATUS_STATE: {
          updateStatus();
          allStates[UPDATE_STATUS_STATE]=READY;
          phase = READY;
        break;
      }//end case UPDATE_STATUS_STATE:

// processing hardware errors in device
      case DEVICE_ERROR_STATE:{
        syslog(LOG_ERR,"Found device error!"); qDebug()<<"Found device error!";
        QStringList err=devError.readDevTextErrorList();
        for(int i=0;i<err.size();i++){
          syslog(LOG_ERR,err.at(i).toAscii());qDebug()<<err.at(i);
        }
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

}
void TDtBehav::initialSystem(void)
{
// cope request data file from data into ctrl
  QFile fileOP(workDirName+"data/ctrl/update_power.req");
  if(!fileOP.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream outP(&fileOP);
  outP << "[request]"<<"\n"<<"power=0"<<"\n";
  fileOP.close();
  QFile fileI(workDirName+"data/data/wfile.dat");
  if (!fileI.open(QIODevice::ReadOnly | QIODevice::Text)) return;
  QTextStream in(&fileI);
  QString line = in.readLine();
  fileI.close();

  QFile fileO(workDirName+"data/ctrl/update_data.req");
  if(!fileO.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&fileO);
  out << "[request]"<<"\n"<<"fname="<<line<<"\n";
  fileO.close();
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
    if(selFile.size()) wrDataReq=true;
    if(wrDataReq) statusReq="\"Processing\"";
    setings.endGroup();
    QFile::remove(workDirName+"data/ctrl/update_data.req");
  }
  measReq=true;
}

void TDtBehav::getStatus()
{
// get status from HW
// get power On/Off get can read/write status and signature
  powerStatus=powerValue;    //for test only
  static int g=1; // debuging
  qDebug()<<"st"<<g;
  canWR=false;
  if(!(g++%10)) { canWR=true;}

}
void TDtBehav::updateStatus()
{ qDebug()<<"GET ST";
  if(QFileInfo(workDirName+"data/ctrl/status.dat").exists()) return;
  QFile fileO(workDirName+"data/ctrl/status");
  if(!fileO.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&fileO);
  QString pwN,errN;
  qDebug()<<statusReq<<powerStatus;
  if(powerStatus==1) pwN="\"On\"";else if(powerStatus==0)pwN="\"Off\""; else pwN="\"unknown\"";
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
    qDebug()<<"Set power on/off request. Set power value "<<powerValue;
  }
  if(wrDataReq and canWR){
// write data csv into HW
// read data automaticaly
    wrDataReadData(true); // write and read data
    wrDataReq=false; // request compleated
    qDebug()<<"Write work file request. Set filename "<<selFile;
    qDebug()<<"Meas request compleated";
    statusReq="\"Data written\"";
  }
  else if(measReq and canWR){
    wrDataReadData(false);  // read measure data
    //measReq=false; // request compleated
    qDebug()<<"Meas request compleated";
  }
}
//-------------------------------------------------------------------------------------------------
//--- Get Measure STARTMEASURE_STATE
//-------------------------------------------------------------------------------------------------
void TDtBehav::wrDataReadData(bool withWr)
{
  double a1,a2,a3;
  char c1,c2;
  if(withWr){
      qDebug()<<"Write "<<selFile;
    if(!selFile.size()){  devError.setDevError(TDevErrors::INFILE_ERROR);return; }
    int fd=setSpi();
    if(fd<0) { devError.setDevError(TDevErrors::RW_DATA_ERROR); return; }
    u_int16_t awd[2],ard[2],*ar,*aw;
    aw=awd; ar=ard;
    QFile fin(workDirName+"data/data/"+selFile);
    if(!fin.open(QIODevice::ReadOnly| QIODevice::Text)) {devError.setDevError(TDevErrors::INFILE_ERROR);return; }
    QTextStream in(&fin);
    while (!in.atEnd()){
      in.readLine();
      in>>a1>>c1>>a2>>c2>>a3;

      awd[0]=(u_int16_t) a2;
      awd[1]=(u_int16_t) a3;
      //awd[0]=0x4000;awd[1]=0x4000;
      //transfer(fd,aw,ar);
      //qDebug()<<a1<<"w0"<<awd[0]<<"R0"<<ard[0]<<"W1"<<awd[1]<<"R1"<<ard[1];
      //QString s=s1.readLine(); // reads line from file
      //firstColumn.append(s.split(",").first()); // appends first column to list, ',' is separator
    }
    fin.close();


    qDebug()<<"Writeen "<<selFile;
    closeSpi(fd);
  }
  canUpdateMData=true;
}


