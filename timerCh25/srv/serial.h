#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/poll.h>
#include <syslog.h>

#include <QDebug>

#define UART_TIMEOUT 1000L  //delay in ms
#define UART_SHORT_TOUT 20L

class TSerial
{
public:
  explicit TSerial();
  ~TSerial();
  int UART_Init(QString devName, int speedBoud);
  int UART_Open(char *port);
  int UART_Set(int speed, int flow_ctrl, int databits, int stopbits, int parity);
  int UART_Recv(char *rcv_buf, int data_len);
  int UART_Read(char *rcv_buf, int data_len, int tout);
  int UART_Send(char *send_buf, int data_len);
  void UART_Flush(void);
  void UART_Close(void);
  int getUARTError(void) { return(errUART);}
private:
  int fd;
  int errUART;
  int speed;
  QString device;

};

#endif // SERIAL_H
