#ifndef TIMERBEHAVE_H
#define TIMERBEHAVE_H

#include <QtCore>
#include <QVector>

#include <syslog.h>

#include "settings.h"
#include "SimpleGPIO.h"
#include "timer.h"

#define SAMPLE_DEVICE 2000 // time of request device
#define PIN_SYSACT 117 // P2_25 GPIO3_21 OUTPUT system work indicator

typedef enum
{
  INITIAL_STATE,
  GETDEVICEINFO_STATE,
  UPDATESTATUS_STATE,
  DATAWRITE_STATE,
  ALLREQSTATES,
  IDLE
} CPhase;  // phases of state machine

class TState {
  public:
    TState(){ allTimers=0; }
    ~TState(){}
    void setAllTimers(int i) { allTimers=i; }
    void clearAll(void){ stateCtrl.clear();listStates.clear(); }
    void setCtrl(QString s) { stateCtrl=s; }
    void setState(QString s) { listStates.append(s); }
    void setState(int at,QString s){ listStates.replace(at,s); }
    void writeState(void);
  private:
    QString stateCtrl;
    QStringList listStates;
    int allTimers;
};
class TStatus {
  public:
    TStatus(){ allTimers=0; }
    ~TStatus(){}
    void setAllTimers(int i) { allTimers=i; }
    void clearAll(void) { listStatuses.clear(); }
    void setStatus(QString s) { listStatuses.append(s); }
    void writeStatus(void);
  private:
    QStringList listStatuses;
    int allTimers;
};

class TTimer;
class QThread;
class TTimerBehave : public QThread
{
  Q_OBJECT
public:
  TTimerBehave();
  ~TTimerBehave();
  void setAbort(bool a) { abort=a; }
  void initialDevice(void);
  int getDeviceInfo(void);
  int writeData(void);
  void collectStatus(void);
protected:
  void run();
signals:
  void signalErrStatusMsg(QString);
private:
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  TTimer *timerHw;
  int stErr; // errors of work with hw
  bool mapTimers[MAX_NUM_TIMERS];
  int cntErrInitial;
  bool abort;
  QString workDir;
  TSettings xmlData;
// timers data
  int allTimers; //read from XML
  TStatus status;
  TState state;
};

#endif // TIMERBEHAVE_H
