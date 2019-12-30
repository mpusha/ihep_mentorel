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
  DATA_READY_STATE,
  UPDATE_STATUS_STATE,
  DATA_WRITE_STATE,
  GET_DATA_STATE,
 // GET_STATUS_STATE,
  SET_DATA_STATE,
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
  void createOrbData(int calibrovka); // with calibration of input probes
  void wrADCFile(QString);
  int readCA(QString);
  void initialDevice(void);
  void writeOrbData(void);
  void getAbsentOs(char r[],char z[],int *numR,int *numZ)
  {
    *numR=0; *numZ=0;
    for(int i=0;i<allAxis;i++) {
      if(absAxisR[i]) r[i]=0x81; else { r[i]=0; (*numR)++; }
      if(absAxisZ[i]) z[i]=0x81; else { z[i]=0; (*numZ)++; }
      //r[i]=(absAxisR[i]==true) ? 0x81 : 0; z[i]=(absAxisZ[i]==true) ? 0x81 : 0;
    }
  }
  void setCalibration(int data) {calibration=data;}
  int getReadyMeasure(void) { return(dataReady); }
  int getReadyHW(void) { return(hwReady); }
  void readMeasure(double realT[timePoints], char to[timePoints], u_int8_t *realMode,double orbitaR[allAxis][timePoints], double orbitaZ[allAxis][timePoints]); //read data to DB
public slots:
  int writeToTmpDataRequest(double tmpTime[timePoints], u_int8_t tmpMR, double Int, int sta, int sto);
  int getDataRequest(void);
protected:
  void run();
signals:
  void signalErrStatusMsg(QString);
private:
  CPhase phase;
  CPhase allStates[ALLREQSTATES];
  int stErr; // errors of work with hw
  TDevErrors devError;  // hardware error in bits represantation
  int cntErrInitial;
  bool abort;//,timerAlarm;
  bool absAxisR[allAxis],absAxisZ[allAxis];
  char threshOldInt[timePoints];
  floatA asR[allAxis],asZ[allAxis]; //statical amendment
  floatA adR[allAxis],adZ[allAxis]; // dynamical amendment
  floatA cR[allAxis],cZ[allAxis];   // coefficients of PU electrode
  floatA dfAdc1[timePoints*2*2][adcCh],dfAdc2[timePoints*2*2][adcCh]; // data from ADCs 2 direction and 2 commutation
  floatA OrbR[allAxis][timePoints],OrbZ[allAxis][timePoints],Intens[2][timePoints]; // pure data of Orbita without dynamical and statical amendment and KRZ
  floatA oR[allAxis][timePoints],oZ[allAxis][timePoints]; // orbita for 60 axis with dynamical and statical amendment and KRZ
  u_int32_t timeMeas[timePoints],realTimeMeas[timePoints];
  u_int8_t modeRegime,realModeRegime;
  QString workDir;

  int calibration;
  int wrFlag,dataReady,hwReady;
  u_int32_t tmpTimeMeas[timePoints];
  u_int8_t tmpModeRegime;
  double tmpThresholdInt;
  int tmpStart,tmpStop;

};

#endif // ORBBEHAV_H
