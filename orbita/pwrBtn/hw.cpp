
/*
 * hw.cpp
 *
 */
#include <QDir>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "hw.h"

THw::THw()
{
  gpioInit=false;
  abort=false;
}

/****************************************************************
 * destructor
 * Unexport pins and free spi1
 ****************************************************************/
THw::~THw()
{
  qDebug()<<"Start THw destructor.";
  usr_pwrLedOff();
  usr_closeBBHW();
  qDebug()<<"Stop THw destructor.";
}


/****************************************************************
 * initialisation of bbb hardware (SPI, GPIO)
 ****************************************************************/
int THw::usr_initalBBHW(void)
{
  qDebug()<<"Initialisation Base Board";
  if(pins_export()) return 1;
  if(pins_set_dir()) return 1;
  // setup rising edge of pin host_stop
  // polling pin WDO. if pulse rise front, then reload FPGA
  if(gpio_set_edge(PIN_BTN_PWR,(char*)"falling")) return 1;
  fd_d0=gpio_fd_open(PIN_BTN_PWR);
  epfd=epoll_create(1);
  ev_d0.events=EPOLLET|EPOLLIN;
  ev_d0.data.fd=fd_d0;
  epoll_ctl(epfd,EPOLL_CTL_ADD,fd_d0,&ev_d0);

  usr_pwrLedOn();
  gpioInit=true;
  return 0;
}

int THw::usr_closeBBHW(void)
{
  if(gpio_fd_close(fd_d0)) return 1;
  if(pins_unexport())return 1;
  return(0);
}


// system activity LED manipulation
void THw::usr_pwrLedOn()
{
  gpio_set_value(PIN_LED_PWR,HIGH);
}
void THw::usr_pwrLedOff()
{
  gpio_set_value(PIN_LED_PWR,LOW);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GPIO functions
/****************************************************************
 * gpio_export
 ****************************************************************/
int THw::pins_export(void)
{
  qDebug()<<"Set pins export";
  if(gpio_export(PIN_BTN_PWR)) return 1;
  if(gpio_export(PIN_LED_PWR)) return 1;
  return 0;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int THw::pins_unexport(void)
{
    qDebug()<<"Set pins unexport";
    gpio_unexport(PIN_BTN_PWR);
    gpio_unexport(PIN_LED_PWR);
    return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int THw::pins_set_dir(void)
{
  qDebug()<<"Set pins direction";
  if(gpio_set_dir(PIN_LED_PWR,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_BTN_PWR,INPUT_PIN)) return 1;

  return 0;
}

// polling GPIOs on find  front of WDO. On this pulse reload FPGA
int THw::usr_pollingGPIOs(int timePolling)
{
   int value = 0 ,n,i;

   n = epoll_wait(epfd, events, 10, timePolling);

  for ( i = 0;  i < n; ++i) {
    if (events[i].data.fd == ev_d0.data.fd) value|=1;
  }
  return value;
}
int THw::usr_getBtnPwr(void)
{
  unsigned int val;
  if(!gpio_get_value(PIN_BTN_PWR,&val)) return val;
  return -1;
}
void THw::run(void)
{
  QEventLoop *loop;
  loop=new QEventLoop();
  usr_initalBBHW();
  msleep(100);
  loop->processEvents();
  usr_pollingGPIOs(100);
  int ev=0;
  while(!abort){
    loop->processEvents();
    ev= usr_pollingGPIOs(1000);
    if(ev) {
      qDebug()<<"Event";
      msleep(1500);
      if(!usr_getBtnPwr()){
        qDebug()<<"Find pressed Button Power";
        system("shutdown -hP 0");
        //usr_pwrLedOff();
      }
    }
  }
}


