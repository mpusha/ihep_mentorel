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


extern "C" int db_init(void);
extern "C" int db_process(unsigned int *update);
extern "C" short sh_put_meas_er(short er_code);
extern "C" short sh_put_data_er(short er_code);
extern "C" void sh_put_mess(char *mes);
extern "C" void set_state(unsigned int num, double *field, char *state);

class TDbBehav : public QThread
{
  Q_OBJECT

  public:
  TDbBehav(void) ; // constructor
  ~TDbBehav();
  void setAbort(bool a) { abort=a; }
  unsigned int dbInit(void);

protected:
    void run();

private:
// Orbita parameters
// current orbit for 60 axis with dynamical and statical amendment and KRZ
  char absR[allAxis],absZ[allAxis];

  floatA mTime_cur[timePoints];
  char tresholdI_cur[timePoints]; // treshold of intens in all timepoints of measure cycle
  QString date_cur,time_cur;
  int cycle_cur;
  floatA oR_cur[allAxis][timePoints],oZ_cur[allAxis][timePoints];
  floatA hS9R_cur[timePoints],hS10R_cur[timePoints],hS11R_cur[timePoints]; // sin R
  floatA hC9R_cur[timePoints],hC10R_cur[timePoints],hC11R_cur[timePoints]; // cos R
  floatA hS9Z_cur[timePoints],hS10Z_cur[timePoints],hS11Z_cur[timePoints]; // sin Z
  floatA hC9Z_cur[timePoints],hC10Z_cur[timePoints],hC11Z_cur[timePoints]; // cos Z
  floatA radiusR_cur[timePoints],radiusZ_cur[timePoints]; // average radius of R/Z
  floatA dispR_cur[timePoints],rmsR_cur[timePoints],dispZ_cur[timePoints],rmsZ_cur[timePoints]; //dispersion and SKVO for R/Z

// last orbit for 60 axis with dynamical and statical amendment and KRZ
  floatA mTime_last[timePoints];
  char tresholdI_last[timePoints]; // treshold of intens in all timepoints of measure cycle
  QString date_last,time_last;
  int cycle_last;
  floatA oR_last[allAxis][timePoints],oZ_last[allAxis][timePoints];
  floatA hS9R_last[timePoints],hS10R_last[timePoints],hS11R_last[timePoints]; // sin R
  floatA hC9R_last[timePoints],hC10R_last[timePoints],hC11R_last[timePoints]; // cos R
  floatA hS9Z_last[timePoints],hS10Z_last[timePoints],hS11Z_last[timePoints]; // sin Z
  floatA hC9Z_last[timePoints],hC10Z_last[timePoints],hC11Z_last[timePoints]; // cos Z
  floatA radiusR_last[timePoints],radiusZ_last[timePoints]; // average radius of R/Z
  floatA dispR_last[timePoints],rmsR_last[timePoints],dispZ_last[timePoints],rmsZ_last[timePoints]; //dispersion and SKVO for R/Z

// remember orbit for 60 axis with dynamical and statical amendment and KRZ
  floatA mTime_mem[timePoints];
  char tresholdI_mem[timePoints]; // treshold of intens in all timepoints of measure cycle
  QString date_mem,time_mem;
  int cycle_mem;
  floatA oR_mem[allAxis][timePoints],oZ_mem[allAxis][timePoints];
  floatA hS9R_mem[timePoints],hS10R_mem[timePoints],hS11R_mem[timePoints]; // sin R
  floatA hC9R_mem[timePoints],hC10R_mem[timePoints],hC11R_mem[timePoints]; // cos R
  floatA hS9Z_mem[timePoints],hS10Z_mem[timePoints],hS11Z_mem[timePoints]; // sin Z
  floatA hC9Z_mem[timePoints],hC10Z_mem[timePoints],hC11Z_mem[timePoints]; // cos Z
  floatA radiusR_mem[timePoints],radiusZ_mem[timePoints]; // average radius of R/Z
  floatA dispR_mem[timePoints],rmsR_mem[timePoints],dispZ_mem[timePoints],rmsZ_mem[timePoints]; //dispersion and SKVO for R/Z

//  calculate orbit by math request for 60 axis with dynamical and statical amendment and KRZ
  floatA mTime_math[timePoints];
  QString date_math,time_math;
  int cycle_math;
  floatA oR_math[allAxis][timePoints],oZ_math[allAxis][timePoints];
  floatA hS9R_math[timePoints],hS10R_math[timePoints],hS11R_math[timePoints]; // sin R
  floatA hC9R_math[timePoints],hC10R_math[timePoints],hC11R_math[timePoints]; // cos R
  floatA hS9Z_math[timePoints],hS10Z_math[timePoints],hS11Z_math[timePoints]; // sin Z
  floatA hC9Z_math[timePoints],hC10Z_math[timePoints],hC11Z_math[timePoints]; // cos Z
  floatA radiusR_math[timePoints],radiusZ_math[timePoints]; // average radius of R/Z
  floatA dispR_math[timePoints],rmsR_math[timePoints],dispZ_math[timePoints],rmsZ_math[timePoints]; //dispersion and SKVO for R/Z


  char tresholdI[timePoints]; // treshold of intens in all timepoints of measure cycle
// control var
  bool abort;
  bool haveErrMsg,haveStatusMsg;
  QString errMsg,statusMsg;

//system var
  QMutex mutex;
  QWaitCondition condition;


public slots:
  void slotErrMsg(QString);
  void slotStatusMsg(QString);

} ;

#endif // DBPROC_H
