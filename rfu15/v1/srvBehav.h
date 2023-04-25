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


#include "SimpleGPIO.h"
#include "progerror.h"
#include "db.h"
#include "hw.h"

#define SAMPLE_DEVICE 1000 // time of request device

#define TIMECONV 1e3

/// GF status
#define PS1_PWR 0x01 ///< 0 0x01-PS1 status On/off from source 1 on, 0 off
#define PS1_ERR 0x02 ///< 1 0x02-PS1 status error from source 1 error
#define PS2_PWR 0x04 ///< 2 0x04-PS2 status On/off from source 1 on, 0 off
#define PS2_ERR 0x08 ///< 3 0x08-PS2 status error from source 1 error
#define PS_SYNC 0x10 ///< 4 0x10-synchronisation is absent 1 error not go start pulse
#define PS_WORK 0x20 ///< 5 0x20-window strobe 1 strobe (work PS)
#define PS_CRW  0x40 ///< 6 0x40 -window can read/write 1 can read/write
#define PS_RES  0x80 ///< 7 0x80-reserve

/// Набор возможных состояний объекта
typedef enum
{
  INITIAL_STATE, ///< начальное состояние
  DEVICE_ERROR_STATE,
  UPDATE_HW_STATE,
  UPDATE_STATUS_STATE,
  ALLREQSTATES,
  IDLE,
  READY ///< готов
}CPhase;  // phases of state machine

/*!
 * \brief The TSrvBehav class
 *
 * Класс описывающий поведение генератора функций.
 */

class TSrvBehav : public QThread
{
  Q_OBJECT
public:
  TSrvBehav();
  ~TSrvBehav();

// function for work with data
  int prepareCSVData(QString filename);
  void readSettings(QString filename);

// function for work with device
  int getReadyHW(void) { return(hwReady); }
  void setErrorSt(short int); // set errors in state of statemachine for all stadies
  void initialDevice(void);
  int getStatus();
  void updateHW(void);
  int  wrHWsimple(void);

// make request fow write HW from external sourcers
  void writeHwReq(void);
  void endHwReq(void);

public slots:
  void timeAlarm(void);

protected:
    void run();

signals:
  void signalErrMsg(QString);
  void signalStatusMsg(QString);

private:
// HW
  THw *dev;
  TDevErrors devError;  // hardware error in bits represantation
  short int stErr; // errors of work with hw
  int cntErr;
  int hw; ///< version
  int statusHW; ///< status get every SAMPLE_DEVICE interval
  int hwReady; ///< status ready setup after compleate initialisation of HW
/// individual device parameters
  double stepTime; ///< В мс. По умолчанию 0.1 мс. Время между точками ГФ. (возможна установка 0.05 мс)
  double coefTransf; ///< коэффициент преобразования  кВ в код ЦАПа. По умолчанию -200.0 (10 кВ - 2.44 В на выходе ЦАП)
  int dac1Sh,dac2Sh; ///< сдвиг выхода ЦАПа для компенсации 0
  double dTime[ALLVECTORS],dAmp[ALLVECTORS];

/// Name of file with CSV data
  QString fileNameCSV;

// process state machine
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  bool abort,canTic;

  QTimer tAlrm;

  QMutex mutex;
  QWaitCondition condition;

private slots:


};

#endif // SRVBEHAV_H

