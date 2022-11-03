#ifndef TIMER_H
#define TIMER_H

#include <QtCore>
//#include <QStringList>
//#include <QDateTime>
//#include <QFile>
//#include <QThread>
//#include <QEventLoop>
//#include <QTimer>
#include "serial.h"

#define REP 3 // number of repeates

#define ERR_NONE 0
// from 0 to 7 reserv for uc ATmega162
#define ERR_UART_ABS 8
#define ERR_UART_TRANS 9
#define ERR_UART_TOUT  10
#define ERR_IDATA_CNT 11
#define ERR_IDATA_ADDR 12
#define ERR_IDATA_CRET 13
#define ERR_IDATA_DATA 14
#define ERR_SETUP_ADDR 15
#define ERR_SETUP_CH 16
#define ERR_SETUP_DATA 17
#define ERR_FILE_WRITE 18
#define ERR_FILE_READ 19

#define MINDATA 10L
#define MAXDATA 12000000L
#define MINWIDTH 10L
#define MAXWIDTH 5000000L
#define RS_DELAY 2000L // delay in us for RS485 trans rec operation
void msleep(uint64_t ms);

class TTimer : public TSerial
{
public:
  //explicit
  TTimer(QString devName);
  ~TTimer();
/*  void m_sleep(uint64_t ms){
    QEventLoop loop;
    QTimer::singleShot(ms,&loop,SLOT(quit()));
    loop.exec();
  }
*/
  QString sendReadRawCmd(QString cmd);

  char *getStrErr(int timerError) { return(strErrors[timerError].toAscii().data()); }
  QString getQStrErr(int timerError){ return(strErrors[timerError]); }
  int initHW(void);
  int setAddress(int addr);
  int writeFile(char* filename);
  int readFile(char *filename);
// write/read data functions
  int writeRegTime(int ch, int data);
  int writeRegWidth(int ch, int data);
  int writeRegMap(int data);
  int writeRegTimeValidate(int ch, int data);
  int writeRegWidthValidate(int ch, int data);
  int writeRegMapValidate(int data);
  int writeHWTime(int ch, int data);
  int writeHWWidth(int ch, int data);
  int writeHWMap(int data);
  int writeHWTimeValidate(int ch, int data);
  int writeHWWidthValidate(int ch, int data);
  int writeHWMapValidate(int data);
// read data functions
  int readRegTime(int ch,int* data);
  int readRegWidth(int ch,int* data);
  int readRegMap(int* data);
  int readHWTime(int ch,int* data);
  int readHWWidth(int ch,int* data);
  int readHWMap(int* data);
// write from register into HW
  int wrHW(void);
  int wideBandWrHW(void);

private:
  QStringList strErrors;
  bool canOperate;
  int address; // address of timer 0 - all device 1-99 work device
  QString device;


  int writeReadData(QString cmd,int ch, int data, bool validate,QString cmdValidate); // write/read data
  int readData(QString cmd,int ch,int *data); //read data
  int sendCmd(QString cmd);
  int readAnswer(QString& answer);

};

#endif // TIMER_H
