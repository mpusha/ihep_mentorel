#ifndef SRVBEHAV_H
#define SRVBEHAV_H

#include <syslog.h>
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QStringList>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QWaitCondition>
#include <QDataStream>
#include <QByteArray>
#include <QSettings>


#include"SimpleGPIO.h"
#include "progerror.h"
#include "spi.h"
#include "hw.h"

#define SAMPLE_DEVICE 500 // time of request device



typedef enum
{
  INITIAL_STATE,
  DEVICE_ERROR_STATE,
  GETINFO_STATE,
  GET_REQ_AND_STATUS_STATE,
  UPDATE_HW_STATE,
  UPDATE_STATUS_STATE,
  ALLREQSTATES,
  READY
}CPhase;  // phases of state machine

class TDtBehav : public QThread
{
  Q_OBJECT
public:
  TDtBehav(QString dirName);
  ~TDtBehav();

// working wit HW


  //QStringList canDevName;
  void setErrorSt(short int); // set errors in state of statemachine for all stadies

// function for work with device
  void initialDevice(void);
  void initialSystem(void);
  void getRequests();
  void getStatus();
  void updateStatus(void);
  void updateHW(void);
  void wrDataReadData(bool withWr);

// -- function for Run Checking

//---
public slots:
  void timeAlarm(void);

protected:
    void run();

private:
// Error processing
  THw *dev;
  TDevErrors devError;  // hardware error in bits represantation
  short int stErr; // errors of work with hw
  int cntErrReq;
// individual device parameters
  QString workDirName;
  int powerValue,powerStatus;
  QString selFile;
  QString statusReq;

// process state machine
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  bool abort,timerAlrm;
  bool wrHWReq,wrDataReq,measReq; // reqests for actions
  bool canWR,canUpdateMData;
  int hw;
  bool pulsePS; //detect finish pulse (can meas data)
  QTimer tAlrm;

  QMutex mutex;
  QWaitCondition condition;


// network strings processing
private slots:



};

#endif // SRVBEHAV_H

