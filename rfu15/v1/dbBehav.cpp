#include "dbBehav.h"

extern TSrvBehav *rfamp;
extern struct rfAmpSetData cur_par;

/*!
 * \brief TDbBehav::TDbBehav
 */
TDbBehav::TDbBehav(void)
{
  qDebug()<<"Start constructor of object TDbBehav";
  abort=false;
  haveErrMsg=false;
  haveStatusMsg=false;
  timer=new QTimer();
  qDebug()<<"Constructor of object TDbBehav was finished";
}

/*!
 * \brief TDbBehav::~TDbBehav
 */
TDbBehav::~TDbBehav()
{
  qDebug()<<"Start destructor of object TDbBehav";
  mutex.lock();
  setAbort(true);
  condition.wakeOne();
  mutex.unlock();
  wait(2000);
  if(isRunning()) {
    qDebug()<<"Object TDbBehav still running... terminate.";
    terminate();
    wait(2000);
  }
  timer->stop();
  qDebug()<<"Destructor of object TDbBehav was finished";
}


//-----------------------------------------------------------------------------
//--- Run process. Main cycle with state machine
//-----------------------------------------------------------------------------
void TDbBehav::run()
{
  qDebug()<<"Run cycle processing of object TDbBehav";
  QEventLoop *loop ;
  loop=new QEventLoop();
  unsigned int update;
  int writeHWReq=0;
  QObject::connect(rfamp,SIGNAL(signalErrMsg(QString)),this,SLOT(slotErrMsg(QString)));
  QObject::connect(rfamp,SIGNAL(signalStatusMsg(QString)),this,SLOT(slotStatusMsg(QString)));
  QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerTic()));

  while(!rfamp->getReadyHW()) { // wait ready HW
    loop->processEvents(); msleep(10);
    if(abort) break;
  }

  canTic=true;
  timerEv=false;
  timer->start(sampleInterval);

  while(!abort) { // run until not set abort
    loop->processEvents();
    mutex.lock();
    condition.wait(&mutex);
    mutex.unlock();
    if(rfamp) {
      if(rfamp->isFinished()) { // fatal error in HW
        qDebug()<<"Delete TOrbBehav object from TDbBehav::run() cycle";
        writeErr_DB((char*)"HW fatal"); // HW fatal error
        delete rfamp;
        rfamp=0; abort=true;
        break ;
      }
    }
    if(timerEv) {
      canTic=false;
      timerEv=false;
      if(db_process(&update)) {
        qDebug()<<"Get HW write request from DB";
        writeHWReq=1;
      }
      if(writeHWReq) {
        if(update) {
          qDebug()<<"Set HW request from DB";
          rfamp->writeHwReq();
          rfamp->endHwReq();
          qDebug()<<"HW request from DB compleate successfully!";
        }
        writeHWReq=0;
      }
      canTic=true;
    }
// processing error request
    if(haveErrMsg) {
      haveErrMsg=false;
      qDebug()<<"DB - Received error message:"<<errMsg;
      writeErr_DB((char*)errMsg.toAscii().data()); // current  error message
      sh_put_meas_er(0);  // подтверждение обновления измерений в БД
    }
// processing status request
    if(haveStatusMsg) {
      haveStatusMsg=false;
      qDebug()<<"DB - Received status message:"<<statusMsg;
      writeStatus_DB((char*)statusMsg.toAscii().data()); // current status message
      sh_put_meas_er(0);  // подтверждение обновления измерений в БД
    }
  } // end RUN cycle

  timer->stop();
  qDebug()<<"Run() of object TDbBehav was finished.";
  writeStatus_DB((char*)"End prog"); // clear status
  sh_put_mess((char*)"Задача srvRfU15 завершила работу.");
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
}

int TDbBehav::dbInit()
{
  unsigned int sts;
  sts=db_init(); ///< initialization of DB 0 - if Ok.
  return(sts);
}


void TDbBehav::processingData()
{

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public slots
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

// Error and status message processing
void TDbBehav::slotErrMsg(QString str)
{
  haveErrMsg=true;
  errMsg=str;
  condition.wakeOne();
}
void TDbBehav::slotStatusMsg(QString str)
{
  haveStatusMsg=true;
  statusMsg=str;
  condition.wakeOne();
}
