
#include "timerBehave.h"
#include "timer.h"

extern QWaitCondition conditions;

//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------



TTimerBehave::TTimerBehave() : QThread()
{
  qDebug()<<"Start constructor of class TTimerBehav";
  workDir=QCoreApplication::applicationDirPath();
  abort=false;
  for(int i=0;i<ALLREQSTATES;i++) allStates[i]=IDLE;
  for(int i=0;i<MAX_NUM_TIMERS;i++) mapTimers[i]=false;
  phase = INITIAL_STATE;
  cntErrInitial=0;
  qDebug()<<"Constructor of class TTimerBehav was finished";
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TTimerBehave::~TTimerBehave()
{
  qDebug()<<"Start destructor of class TTimerBehav";
  gpio_unexport(PIN_SYSACT);
  if(timerHw) { delete timerHw; timerHw=0; }
  setAbort(true);
  wait(2000);
  if(isRunning()) {
    terminate();
    wait(2000);
  }
  qDebug()<<"Destructor of class TTimerBehav was finished";
}

// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
 // dbg.nospace() <<"\n---------------------------------\nSTATE=";
  dbg.nospace()<<"TIMER BEHAVE STATE=";
  switch(t){
  case IDLE: dbg.space() << "IDLE" ; break;
  case INITIAL_STATE: dbg.space() <<       "INITIAL_STATE" ; break;
  case DATAWRITE_STATE: dbg.space() <<    "DATAWRITE_STATE" ; break;
  case UPDATESTATUS_STATE: dbg.space() << "UPDATESTATUS_STATE" ; break;
  case GETDEVICEINFO_STATE: dbg.space() << "GETDEVICE_STATE" ; break;
  default:;
  }
  return dbg.nospace() ;//<< endl;;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Run thread cycle
void TTimerBehave::run()
{
  CPhase deb=IDLE;//for debug only

  QEventLoop *loop;
  loop=new QEventLoop();

  phase = INITIAL_STATE;
  msleep(1000);
  qDebug()<<"Run cycle processing of object TTimerBehave";

  gpio_export(PIN_SYSACT);
  gpio_set_dir(PIN_SYSACT,OUTPUT_PIN);
  if(xmlData.loadSetup(workDir+"/settings/setup.xml")) abort=true;
  timerHw=new TTimer(xmlData.getDevName(),xmlData.getSpeed());
  if(!timerHw) abort=true;
  int cnt=0;
  while(!abort){
    cnt++;
    cnt&=0x1f;
    if(cnt&0x10) gpio_set_value(PIN_SYSACT,HIGH); else gpio_set_value(PIN_SYSACT,LOW);  //LED SYSTEM on front panel
    msleep(50);
    loop->processEvents();
    if(phase==IDLE) {
      for(int i=0;i<ALLREQSTATES;i++) { //read all statese request and run if state!= READY state. high priority has low index
        if(allStates[i]!=IDLE){
          phase=allStates[i];
          allStates[i]=IDLE;
          break;
        }
      }
    }
    if(deb!=phase) { qDebug()<<phase;deb=phase;}
    switch(phase) {
      case IDLE: {
        if(QFileInfo("/mnt/ramdisk/writedata_req").exists()) {
          if(QFileInfo("/mnt/ramdisk/writedata_req").exists()) QFile::remove("/mnt/ramdisk/writedata_req");
          allStates[DATAWRITE_STATE]=DATAWRITE_STATE;
        }
        else if(QFileInfo("/mnt/ramdisk/status_req").exists()){
          if(QFileInfo("/mnt/ramdisk/status_req").exists()) QFile::remove("/mnt/ramdisk/status_req");
          allStates[UPDATESTATUS_STATE]=UPDATESTATUS_STATE;
        }
        else if(QFileInfo("/mnt/ramdisk/reload_req").exists()){
          if(QFileInfo("/mnt/ramdisk/reload_req").exists()) QFile::remove("/mnt/ramdisk/reload_req");
          allStates[INITIAL_STATE]=INITIAL_STATE;
        }
        else if(QFileInfo("/mnt/ramdisk/unlockIP_req").exists()){
          if(QFileInfo("/mnt/ramdisk/unlockIP_req").exists()) QFile::remove("/mnt/ramdisk/unlockIP_req");
          if(QFileInfo("/mnt/ramdisk/lockIP").exists()) QFile::remove("/mnt/ramdisk/lockIP");
        }
        break;
      }//end case IDLE:
// initial device on begin work
      case INITIAL_STATE: { //
        syslog(LOG_INFO,"Initialization of device is begin.");
        qDebug()<<"Initialization of device is begin.";
        initialDevice();
        if(stErr) {
          syslog(LOG_ERR,"Initialization of device error. Try again.");
          qDebug()<<"Initialization of device error. Try again.";
          allStates[INITIAL_STATE]=INITIAL_STATE;
          syslog(LOG_ERR,"Found device error! %s",timerHw->getStrErr(stErr));
          qDebug()<<"Found device error! "<<timerHw->getQStrErr(stErr);
          msleep(1000); //wait some time and try init another
          cntErrInitial++;
          if(cntErrInitial>5) {
            syslog(LOG_ERR,"Critical error of initialization device. %s. Programm terminated.",timerHw->getStrErr(stErr));
            qDebug()<<"Critical error of initialization device. "<<timerHw->getQStrErr(stErr)<<" Programm terminated. ";
            abort=true;
          }
        }
        else {
          cntErrInitial=0;
          allStates[INITIAL_STATE]=IDLE;
          allStates[GETDEVICEINFO_STATE]=GETDEVICEINFO_STATE;
          phase = IDLE;  
          syslog(LOG_INFO,"Initialization of device was finished successfully.");
          qDebug()<<"Initialization of device was finished successfully.";
        }
      break;
      }// end case INITIAL_STATE:
      case GETDEVICEINFO_STATE:{
        if((stErr=getDeviceInfo())) { // 3 low bits 0 - error xml 1 - more timers 2 - more channels
          abort=true;
          //if(QFileInfo("/mnt/ramdisk/status").exists()) QFile::remove("/mnt/ramdisk/status");
          syslog(LOG_INFO,"Input file %s was corrupted or absent %s.",xmlData.getfullWFileName().toAscii().data(),timerHw->getStrErr(stErr));
          qDebug()<<"Input file "<<xmlData.getfullWFileName()<<" was corrupted or absent."<<timerHw->getQStrErr(stErr);
          break;
        }
        phase=IDLE;
      break;
      }// end case GETDEVICEINFO_STATE
      case DATAWRITE_STATE:{
        allStates[DATAWRITE_STATE]=IDLE;
        if((stErr=writeData())){
          abort=true;
          syslog(LOG_INFO,"Input file %s was corrupted or absent %s.",xmlData.getfullWFileName().toAscii().data(),timerHw->getStrErr(stErr));
          qDebug()<<"Input file "<<xmlData.getfullWFileName()<<" was corrupted or absent."<<timerHw->getQStrErr(stErr);
        }
        phase=IDLE;
      break;
      }// end case DATAWRITE_STATE
      case UPDATESTATUS_STATE: {
        collectStatus();
        status.writeStatus();
        phase=IDLE;
        break;
      }// end case UPDATESTATUS_STATE:
      default:;
    } // end switch
  }//end while

  qDebug()<<"Run() processing of object TTimerBehave was finished";
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public functions
void TTimerBehave::initialDevice(void)
{
  stErr=timerHw->initHW();
}

int TTimerBehave::getDeviceInfo(void)
{
  if(xmlData.loadSetup(workDir+"/settings/setup.xml")) return ERR_FILE_SETUP_ABS; //Setup file absent error
  if(xmlData.load(workDir+"/settings/"+xmlData.getWorkFileName()+".xml")&0x7) // 3 low bits 0 - error xml 1 - more timers 2 - more channels
    return ERR_FILE_SETTINGS; //XML error
  allTimers=xmlData.getAllTimers();
  status.setAllTimers(allTimers);
  collectStatus();
  status.writeStatus();

  state.clearAll();
  state.setAllTimers(allTimers);
  state.setCtrl("Ready");
  for(int i=0;i<allTimers;i++){
    state.setState("undefined");
  }
  state.writeState();
  return ERR_NONE;
}

void TTimerBehave::collectStatus(void)
{
  status.clearAll();
  for(int i=0;i<allTimers;i++) {
    int addr=xmlData.tdata[i].address;
    if(xmlData.tdata[i].tenable) { // timer enable in xml file
      timerHw->setAddress(addr);
      int alive=timerHw->testAlive();
      //qDebug()<<alive;
      if(!alive) { //allive
      // qDebug()<<timerHw->sendReadRawCmd("GF");
      //msleep(10);
        status.setStatus("Ok "+timerHw->sendReadRawCmd("GF"));//get firmware version
        mapTimers[i]=true;
      }
      else {
        status.setStatus("<err style=color:red>off</err>");
        mapTimers[i]=false;
      }
    }
    else {
      status.setStatus("disable");
      mapTimers[i]=false;
    }
  }
}

//<err style=color:red>Error, "+timerHw->getQStrErr(cErr)+"</err>"
//write data into HW
int TTimerBehave::writeData(void)
{
  int xmlErr;
  QString strErr="";
  state.setCtrl("Writing data");
  state.writeState();
  // 3 low bits 0 - error xml 1 - more timers 2 - more channels
  if((xmlErr=xmlData.load(workDir+"/settings/"+xmlData.getWorkFileName()+".xml"))&0x7) {
    return ERR_FILE_SETTINGS; //XML error
  }
  allTimers=xmlData.getAllTimers();

  status.setAllTimers(allTimers);
  collectStatus();
  status.writeStatus();

  state.clearAll();
  state.setAllTimers(allTimers);
  for(int i=0;i<allTimers;i++){
    state.setState("undefined");
  }
  if(xmlErr){
//16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// 0  0 ------TI------  ------CH-----  -PR-  MT MC XML
    int pr=(xmlErr>>3)&0x3;   // parameters where was error: 0 - absent, 1 - time, 2 - width, 3 - enable
    int ch=(xmlErr>>5)&0x1f;  // number of channel where was error
    int ti=(xmlErr>>10)&0x1f; //  number of timer where was error
    switch(pr) {
    case 1:
      strErr=QString("server: Error setup parameter time on timer %1 Ch=%2").arg(ti).arg(ch);
    break;
    case 2:
      strErr=QString("server: Error setup parameter width on timer %1 Ch=%2").arg(ti).arg(ch);
    break;
      strErr=QString("server: Error setup parameter enable on timer %1 Ch=%2").arg(ti).arg(ch);
    case 3:
    break;
    }
    state.setCtrl(strErr);
    state.writeState();
    return ERR_NONE;
  }
  int err=0,en=0;
  QString preErr="";
  for(int i=0;i<allTimers;i++) {
    int addr=xmlData.tdata[i].address;
    if(xmlData.tdata[i].tenable) { // timer enable in xml file
      if(mapTimers[i]){ //timer On
        en=xmlData.mkDatFile("/mnt/ramdisk/timer.dat",i);
        if((err=timerHw->setAddress(addr))){
          if(err<ERR_UART_ABS) preErr="uC error: "; else preErr="server error: ";
          state.setState(i,preErr+"<err style=color:red>"+timerHw->getQStrErr(err)+"</err>");
          continue;
        }
        if((err=timerHw->writeFile((char*)"/mnt/ramdisk/timer.dat"))){
          if(err<ERR_UART_ABS) preErr="uC error: "; else preErr="server error: ";
          state.setState(i,preErr+"<err style=color:red>"+timerHw->getQStrErr(err)+"</err>");
          continue;
        }
        if((err=timerHw->writeRegMap(en))){
          if(err<ERR_UART_ABS) preErr="uC error: "; else preErr="server error: ";
          state.setState(i,preErr+"<err style=color:red>"+timerHw->getQStrErr(err)+"</err>");
          continue;
        }
        state.setState(i,"ok. Data writen");
      }
      else {
        state.setState(i,"offline. Data don't written");
      }
    }
    else {
      state.setState(i,"disable");
    }
  }
  if((err=timerHw->wideBandWrHW())){
    if(err<ERR_UART_ABS) preErr="uC error: "; else preErr="server error: ";
    state.setCtrl(preErr+"<err style=color:red>"+"can't write data. "+timerHw->getQStrErr(err)+"</err>");
  }
  else
    state.setCtrl("Write is done");
  state.writeState();
  return ERR_NONE;
}


void TStatus::writeStatus()
{
// write status info
// create lock file
  QFile fileLock("/mnt/ramdisk/lock_status");
  fileLock.open(QIODevice::WriteOnly); // Or QIODevice::ReadWrite
  fileLock.close();
  QFile file("/mnt/ramdisk/status");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&file);
  QString tmp="[{"; //start element

  for(int i=0;i<allTimers;i++){
    if(listStatuses.size()<=i) break;
    tmp+=QString("\"status_t%1\":\"%2\",").arg(i+1).arg(listStatuses.at(i));
  }
  //qDebug()<<tmp;
  tmp+="\"empty\":0}]"; //end element
  out<<tmp;
  file.close();
  // delete lock file
  if(QFileInfo("/mnt/ramdisk/lock_status").exists()) QFile::remove("/mnt/ramdisk/lock_status");
}

/*
  QString kv="\"",cm=",";
  out<<"[{"<<"'status_ctrl':" <<kv<<1<<kv<<cm
           <<"'status_t1':"   <<kv<<2<<kv<<cm
           <<"'status_t2':"   <<kv<<3<<kv<<cm
           <<"'status_write':"<<kv<<4<<kv
           <<"}]";
*/

void TState::writeState()
{
// write state info
// create lock file
  QFile fileLock("/mnt/ramdisk/lock_state");
  fileLock.open(QIODevice::WriteOnly); // Or QIODevice::ReadWrite
  fileLock.close();
  QFile file("/mnt/ramdisk/state");
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  QTextStream out(&file);
  QString tmp="[{"; //start element

  for(int i=0;i<allTimers;i++){
    if(listStates.size()<=i) break;
    tmp+=QString("\"state_t%1\":\"%2\",").arg(i+1).arg(listStates.at(i));
  }
  tmp+=QString("\"status_ctrl\":\"%1\"").arg(stateCtrl);
  tmp+="}]"; //end element
  out<<tmp;
  file.close();
 // qDebug()<<tmp;
  // delete lock file
  if(QFileInfo("/mnt/ramdisk/lock_state").exists()) QFile::remove("/mnt/ramdisk/lock_state");
}
