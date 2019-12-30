
/*
 * SimpleGPIO.cpp
 *
 */

#include "hw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>



THw::THw() :QObject()
{
  pins_export();
  pins_set_dir();
}
THw::~THw()
{
  pins_unexport();
}

/****************************************************************
 * gpio_export
 ****************************************************************/
int THw::pins_export(void)
{
  qDebug()<<"Set pins export";
  gpio_export(PIN_HOST_RST);
  gpio_export(PIN_HOST_DAC_PD);
  gpio_export(PIN_HOST_DAC_CLR);
  gpio_export(PIN_WDO_H);
  return 0;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int THw::pins_unexport(void)
{
    qDebug()<<"Set pins unexport";
    gpio_unexport(PIN_HOST_RST);
    gpio_unexport(PIN_HOST_DAC_PD);
    gpio_unexport(PIN_HOST_DAC_CLR);
    gpio_unexport(PIN_WDO_H);
    return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int THw::pins_set_dir(void)
{
  qDebug()<<"Set pins direction";
  gpio_set_dir(PIN_HOST_RST,OUTPUT_PIN);
  gpio_set_dir(PIN_HOST_DAC_PD,OUTPUT_PIN);
  gpio_set_dir(PIN_HOST_DAC_CLR,OUTPUT_PIN);
  gpio_set_dir(PIN_WDO_H,INPUT_PIN);
  return 0;
}

void THw::rst_FPGA(void)
{
  gpio_set_value(PIN_HOST_RST,HIGH);
 // m_sleep(10);// sleep 10 ms
  gpio_set_value(PIN_HOST_RST,LOW);
 // m_sleep(10);// sleep 10 ms
}
int THw::getWDO(void)
{
  unsigned int val;
  if(!gpio_get_value(PIN_WDO_H,&val)) return val;
  return -1;
}
int THw::SPI_getHW(void)
{
    u_int32_t awd[1],ard[1],*ar,*aw;
    aw=awd; ar=ard;
    int fd=setSpi();
    awd[0]=0xf00000;
    transfer(fd,aw,ar);
    qDebug()<<"read fw"<<(ard[0]&0xffff);

    awd[0]=0xaa0000;
    transfer(fd,aw,ar);
    m_sleep(10);
    transfer(fd,aw,ar);
    qDebug()<<"read mode"<<(ard[0]&0xffff);

  //  awd[0]=0x300021; // write address reg
  //  transfer(fd,aw,ar);

  //  awd[0]=0x200001; // write dac reg
 //   transfer(fd,aw,ar);

    awd[0]=0xa00000; // read address reg
    transfer(fd,aw,ar);

    qDebug()<<"address"<<(ard[0]&0xffff);

    awd[0]=0x800000; // read dac reg
    transfer(fd,aw,ar);
    qDebug()<<"dac data"<<(ard[0]&0xffff);

    close(fd);
    return fd;
}
