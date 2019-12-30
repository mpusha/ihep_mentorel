#ifndef DBPROC_H
#define DBPROC_H

#include <QtCore/QCoreApplication>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include <QWaitCondition>

#include "orbBehav.h"
#include "db.h"

const int sampleInterval=300;
extern "C" int db_init(void);
extern "C" int db_process(unsigned int *update,int *start,int *stop);
extern "C" int writeErrState_DB(int param, char data[], int size);
extern "C" int writeCntUD_DB(int *cnt);
extern "C" int writeAbsAxes_DB(char axisR[], char axisZ[], int size);
extern "C" int writeOrbData_DB(int ind,double data[][timePoints]);
extern "C" int writeOrbParamData_DB(int ind, int param, double data[], int size);
extern "C" int writeOrbParamTH_DB(int ind,int param,char data[], int size);
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
  unsigned int dbInit(void);
  char getDBStatus(void) { return(statusDB); }
  void processingData(int allValid, double data[allAxis][timePoints], double radius[timePoints],
                      double Sin9[], double Cos9[], double Sin10[], double Cos10[], double Sin11[],
                      double Cos11[], double Disp[], double Rms[]);
  int saveCurrMeas(void);
  int savePrevMeas(void);
  int clearOrbAndSave(void);
  int rememberOrbAndSave(void);
  int mathOrbAndSave(int meas);
  int prepDataWWWSts(QString fileName);
  void prepDataWWWOrb(double orb[allAxis][timePoints],QString fileName);
  void prepDataWWWHarm(double t[],double s9[],double c9[],double s10[],double c10[],double s11[],double c11[],
                               double avr[],double rms[],QString fileName);
  void prepDataWWWEndRequest(void);
  protected:
    void run();

private:
// Orbita parameters
// current orbit for 60 axis with dynamical and statical amendment and KRZ
  char absR[allAxis],absZ[allAxis];
  int allValidR,allValidZ;
  QDateTime dt;
  u_int8_t regime_cur;
  double mTime_cur[timePoints];
  char tresholdI_cur[timePoints]; // treshold of intens in all timepoints of measure cycle
  QString msg_cur,dateTime_cur;
  int cycle_cur;
  double oR_cur[allAxis][timePoints],oZ_cur[allAxis][timePoints];
  double hS9R_cur[timePoints],hS10R_cur[timePoints],hS11R_cur[timePoints]; // sin R
  double hC9R_cur[timePoints],hC10R_cur[timePoints],hC11R_cur[timePoints]; // cos R
  double hS9Z_cur[timePoints],hS10Z_cur[timePoints],hS11Z_cur[timePoints]; // sin Z
  double hC9Z_cur[timePoints],hC10Z_cur[timePoints],hC11Z_cur[timePoints]; // cos Z
  double radiusR_cur[timePoints],radiusZ_cur[timePoints]; // average radius of R/Z
  double dispR_cur[timePoints],rmsR_cur[timePoints],dispZ_cur[timePoints],rmsZ_cur[timePoints]; //dispersion and SKVO for R/Z

// last orbit for 60 axis with dynamical and statical amendment and KRZ
  double mTime_last[timePoints];
  char tresholdI_last[timePoints]; // treshold of intens in all timepoints of measure cycle
  QString msg_last,dateTime_last;
  int cycle_last;
  double oR_last[allAxis][timePoints],oZ_last[allAxis][timePoints];
  double hS9R_last[timePoints],hS10R_last[timePoints],hS11R_last[timePoints]; // sin R
  double hC9R_last[timePoints],hC10R_last[timePoints],hC11R_last[timePoints]; // cos R
  double hS9Z_last[timePoints],hS10Z_last[timePoints],hS11Z_last[timePoints]; // sin Z
  double hC9Z_last[timePoints],hC10Z_last[timePoints],hC11Z_last[timePoints]; // cos Z
  double radiusR_last[timePoints],radiusZ_last[timePoints]; // average radius of R/Z
  double dispR_last[timePoints],rmsR_last[timePoints],dispZ_last[timePoints],rmsZ_last[timePoints]; //dispersion and SKVO for R/Z

// remember orbit for 60 axis with dynamical and statical amendment and KRZ
  double mTime_mem[timePoints];
  char tresholdI_mem[timePoints]; // treshold of intens in all timepoints of measure cycle
  QString msg_mem,dateTime_mem;
  int cycle_mem;
  double oR_mem[allAxis][timePoints],oZ_mem[allAxis][timePoints];
  double hS9R_mem[timePoints],hS10R_mem[timePoints],hS11R_mem[timePoints]; // sin R
  double hC9R_mem[timePoints],hC10R_mem[timePoints],hC11R_mem[timePoints]; // cos R
  double hS9Z_mem[timePoints],hS10Z_mem[timePoints],hS11Z_mem[timePoints]; // sin Z
  double hC9Z_mem[timePoints],hC10Z_mem[timePoints],hC11Z_mem[timePoints]; // cos Z
  double radiusR_mem[timePoints],radiusZ_mem[timePoints]; // average radius of R/Z
  double dispR_mem[timePoints],rmsR_mem[timePoints],dispZ_mem[timePoints],rmsZ_mem[timePoints]; //dispersion and SKVO for R/Z

//  calculate orbit by math request for 60 axis with dynamical and statical amendment and KRZ
  double mTime_math[timePoints];
  char tresholdI_math[timePoints];
  QString msg_math,dateTime_math;
  int cycle_math;
  double oR_math[allAxis][timePoints],oZ_math[allAxis][timePoints];
  double hS9R_math[timePoints],hS10R_math[timePoints],hS11R_math[timePoints]; // sin R
  double hC9R_math[timePoints],hC10R_math[timePoints],hC11R_math[timePoints]; // cos R
  double hS9Z_math[timePoints],hS10Z_math[timePoints],hS11Z_math[timePoints]; // sin Z
  double hC9Z_math[timePoints],hC10Z_math[timePoints],hC11Z_math[timePoints]; // cos Z
  double radiusR_math[timePoints],radiusZ_math[timePoints]; // average radius of R/Z
  double dispR_math[timePoints],rmsR_math[timePoints],dispZ_math[timePoints],rmsZ_math[timePoints]; //dispersion and SKVO for R/Z


  char tresholdI[timePoints]; // treshold of intens in all timepoints of measure cycle
// control var
  bool abort,timerEv,canTic;
  bool haveErrStatusMsg;
  QString errStatusMsg;

//system var
  QMutex mutex;
  QWaitCondition condition;
  QTimer *timer;
// DB var
  char statusDB; // if 0 - DB is OK


public slots:
  void slotErrStatusMsg(QString);
  void slotTimerTic(void);
} ;

#endif // DBPROC_H
