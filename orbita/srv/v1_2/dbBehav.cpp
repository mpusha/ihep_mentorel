#include "dbBehav.h"

extern TOrbBehav *orb;
extern struct orbSetData cur_par;

TDbBehav::TDbBehav(void)
{
  qDebug()<<"Start constructor of object TDbBehav";
  abort=false;
  haveErrStatusMsg=false;
  statusDB=dbInit(); // if 0 - OK
  qDebug()<<"Constructor of object TDbBehav was finished";
  //timer = new QTimer(this);
  timerEv=false;
  timer=new QTimer();
  dateTime_cur="                ";
}

TDbBehav::~TDbBehav()
{
  qDebug()<<"Start destructor of object TDbBehav";
  mutex.lock();
  setAbort(true);
  condition.wakeOne();
  mutex.unlock();
  wait(2000);
  if(isRunning()) {
    terminate();
    wait(2000);
  }
  qDebug()<<"Destructor of object TDbBehav was finished";
}
//On timer event
void TDbBehav::slotTimerTic(void)
{
  if(canTic){
    mutex.lock();
    timerEv=true;
    condition.wakeOne();
    mutex.unlock();
  }
}

//-----------------------------------------------------------------------------
//--- Run process. Main cycle with state machine
//-----------------------------------------------------------------------------
void TDbBehav::run()
{
  QEventLoop *loop ;
  loop= new QEventLoop();
  unsigned int update;
  int realStart,realStop,realMath=0,viewWWW=1;
  int writeHWReq=0;
  if(!orb) return;
  QObject::connect(orb,SIGNAL(signalErrStatusMsg(QString)),this,SLOT(slotErrStatusMsg(QString)));
  QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerTic()));
  qDebug()<<"Run cycle processing of object TDbBehav";
  while(!orb->getReadyHW()) {loop->processEvents(); msleep(10);} // wait ready HW

  orb->getAbsentOs(absR,absZ,&allValidR,&allValidZ);
  if(writeAbsAxes_DB(absR,absZ,sizeof(absR))) abort=true; //error in DB
  int cy=0;
  canTic=true;
  timer->start(sampleInterval);
  while(!abort) { // run until destructor not set abort
    loop->processEvents();
    mutex.lock();
    condition.wait(&mutex);
    mutex.unlock();
    if(orb) {
      if(orb->isFinished()) {
        qDebug()<<"Delete TOrbBehav object from TDbBehav::run() cycle";
        delete orb;
        orb=0; abort=true;
        break ;
      }
    }
    //if(!((cy++)%10)) qDebug()<<"==== DB ==== TIMER"<<cy;
    //loop->processEvents();
    if(timerEv){
      canTic=false;
      //timer->stop();
      //if((cy++)%5)
      //qDebug()<<"==== DB ==== Timer event"<<cy;
      timerEv=false;
      if(db_process(&update,&realStart,&realStop)) {
        qDebug()<<"HW write request";
        writeHWReq=1;
      }
      if(writeHWReq){
        if(update){
          if(update&0xfd7f){ //write in HW if data not change graph or math
            printf("Start write Orb parameters update %0x\n",update);
            u_int8_t reg=cur_par.single|((cur_par.work&0x1)<<1)|((cur_par.dir&0x1)<<2)|((cur_par.launch&0x1)<<3)|
                         ((cur_par.mode&0x3)<<4);
          //request for write parameers
            while(orb->writeToTmpDataRequest(cur_par.ti,reg,cur_par.tresholdI,realStart,realStop)) msleep(1);
          }
          if(update&0x200) viewWWW=1;
          if(update&0x0080){
            if(cur_par.math==0){ //clear mem
              clearOrbAndSave();
              realMath=1;
              viewWWW=1;
            }
            else if(cur_par.math==1){ //remember
              rememberOrbAndSave();
              realMath=2;
              viewWWW=1;
            }
            else if(cur_par.math==2){ //math set cur-mem
              if(!cur_par.single) { mathOrbAndSave(0); viewWWW=1; }
              realMath=3;
            }
            else if(cur_par.math==3){ //math set cur-prev
              if(!cur_par.single) { mathOrbAndSave(1); viewWWW=1; }
              realMath=4;
            }
          }
          printf("All parameters  was writen successfull!\n");
        }
        writeHWReq=0;
      }
      if(orb->getReadyMeasure()){

        qDebug()<<"DB - Meas ready";

        if(savePrevMeas()) abort=true;
// get curren measure
        while(orb->getDataRequest()) msleep(1);
        while(orb->getReadyMeasure()) msleep(2);
        orb->readMeasure(mTime_cur,tresholdI_cur,&regime_cur,oR_cur,oZ_cur);
        qDebug()<<"DB - Meas has being reading";
        qDebug()<<"DB - Updata DB begin";

// begin write data into current DB --------------------------------------------------------------------------------------------------
        if(saveCurrMeas()) abort=true;
// end write data into current DB --------------------------------------------------------------------------------------------------
        /*if(realMath==1){
          clearOrbAndSave();
          realMath=0;
        }
        else
        if(realMath==2){
          rememberOrbAndSave();
          realMath=0;
        }
        else*/
        if(realMath>2)
          mathOrbAndSave(realMath-3);
        viewWWW=1;
        qDebug()<<"DB - Updata DB end";
      }
      if(viewWWW){
//        prepDataWWWSts("/mnt/ramdisk/status.dat");
        //if(!prepDataWWWSts("/mnt/ramdisk/status.dat")) {
          qDebug()<<"WWW - Updata date start";
          if(cur_par.graph==0){ // prepere for view of current orbit
            prepDataWWWOrb(oR_cur,"/mnt/ramdisk/orbitr.dat");
            prepDataWWWOrb(oZ_cur,"/mnt/ramdisk/orbitz.dat");
            prepDataWWWHarm(mTime_cur,hS9R_cur,hC9R_cur,hS10R_cur,hC10R_cur,hS11R_cur,hC11R_cur,radiusR_cur,rmsR_cur,"/mnt/ramdisk/harmr.dat");
            prepDataWWWHarm(mTime_cur,hS9Z_cur,hC9Z_cur,hS10Z_cur,hC10Z_cur,hS11Z_cur,hC11Z_cur,radiusZ_cur,rmsZ_cur,"/mnt/ramdisk/harmz.dat");
            prepDataWWWEndRequest();
          }
          else if(cur_par.graph==1){// prepere for view of previous orbit
            prepDataWWWOrb(oR_last,"/mnt/ramdisk/orbitr.dat");
            prepDataWWWOrb(oZ_last,"/mnt/ramdisk/orbitz.dat");
            prepDataWWWHarm(mTime_last,hS9R_last,hC9R_last,hS10R_last,hC10R_last,hS11R_last,hC11R_last,radiusR_last,rmsR_last,"/mnt/ramdisk/harmr.dat");
            prepDataWWWHarm(mTime_last,hS9Z_last,hC9Z_last,hS10Z_last,hC10Z_last,hS11Z_last,hC11Z_last,radiusZ_last,rmsZ_last,"/mnt/ramdisk/harmz.dat");
            prepDataWWWEndRequest();
          }
          else if(cur_par.graph==2){// prepere for view of math processing
            if((realMath==3)||(realMath==4)){
              prepDataWWWOrb(oR_math,"/mnt/ramdisk/orbitr.dat");
              prepDataWWWOrb(oZ_math,"/mnt/ramdisk/orbitz.dat");
              prepDataWWWHarm(mTime_math,hS9R_math,hC9R_math,hS10R_math,hC10R_math,hS11R_math,hC11R_math,radiusR_math,rmsR_math,"/mnt/ramdisk/harmr.dat");
              prepDataWWWHarm(mTime_math,hS9Z_math,hC9Z_math,hS10Z_math,hC10Z_math,hS11Z_math,hC11Z_math,radiusZ_math,rmsZ_math,"/mnt/ramdisk/harmz.dat");
              prepDataWWWEndRequest();
            }
            else {
              prepDataWWWOrb(oR_mem,"/mnt/ramdisk/orbitr.dat");
              prepDataWWWOrb(oZ_mem,"/mnt/ramdisk/orbitz.dat");
              prepDataWWWHarm(mTime_mem,hS9R_mem,hC9R_mem,hS10R_mem,hC10R_mem,hS11R_mem,hC11R_mem,radiusR_mem,rmsR_mem,"/mnt/ramdisk/harmr.dat");
              prepDataWWWHarm(mTime_mem,hS9Z_mem,hC9Z_mem,hS10Z_mem,hC10Z_mem,hS11Z_mem,hC11Z_mem,radiusZ_mem,rmsZ_mem,"/mnt/ramdisk/harmz.dat");
              prepDataWWWEndRequest();
            }
          }
          prepDataWWWSts("/mnt/ramdisk/status.dat");
          viewWWW=0;
          qDebug()<<"WWW - Updata date end";
        //}
      }
      canTic=true;
      orb->usr_sysLedOff();
      if(!((cy++)%20)) orb->usr_sysLedOn();
      //timer->start(sampleInterval);
    }
// processing status or error request
    if(haveErrStatusMsg){
      haveErrStatusMsg=false;
      msg_cur=errStatusMsg;
      qDebug()<<"DB - Recieved error or status message:"<<msg_cur;
      writeErrState_DB(1,msg_cur.toAscii().data(),msg_cur.size()); // current status error
      sh_put_meas_er(0);  // подтверждение обновления измерений в БД
    }
  }
  if(msg_cur.at(0)!='E') {
    msg_cur="Program is stop ";
    writeErrState_DB(1,msg_cur.toAscii().data(),msg_cur.size()); // current status error
    sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  }
  qDebug()<<"Run() processing of object TDbBehav was finished";
}

unsigned int TDbBehav::dbInit()
{
  unsigned int sts=db_init();//initialization of DB 0 - if Ok.
  if(sts==0) sh_put_mess((char*)"Task srvOrbita is starting.");
  return(sts);
}
//-------------------------SLOT--------------------
void TDbBehav::slotErrStatusMsg(QString str)
{
  haveErrStatusMsg=true;
  errStatusMsg=str;
  condition.wakeOne();
}

// calculate orbit data
void TDbBehav::processingData(int allValid,double data[allAxis][timePoints],
                              double radius[timePoints],double Sin9[],double Cos9[],double Sin10[],double Cos10[],
                              double Sin11[],double Cos11[],double Disp[], double Rms[])
{
  double dd;
  double R,sin9,sin10,sin11,cos9,cos10,cos11,disp;
  for(int i=0;i<timePoints;i++){
    R=0;sin9=0;sin10=0;sin11=0;cos9=0;cos10=0;cos11=0;
    for(int j=0;j<allAxis;j++) {
      dd=data[j][i];
      R+=dd;
      sin9 +=dd*sin(9*2*M_PI*j/allAxis);  cos9 +=dd*cos(9*2*M_PI*j/allAxis);
      sin10+=dd*sin(10*2*M_PI*j/allAxis); cos10+=dd*cos(10*2*M_PI*j/allAxis);
      sin11+=dd*sin(11*2*M_PI*j/allAxis); cos11+=dd*cos(11*2*M_PI*j/allAxis);
    }
    R=R/allValid;
    radius[i]=R;
    Sin9[i] =sin9*2/allValid;  Cos9[i] =cos9*2/allValid;
    Sin10[i]=sin10*2/allValid; Cos10[i]=cos10*2/allValid;
    Sin11[i]=sin11*2/allValid; Cos11[i]=cos11*2/allValid;
    disp=0;
    for(int k=0;k<allAxis;k++) { dd=data[k][i]; disp+=(dd-R)*(dd-R);}
    disp=disp/allValid;
    Disp[i]=disp;
    Rms[i]=sqrt(disp);
  }
}

// create data for previous measure
int TDbBehav::saveCurrMeas(void)
{
  qDebug()<<"DB - Save current orbit";
  dt=QDateTime::currentDateTime();
  dateTime_cur=dt.toString("dd.MM.yyhh:mm:ss");
  if(writeErrState_DB(2,dateTime_cur.toAscii().data(),dateTime_cur.size())) return(1); // current data/time

  if(writeOrbData_DB(4,oR_cur)) return(1); //current Orbita R data
  if(writeOrbData_DB(7,oZ_cur)) return(1); //current Orbita Z data

//calk orbits val R direction
  processingData(allValidR,oR_cur,radiusR_cur,hS9R_cur,hC9R_cur,hS10R_cur,hC10R_cur,hS11R_cur,hC11R_cur,dispR_cur,rmsR_cur);
//calk orbits val Z direction
  processingData(allValidZ,oZ_cur,radiusZ_cur,hS9Z_cur,hC9Z_cur,hS10Z_cur,hC10Z_cur,hS11Z_cur,hC11Z_cur,dispZ_cur,rmsZ_cur);
// R orbit current parameters
  if(writeOrbParamData_DB(10,1,mTime_cur,sizeof(mTime_cur)))return(1); //current measure time R
  if(writeOrbParamData_DB(10,2,hS9R_cur,sizeof(hS9R_cur)))return(1);   // harm 9 sin current
  if(writeOrbParamData_DB(10,3,hC9R_cur,sizeof(hC9R_cur)))return(1);   // harm 9 cos current
  if(writeOrbParamData_DB(10,4,hS10R_cur,sizeof(hS10R_cur)))return(1); // harm 9 sin current
  if(writeOrbParamData_DB(10,5,hC10R_cur,sizeof(hC10R_cur)))return(1); // harm 9 cos current
  if(writeOrbParamData_DB(10,6,hS11R_cur,sizeof(hS11R_cur)))return(1); // harm 9 sin current
  if(writeOrbParamData_DB(10,7,hC11R_cur,sizeof(hC11R_cur)))return(1); // harm 9 cos current
  if(writeOrbParamData_DB(10,8,radiusR_cur,sizeof(radiusR_cur)))return(1);   // Radius current
  if(writeOrbParamData_DB(10,9,dispR_cur,sizeof(dispR_cur)))return(1);   // Dispersion current
  if(writeOrbParamData_DB(10,10,rmsR_cur,sizeof(rmsR_cur)))return(1);   // RMS current
  if(writeOrbParamTH_DB(10,11,tresholdI_cur,sizeof(tresholdI_cur)))return(1); // treshold current
// Z orbit current parameters
  if(writeOrbParamData_DB(13,1,mTime_cur,sizeof(mTime_cur)))return(1); //current measure time Z
  if(writeOrbParamData_DB(13,2,hS9Z_cur,sizeof(hS9Z_cur)))return(1);   // harm 9 sin current
  if(writeOrbParamData_DB(13,3,hC9Z_cur,sizeof(hC9Z_cur)))return(1);   // harm 9 cos current
  if(writeOrbParamData_DB(13,4,hS10Z_cur,sizeof(hS10Z_cur)))return(1); // harm 9 sin current
  if(writeOrbParamData_DB(13,5,hC10Z_cur,sizeof(hC10Z_cur)))return(1); // harm 9 cos current
  if(writeOrbParamData_DB(13,6,hS11Z_cur,sizeof(hS11Z_cur)))return(1); // harm 9 sin current
  if(writeOrbParamData_DB(13,7,hC11Z_cur,sizeof(hC11Z_cur)))return(1); // harm 9 cos current
  if(writeOrbParamData_DB(13,8,radiusZ_cur,sizeof(radiusZ_cur)))return(1);   // Radius current
  if(writeOrbParamData_DB(13,9,dispZ_cur,sizeof(dispZ_cur)))return(1);   // Dispersion current
  if(writeOrbParamData_DB(13,10,rmsZ_cur,sizeof(rmsZ_cur)))return(1);   // RMS current
  if(writeOrbParamTH_DB(13,11,tresholdI_cur,sizeof(tresholdI_cur)))return(1); // treshold current

  if(writeCntUD_DB(&cycle_cur)) return(1);
  return 0;
}

// create data for previous measure
int TDbBehav::savePrevMeas(void)
{
  qDebug()<<"DB - Save previous orbit";
  dateTime_last=dateTime_cur;
  msg_last=msg_cur;
  cycle_last=cycle_cur;
  for(int i=0;i<allAxis;i++)
    for(int j=0;j<timePoints;j++){
      oR_last[i][j]=oR_cur[i][j];
      oZ_last[i][j]=oZ_cur[i][j];
    }
  for(int i=0;i<timePoints;i++){
    mTime_last[i]=mTime_cur[i];
    tresholdI_last[i]=tresholdI_cur[i];
    hS9R_last[i]=hS9R_cur[i]; hS10R_last[i]=hS10R_cur[i]; hS11R_last[i]=hS11R_cur[i];
    hC9R_last[i]=hC9R_cur[i]; hC10R_last[i]=hC10R_cur[i]; hC11R_last[i]=hC11R_cur[i];
    radiusR_last[i]=radiusR_cur[i]; dispR_last[i]=dispR_cur[i];rmsR_last[i]=rmsR_cur[i];
    hS9Z_last[i]=hS9Z_cur[i]; hS10Z_last[i]=hS10Z_cur[i]; hS11Z_last[i]=hS11Z_cur[i];
    hC9Z_last[i]=hC9Z_cur[i]; hC10Z_last[i]=hC10Z_cur[i]; hC11Z_last[i]=hC11Z_cur[i];
    radiusZ_last[i]=radiusZ_cur[i]; dispZ_last[i]=dispZ_cur[i];rmsZ_last[i]=rmsZ_cur[i];
  }

  if(writeErrState_DB(3,msg_last.toAscii().data(),msg_last.size()))return(1); // previous status error
  if(writeErrState_DB(4,dateTime_last.toAscii().data(),dateTime_last.size())) return(1); // previous data/time

  if(writeOrbData_DB(5,oR_last)) return(1); //previous Orbita R data
  if(writeOrbData_DB(8,oZ_last)) return(1); //previous  Orbita Z data

  if(writeOrbParamData_DB(11,1,mTime_last,sizeof(mTime_last)))return(1); //previous measure time R
  if(writeOrbParamData_DB(14,1,mTime_last,sizeof(mTime_last)))return(1); //previous measure time Z
// R orbit previous parameters
  if(writeOrbParamData_DB(11,2,hS9R_last,sizeof(hS9R_last)))return(1);   // harm 9 sin previous
  if(writeOrbParamData_DB(11,3,hC9R_last,sizeof(hC9R_last)))return(1);   // harm 9 cos previous
  if(writeOrbParamData_DB(11,4,hS10R_last,sizeof(hS10R_last)))return(1); // harm 9 sin previous
  if(writeOrbParamData_DB(11,5,hC10R_last,sizeof(hC10R_last)))return(1); // harm 9 cos previous
  if(writeOrbParamData_DB(11,6,hS11R_last,sizeof(hS11R_last)))return(1); // harm 9 sin previous
  if(writeOrbParamData_DB(11,7,hC11R_last,sizeof(hC11R_last)))return(1); // harm 9 cos previous
  if(writeOrbParamData_DB(11,8,radiusR_last,sizeof(radiusR_last)))return(1);   // Radius previous
  if(writeOrbParamData_DB(11,9,dispR_last,sizeof(dispR_last)))return(1);   // Dispersion previous
  if(writeOrbParamData_DB(11,10,rmsR_last,sizeof(rmsR_last)))return(1);   // RMS previous
  if(writeOrbParamTH_DB(11,11,tresholdI_last,sizeof(tresholdI_last)))return(1); // treshold previous

// Z orbit previous parameters
  if(writeOrbParamData_DB(14,2,hS9Z_last,sizeof(hS9Z_last)))return(1);   // harm 9 sin previous
  if(writeOrbParamData_DB(14,3,hC9Z_last,sizeof(hC9Z_last)))return(1);   // harm 9 cos previous
  if(writeOrbParamData_DB(14,4,hS10Z_last,sizeof(hS10Z_last)))return(1); // harm 9 sin previous
  if(writeOrbParamData_DB(14,5,hC10Z_last,sizeof(hC10Z_last)))return(1); // harm 9 cos previous
  if(writeOrbParamData_DB(14,6,hS11Z_last,sizeof(hS11Z_last)))return(1); // harm 9 sin previous
  if(writeOrbParamData_DB(14,7,hC11Z_last,sizeof(hC11Z_last)))return(1); // harm 9 cos previous
  if(writeOrbParamData_DB(14,8,radiusZ_last,sizeof(radiusZ_last)))return(1);   // Radius previous
  if(writeOrbParamData_DB(14,9,dispZ_last,sizeof(dispZ_last)))return(1);   // Dispersion previous
  if(writeOrbParamData_DB(14,10,rmsZ_last,sizeof(rmsZ_last)))return(1);   // RMS previous
  if(writeOrbParamTH_DB(14,11,tresholdI_last,sizeof(tresholdI_last)))return(1); // treshold previous
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return(0);
}

// clear memory
int TDbBehav::clearOrbAndSave(void)
{
  qDebug()<<"DB - Clear mem";
  dt=QDateTime::currentDateTime();
  dateTime_mem=dt.toString("dd.MM.yyhh:mm:ss");
  msg_mem="                ";
  cycle_mem=0;
  for(int i=0;i<allAxis;i++)
    for(int j=0;j<timePoints;j++){
      oR_mem[i][j]=0;
      oZ_mem[i][j]=0;
    }
  for(int i=0;i<timePoints;i++){
    mTime_mem[i]=0;
    tresholdI_mem[i]=0;
    hS9R_mem[i]=0; hS10R_mem[i]=0; hS11R_mem[i]=0;
    hC9R_mem[i]=0; hC10R_mem[i]=0; hC11R_mem[i]=0;
    radiusR_mem[i]=0; dispR_mem[i]=0;rmsR_mem[i]=0;
    hS9Z_mem[i]=0; hS10Z_mem[i]=0; hS11Z_mem[i]=0;
    hC9Z_mem[i]=0; hC10Z_mem[i]=0; hC11Z_mem[i]=0;
    radiusZ_mem[i]=0; dispZ_mem[i]=0;rmsZ_mem[i]=0;
  }

  if(writeErrState_DB(5,msg_mem.toAscii().data(),msg_mem.size()))return(1); // math status error
  if(writeErrState_DB(6,dateTime_mem.toAscii().data(),dateTime_mem.size())) return(1); // math data/time

  if(writeOrbData_DB(6,oR_mem)) return(1); //mem  Orbita R data
  if(writeOrbData_DB(9,oZ_mem)) return(1); //mem  Orbita Z data

// R orbit previous parameters
  if(writeOrbParamData_DB(12,1,mTime_mem,sizeof(mTime_mem)))return(1); //mem measure time R
  if(writeOrbParamData_DB(12,2,hS9R_mem,sizeof(hS9R_mem)))return(1);   // harm 9 sin mem
  if(writeOrbParamData_DB(12,3,hC9R_mem,sizeof(hC9R_mem)))return(1);   // harm 9 cos mem
  if(writeOrbParamData_DB(12,4,hS10R_mem,sizeof(hS10R_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(12,5,hC10R_mem,sizeof(hC10R_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(12,6,hS11R_mem,sizeof(hS11R_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(12,7,hC11R_mem,sizeof(hC11R_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(12,8,radiusR_mem,sizeof(radiusR_mem)))return(1);   // Radius mem
  if(writeOrbParamData_DB(12,9,dispR_mem,sizeof(dispR_mem)))return(1);   // Dispersion mem
  if(writeOrbParamData_DB(12,10,rmsR_mem,sizeof(rmsR_mem)))return(1);   // RMS mem
  if(writeOrbParamTH_DB(12,11,tresholdI_mem,sizeof(tresholdI_mem)))return(1); // treshold mem

// Z orbit previous parameters
  if(writeOrbParamData_DB(15,1,mTime_mem,sizeof(mTime_mem)))return(1); //mem measure time Z
  if(writeOrbParamData_DB(15,2,hS9Z_mem,sizeof(hS9Z_mem)))return(1);   // harm 9 sin mem
  if(writeOrbParamData_DB(15,3,hC9Z_mem,sizeof(hC9Z_mem)))return(1);   // harm 9 cos mem
  if(writeOrbParamData_DB(15,4,hS10Z_mem,sizeof(hS10Z_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(15,5,hC10Z_mem,sizeof(hC10Z_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(15,6,hS11Z_mem,sizeof(hS11Z_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(15,7,hC11Z_mem,sizeof(hC11Z_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(15,8,radiusZ_mem,sizeof(radiusZ_mem)))return(1);   // Radius mem
  if(writeOrbParamData_DB(15,9,dispZ_mem,sizeof(dispZ_mem)))return(1);   // Dispersion mem
  if(writeOrbParamData_DB(15,10,rmsZ_mem,sizeof(rmsZ_mem)))return(1);   // RMS mem
  if(writeOrbParamTH_DB(15,11,tresholdI_mem,sizeof(tresholdI_mem)))return(1); // treshold mem
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return(0);
}

int TDbBehav::rememberOrbAndSave(void)
{
  qDebug()<<"DB - Remember orbit";
  dt=QDateTime::currentDateTime();
  dateTime_mem=dt.toString("dd.MM.yyhh:mm:ss");
  msg_mem=msg_cur;
  cycle_mem=cycle_cur;
  for(int i=0;i<allAxis;i++)
    for(int j=0;j<timePoints;j++){
      oR_mem[i][j]=oR_cur[i][j];
      oZ_mem[i][j]=oZ_cur[i][j];
    }
  for(int i=0;i<timePoints;i++){
    mTime_mem[i]=mTime_cur[i];
    tresholdI_mem[i]=tresholdI_cur[i];
    hS9R_mem[i]=hS9R_cur[i]; hS10R_mem[i]=hS10R_cur[i]; hS11R_mem[i]=hS11R_cur[i];
    hC9R_mem[i]=hC9R_cur[i]; hC10R_mem[i]=hC10R_cur[i]; hC11R_mem[i]=hC11R_cur[i];
    radiusR_mem[i]=radiusR_cur[i]; dispR_mem[i]=dispR_cur[i];rmsR_mem[i]=rmsR_cur[i];
    hS9Z_mem[i]=hS9Z_cur[i]; hS10Z_mem[i]=hS10Z_cur[i]; hS11Z_mem[i]=hS11Z_cur[i];
    hC9Z_mem[i]=hC9Z_cur[i]; hC10Z_mem[i]=hC10Z_cur[i]; hC11Z_mem[i]=hC11Z_cur[i];
    radiusZ_mem[i]=radiusZ_cur[i]; dispZ_mem[i]=dispZ_cur[i];rmsZ_mem[i]=rmsZ_cur[i];
  }
 if(writeErrState_DB(5,msg_mem.toAscii().data(),msg_mem.size()))return(1); // math status error
  if(writeErrState_DB(6,dateTime_mem.toAscii().data(),dateTime_mem.size())) return(1); // math data/time

  if(writeOrbData_DB(6,oR_mem)) return(1); //mem  Orbita R data
  if(writeOrbData_DB(9,oZ_mem)) return(1); //mem  Orbita Z data

// R orbit previous parameters
  if(writeOrbParamData_DB(12,1,mTime_mem,sizeof(mTime_mem)))return(1); //mem measure time R
  if(writeOrbParamData_DB(12,2,hS9R_mem,sizeof(hS9R_mem)))return(1);   // harm 9 sin mem
  if(writeOrbParamData_DB(12,3,hC9R_mem,sizeof(hC9R_mem)))return(1);   // harm 9 cos mem
  if(writeOrbParamData_DB(12,4,hS10R_mem,sizeof(hS10R_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(12,5,hC10R_mem,sizeof(hC10R_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(12,6,hS11R_mem,sizeof(hS11R_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(12,7,hC11R_mem,sizeof(hC11R_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(12,8,radiusR_mem,sizeof(radiusR_mem)))return(1);   // Radius mem
  if(writeOrbParamData_DB(12,9,dispR_mem,sizeof(dispR_mem)))return(1);   // Dispersion mem
  if(writeOrbParamData_DB(12,10,rmsR_mem,sizeof(rmsR_mem)))return(1);   // RMS mem
  if(writeOrbParamTH_DB(12,11,tresholdI_mem,sizeof(tresholdI_mem)))return(1); // treshold mem

// Z orbit previous parameters
  if(writeOrbParamData_DB(15,1,mTime_mem,sizeof(mTime_mem)))return(1); //mem measure time Z
  if(writeOrbParamData_DB(15,2,hS9Z_mem,sizeof(hS9Z_mem)))return(1);   // harm 9 sin mem
  if(writeOrbParamData_DB(15,3,hC9Z_mem,sizeof(hC9Z_mem)))return(1);   // harm 9 cos mem
  if(writeOrbParamData_DB(15,4,hS10Z_mem,sizeof(hS10Z_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(15,5,hC10Z_mem,sizeof(hC10Z_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(15,6,hS11Z_mem,sizeof(hS11Z_mem)))return(1); // harm 9 sin mem
  if(writeOrbParamData_DB(15,7,hC11Z_mem,sizeof(hC11Z_mem)))return(1); // harm 9 cos mem
  if(writeOrbParamData_DB(15,8,radiusZ_mem,sizeof(radiusZ_mem)))return(1);   // Radius mem
  if(writeOrbParamData_DB(15,9,dispZ_mem,sizeof(dispZ_mem)))return(1);   // Dispersion mem
  if(writeOrbParamData_DB(15,10,rmsZ_mem,sizeof(rmsZ_mem)))return(1);   // RMS mem
  if(writeOrbParamTH_DB(15,11,tresholdI_mem,sizeof(tresholdI_mem)))return(1); // treshold mem
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return(0);
}

int TDbBehav::mathOrbAndSave(int meas)
{
  qDebug()<<"DB - Math calk of orbit"<<meas;
  dt=QDateTime::currentDateTime();
  dateTime_math=dt.toString("dd.MM.yyhh:mm:ss");
  msg_math=msg_cur;
  cycle_math=cycle_cur;
  for(int i=0;i<timePoints;i++){
    mTime_math[i]=mTime_cur[i];
    tresholdI_math[i]=tresholdI_cur[i];
    }
  if(meas==0){
    for(int i=0;i<allAxis;i++)
      for(int j=0;j<timePoints;j++){
        oR_math[i][j]=oR_cur[i][j]-oR_mem[i][j];
        oZ_math[i][j]=oZ_cur[i][j]-oZ_mem[i][j];
      }
    for(int i=0;i<timePoints;i++){
      hS9R_math[i]=hS9R_cur[i]-hS9R_mem[i];          hS10R_math[i]=hS10R_cur[i]-hS10R_mem[i]; hS11R_math[i]=hS11R_cur[i]-hS11R_mem[i];
      hC9R_math[i]=hC9R_cur[i]-hC9R_mem[i];          hC10R_math[i]=hC10R_cur[i]-hC10R_mem[i]; hC11R_math[i]=hC11R_cur[i]-hC11R_mem[i];
      radiusR_math[i]=radiusR_cur[i]-radiusR_mem[i]; dispR_math[i]=dispR_cur[i]-dispR_mem[i]; rmsR_math[i]=rmsR_cur[i]-rmsR_mem[i];
      hS9Z_math[i]=hS9Z_cur[i]-hS9Z_mem[i];          hS10Z_math[i]=hS10Z_cur[i]-hS10Z_mem[i]; hS11Z_math[i]=hS11Z_cur[i]-hS11Z_mem[i];
      hC9Z_math[i]=hC9Z_cur[i]-hC9Z_cur[i];          hC10Z_math[i]=hC10Z_cur[i]-hC10Z_mem[i]; hC11Z_math[i]=hC11Z_cur[i]-hC11Z_mem[i];
      radiusZ_math[i]=radiusZ_cur[i]-radiusZ_cur[i]; dispZ_math[i]=dispZ_cur[i]-dispZ_mem[i];rmsZ_math[i]=rmsZ_cur[i]-rmsZ_mem[i];
    }
  }
  else{
    for(int i=0;i<allAxis;i++)
      for(int j=0;j<timePoints;j++){
        oR_math[i][j]=oR_cur[i][j]-oR_last[i][j];
        oZ_math[i][j]=oZ_cur[i][j]-oZ_last[i][j];
      }
    for(int i=0;i<timePoints;i++){
      hS9R_math[i]=hS9R_cur[i]-hS9R_last[i];          hS10R_math[i]=hS10R_cur[i]-hS10R_last[i]; hS11R_math[i]=hS11R_cur[i]-hS11R_last[i];
      hC9R_math[i]=hC9R_cur[i]-hC9R_last[i];          hC10R_math[i]=hC10R_cur[i]-hC10R_last[i]; hC11R_math[i]=hC11R_cur[i]-hC11R_last[i];
      radiusR_math[i]=radiusR_cur[i]-radiusR_last[i]; dispR_math[i]=dispR_cur[i]-dispR_last[i]; rmsR_math[i]=rmsR_cur[i]-rmsR_last[i];
      hS9Z_math[i]=hS9Z_cur[i]-hS9Z_last[i];          hS10Z_math[i]=hS10Z_cur[i]-hS10Z_last[i]; hS11Z_math[i]=hS11Z_cur[i]-hS11Z_last[i];
      hC9Z_math[i]=hC9Z_cur[i]-hC9Z_last[i];          hC10Z_math[i]=hC10Z_cur[i]-hC10Z_last[i]; hC11Z_math[i]=hC11Z_cur[i]-hC11Z_last[i];
      radiusZ_math[i]=radiusZ_cur[i]-radiusZ_last[i]; dispZ_math[i]=dispZ_cur[i]-dispZ_last[i];rmsZ_math[i]=rmsZ_cur[i]-rmsZ_last[i];
    }
  }
  if(writeErrState_DB(5,msg_math.toAscii().data(),msg_math.size()))return(1); // math status error
  if(writeErrState_DB(6,dateTime_math.toAscii().data(),dateTime_math.size())) return(1); // math data/time

  if(writeOrbData_DB(6,oR_math)) return(1); //math  Orbita R data
  if(writeOrbData_DB(9,oZ_math)) return(1); //math  Orbita Z data

// R orbit previous parameters
  if(writeOrbParamData_DB(12,1,mTime_math,sizeof(mTime_math)))return(1); //math measure time R
  if(writeOrbParamData_DB(12,2,hS9R_math,sizeof(hS9R_math)))return(1);   // harm 9 sin math
  if(writeOrbParamData_DB(12,3,hC9R_math,sizeof(hC9R_math)))return(1);   // harm 9 cos math
  if(writeOrbParamData_DB(12,4,hS10R_math,sizeof(hS10R_math)))return(1); // harm 9 sin math
  if(writeOrbParamData_DB(12,5,hC10R_math,sizeof(hC10R_math)))return(1); // harm 9 cos math
  if(writeOrbParamData_DB(12,6,hS11R_math,sizeof(hS11R_math)))return(1); // harm 9 sin math
  if(writeOrbParamData_DB(12,7,hC11R_math,sizeof(hC11R_math)))return(1); // harm 9 cos math
  if(writeOrbParamData_DB(12,8,radiusR_math,sizeof(radiusR_math)))return(1);   // Radius math
  if(writeOrbParamData_DB(12,9,dispR_math,sizeof(dispR_math)))return(1);   // Dispersion math
  if(writeOrbParamData_DB(12,10,rmsR_math,sizeof(rmsR_math)))return(1);   // RMS math
  if(writeOrbParamTH_DB(12,11,tresholdI_math,sizeof(tresholdI_math)))return(1); // treshold math

// Z orbit previous parameters
  if(writeOrbParamData_DB(15,1,mTime_math,sizeof(mTime_math)))return(1); //math measure time Z
  if(writeOrbParamData_DB(15,2,hS9Z_math,sizeof(hS9Z_math)))return(1);   // harm 9 sin math
  if(writeOrbParamData_DB(15,3,hC9Z_math,sizeof(hC9Z_math)))return(1);   // harm 9 cos math
  if(writeOrbParamData_DB(15,4,hS10Z_math,sizeof(hS10Z_math)))return(1); // harm 9 sin math
  if(writeOrbParamData_DB(15,5,hC10Z_math,sizeof(hC10Z_math)))return(1); // harm 9 cos math
  if(writeOrbParamData_DB(15,6,hS11Z_math,sizeof(hS11Z_math)))return(1); // harm 9 sin math
  if(writeOrbParamData_DB(15,7,hC11Z_math,sizeof(hC11Z_math)))return(1); // harm 9 cos math
  if(writeOrbParamData_DB(15,8,radiusZ_math,sizeof(radiusZ_math)))return(1);   // Radius math
  if(writeOrbParamData_DB(15,9,dispZ_math,sizeof(dispZ_math)))return(1);   // Dispersion math
  if(writeOrbParamData_DB(15,10,rmsZ_math,sizeof(rmsZ_math)))return(1);   // RMS math
  if(writeOrbParamTH_DB(15,11,tresholdI_math,sizeof(tresholdI_math)))return(1); // treshold mem
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return(0);
}

// prepare data for WWW
int TDbBehav::prepDataWWWSts(QString fileName)
{
//  if(QFileInfo("/mnt/ramdisk/update.req").exists()) return(1);
// write status
  QFile file(fileName);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return(1);
  QTextStream out(&file);
  QString B2KC1,gr,direction;
  if((regime_cur&0x30)==0x20) direction=" R/Z direction";
  else
    if(regime_cur&0x04) direction=" Z direction";
  else
    direction=" R direction";
  qDebug()<<"WWW - cur regime"<<regime_cur<<direction;
  if(cur_par.graph==0) gr=" current orbit";
  else if(cur_par.graph==1) gr=" previous orbit";
  else if(cur_par.graph==2) gr=" math processing of orbit";
  if(regime_cur&0x8) B2KC1="KC1"; else B2KC1="B2";
  QString kv="\"",cm=",";
  out<<"[{"<<"'status':"<<kv<<errStatusMsg<<kv<<cm<<"'start':"<<kv<<B2KC1<<kv<<cm<<"'datetime':"<<kv<<dt.toString("dd.MM.yy hh:mm:ss")<<kv<<cm<<
        "'update':"<<"1"<<cm<<"'graphview':"<<kv<<gr<<kv<<cm<<"'direction':"<<kv<<direction<<kv<<cm<<
        "\"T1\""<<":"<<mTime_cur[0]<<cm<<"\"T2\""<<":"<<mTime_cur[1]<<cm<<"\"T3\""<<":"<<mTime_cur[2]<<cm<<
        "\"T4\""<<":"<<mTime_cur[3]<<cm<<"\"T5\""<<":"<<mTime_cur[4]<<cm<<"\"T6\""<<":"<<mTime_cur[5]<<cm<<
        "\"T7\""<<":"<<mTime_cur[6]<<cm<<"\"T8\""<<":"<<mTime_cur[7]<<cm<<"\"T9\""<<":"<<mTime_cur[8]<<cm<<
        "\"T10\""<<":"<<mTime_cur[9]<<cm<<"\"T11\""<<":"<<mTime_cur[10]<<cm<<"\"T12\""<<":"<<mTime_cur[11]<<cm<<
        "\"T13\""<<":"<<mTime_cur[12]<<cm<<"\"T14\""<<":"<<mTime_cur[13]<<cm<<"\"T15\""<<":"<<mTime_cur[14]<<cm<<
        "\"T16\""<<":"<<mTime_cur[15]<<
        "}]";
  file.close();
  return(0);
}

void TDbBehav::prepDataWWWOrb(double orb[allAxis][timePoints], QString fileName)
{
// write orb data
  QFile fileOrb(fileName);
  if (!fileOrb.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug()<<"Can't create file on Ram disk"<<fileName;
    return;
  }
  QTextStream outOrb(&fileOrb);
  outOrb<<"axe,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16"<<'\n';
  for(int i=0;i<allAxis;i++){
    QString tmp=QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17").arg((i+1)*2).
               arg(orb[i][0],0,'f',1).arg(orb[i][1],0,'f',1).arg(orb[i][2],0,'f',1).arg(orb[i][3],0,'f',1).
               arg(orb[i][4],0,'f',1).arg(orb[i][5],0,'f',1).arg(orb[i][6],0,'f',1).arg(orb[i][7],0,'f',1).
               arg(orb[i][8],0,'f',1).arg(orb[i][9],0,'f',1).arg(orb[i][10],0,'f',1).arg(orb[i][11],0,'f',1).
               arg(orb[i][12],0,'f',1).arg(orb[i][13],0,'f',1).arg(orb[i][14],0,'f',1).arg(orb[i][15],0,'f',1);
    outOrb<<tmp<<'\n';
  }
  fileOrb.close();
}

void TDbBehav::prepDataWWWHarm(double t[],double s9[],double c9[],double s10[],double c10[],double s11[],double c11[],
                               double avr[],double rms[],QString fileName)
{
  QFile fileOrb(fileName);
  if (!fileOrb.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug()<<"Can't create file on Ram disk"<<fileName;
    return;
  }
  QTextStream outOrb(&fileOrb);
  outOrb<<"ti,sin9,cos9,sin10,cos10,sin11,cos11,avr,rms"<<'\n';
  for(int i=0;i<timePoints;i++){
    QString tmp=QString("%1,%2,%3,%4,%5,%6,%7,%8,%9").arg(t[i]).
               arg(s9[i],0,'f',1).arg(c9[i],0,'f',1).arg(s10[i],0,'f',1).arg(c10[i],0,'f',1).
               arg(s11[i],0,'f',1).arg(c11[i],0,'f',1).arg(avr[i],0,'f',1).arg(rms[i],0,'f',1);
    outOrb<<tmp<<'\n';
  }
  fileOrb.close();
}
void TDbBehav::prepDataWWWEndRequest(void)
{
  QFile req("/mnt/ramdisk/update.req");
  req.open(QIODevice::WriteOnly);
  req.close();
}
