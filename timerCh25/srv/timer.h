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
// 0 - отсутствие ошибки
// 1 - переполнение входного буфера (возможно посылаемвая строка не заканяивается кодом 0)
// 2 - формат входных данных не соответствует ожидаемому scanf
// 3 - ошибка во входных данных. Возможен выход из диапазона или отрицательный знак
// 4 - ошибка при записи в HW (считались данные не соответствующие записываемым)
// 5 - ошибка FPGA (возможно не сконфигурирована)
#define ERR_UART_ABS 8     // отсутствует порт UART. Ошибка инициализации устройства
#define ERR_UART_TRANS 9   // ошибка отправки сообщения
#define ERR_UART_TOUT  10  // при приеме произоше таймаут UART_TIMEOUT определенный в serial.h
#define ERR_IDATA_CNT 11   // принято колличество данных менее ожидаемого
#define ERR_IDATA_ADDR 12  // приятый адрес не является числом
#define ERR_IDATA_CRET 13  // принятые данные не являются числом
#define ERR_IDATA_DATA 14  // принятые данные не совпадают с записываемыми (при операции проверки)
#define ERR_SETUP_ADDR 15  // адрес устройства не установлен, или выходит из диапазона 0-99
#define ERR_SETUP_CH 16    // устанавливаемый канал таймера выходит из диапазона 1-16, либо не установлен
#define ERR_SETUP_DATA 17  // устанавливаоемое время запуска или длительность выходного имульса выходит из диапазона MINDATA-MAXDATA MINWIDTH-MAXWIDTH
#define ERR_FILE_WRITE 18  // ошибка открытия файла на запись при чтении данных из таймера
#define ERR_FILE_READ 19   // ошибка открытия файла на чтение при записи данных в таймер
#define ERR_FILE_SETUP_ABS 20 // отсутствует файл setup.xms in settings
#define ERR_FILE_SETTINGS 21  // ошибка в файле с установками либо он отсутствет

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
  TTimer(QString devName, QString spd);
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
  int testAlive(void);
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
  int speed;


  int writeReadData(QString cmd,int ch, int data, bool validate,QString cmdValidate); // write/read data
  int readData(QString cmd,int ch,int *data); //read data
  int sendCmd(QString cmd);
  int readAnswer(QString& answer);

};

#endif // TIMER_H
