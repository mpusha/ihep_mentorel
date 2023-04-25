#ifndef DBPROC_H
#define DBPROC_H

#include <syslog.h>
#include <QtCore/QCoreApplication>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include <QWaitCondition>

#include "srvBehav.h"
#include "db.h"

const int sampleInterval=200;
extern "C" int db_init(void);
extern "C" int db_process(unsigned int *update);
extern "C" int writeErr_DB(char data[]);
extern "C" int writeStatus_DB(char data[]);
extern "C" int writeTime_DB(void);
extern "C" int read_controls();

extern "C" short sh_put_meas_er(short er_code);
extern "C" short sh_put_data_er(short er_code);
extern "C" void sh_put_mess(char *mes);


class TDbBehav : public QThread
{
  Q_OBJECT

  public:
  TDbBehav(void) ; // constructor
  ~TDbBehav();
  void setAbort(bool a) { abort=a; }
  int dbInit(void);

  void processingData();

  protected:
    void run();

private:

// control var
  bool abort,timerEv,canTic;
  bool haveErrMsg,haveStatusMsg;
  QString errMsg,statusMsg;

//system var
  QMutex mutex;
  QWaitCondition condition;
  QTimer *timer;

public slots:
  void slotErrMsg(QString);
  void slotStatusMsg(QString);
  void slotTimerTic(void);
} ;

#endif // DBPROC_H
