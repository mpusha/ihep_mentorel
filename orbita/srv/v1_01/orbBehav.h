#ifndef ORBBEHAV_H
#define ORBBEHAV_H

#include <QtCore>
#include <QVector>

#include <syslog.h>

#include "progerror.h"
#include "hw.h"

#define SAMPLE_DEVICE 100 // time of request device

typedef enum
{
  INITIAL_STATE,
  DEVICE_ERROR_STATE,
  GET_DATA_STATE,
  GET_STATUS_STATE,
  SET_DATA_STATE,
  UPDATE_STATUS_STATE,
  ALLREQSTATES,
  IDLE
} CPhase;  // phases of state machine


class THw;
class TOrbBehav : public THw
{
  Q_OBJECT
public:
  TOrbBehav();
  ~TOrbBehav();
  void setAbort(bool a) { abort=a; }
  void createAdcData(int); // get mease from ADCs and sort data according axis and time points
  void createOrbData(void); // add statical and dynamical amendment and multiply data on KRZ
  void wrADCFile(QString);
  int readCA(QString);
  void initialDevice(void);
  void getAbsentOs(char r[],char z[]){ for(int i=0;i<allAxis;i++) {r[i]=absAxisR[i]; z[i]=absAxisZ[i];} }

protected:
  void run();
signals:
  void signalErrMsg(QString);
  void signalStatusMsg(QString);
private:
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  int stErr; // errors of work with hw
  TDevErrors devError;  // hardware error in bits represantation
  int cntErrInitial;
  bool abort,timerAlarm;
  bool absAxisR[allAxis],absAxisZ[allAxis];
  floatA asR[allAxis],asZ[allAxis]; //statical amendment
  floatA adR[allAxis],adZ[allAxis]; // dynamical amendment
  floatA cR[allAxis],cZ[allAxis];   // coefficients of PU electrode
  floatA dfAdc1[timePoints*2*2][adcCh],dfAdc2[timePoints*2*2][adcCh]; // data from ADCs 2 direction and 2 commutation
  floatA OrbR[allAxis][timePoints],OrbZ[allAxis][timePoints],Intens[2][timePoints]; // pure data of Orbita without dynamical and statical amendment and KRZ
//  float oR[allAxis][timePoints],oZ[allAxis][timePoints]; // orbita for 60 axis with dynamical and statical amendment and KRZ
  u_int32_t timeMeas[timePoints];
  u_int8_t modeRegime;
  QString workDir;
};

#endif // ORBBEHAV_H
