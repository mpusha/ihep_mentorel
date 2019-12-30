#include "dbBehav.h"

extern TOrbBehav *orb;


TDbBehav::TDbBehav(void)
{
  qDebug()<<"Start constructor of object TDbBehav";
  abort=false;
  haveErrMsg=false;
  haveStatusMsg=false;
  qDebug()<<"Constructor of object TDbBehav was finished";
  QObject::connect(orb,SIGNAL(signalErrMsg(QString)),this,SLOT(slotErrMsg(QString)));
  QObject::connect(orb,SIGNAL(signalStatusMsg(QString)),this,SLOT(slotStatusMsg(QString)));
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

//-----------------------------------------------------------------------------
//--- Run process. Main cycle with state machine
//-----------------------------------------------------------------------------
void TDbBehav::run()
{
  QEventLoop *loop ;
  loop= new QEventLoop();
  unsigned int update,newData=0;
  int writeHWReq=0;
  double field=0;
  char st=0;
  qDebug()<<"Run cycle processing of object TDbBehav";
  while(!abort) { // run until destructor not set abort
    //loop->processEvents();
    //msleep(300);
    mutex.lock();
    condition.wait(&mutex);
    if(orb) {
      if(orb->isFinished()) {
        qDebug()<<"Delete TOrbBehav object from TDbBehav::run() cycle";
        delete orb;
        orb=0; break ;
      }
    }

    //for(int i=0;i<30;i++)
    {
    //  loop->processEvents();
    //  msleep(10);
      if(haveErrMsg){
        haveErrMsg=false;
        qDebug()<<"==== DB ==== Get error message:"<<errMsg;
        // write into DB
      }
      if(haveStatusMsg){
        haveStatusMsg=false;
        qDebug()<<"==== DB ==== Get status message:"<<statusMsg;
        // write into DB
      }
    }
   mutex.unlock();
/*
    if(db_process(&update)) { // delay 300 ms
      writeHWReq=1;
    }
    if(writeHWReq){
      if(update){
        printf("Start write measH parameters update %0x\n",update);
  //      meas->writeHWReq(update);
        printf("All parameters  was writen successfull!\n");
      }
      writeHWReq=0;
    }
//    newData=meas->haveNewMeasDataReq();

    if(newData&1) {
  //    meas->readMeasReq(1,&st,&field);
      printf("ch1 st %0x data %e\n",st,field);
      set_state(1,&field,&st);
    }
    if(newData&2) {
    //  meas->readMeasReq(2,&st,&field);
      printf("ch2 st %0x data %e\n",st,field);
      set_state(2,&field,&st);
    }
*/
  }

  qDebug()<<"Run() processing of object TDbBehav was finished";
}

unsigned int TDbBehav::dbInit()
{
  unsigned int sts=db_init();//initialization of DB 0 - if Ok.
  if(sts==0) sh_put_mess((char*)"Task measH is starting.");
  return(sts);
}
//-------------------------SLOT--------------------
void TDbBehav::slotErrMsg(QString str)
{
  haveErrMsg=true;
  errMsg=str;
  condition.wakeOne();
}
//-------------------------SLOT--------------------
void TDbBehav::slotStatusMsg(QString str)
{
  haveStatusMsg=true;
  statusMsg=str;
  condition.wakeOne();
}
