
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



THw::THw(int c):QObject(),size(c)
{
  fd_spi=0;
}
THw::~THw()
{
  pins_unexport();
  if(fd_spi>0) close(fd_spi);
}

/****************************************************************
 * initialisation of bbb hardware (SPI, GPIO)
 ****************************************************************/
int THw::initalBBHW(void)
{
  pins_export();
  pins_set_dir();
  fd_spi=setSpi();
  return 0;
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
 // m_sleep(12000);// sleep 10 ms
  gpio_set_value(PIN_HOST_DAC_PD,LOW);
  gpio_set_value(PIN_HOST_DAC_CLR,LOW);
  m_sleep(10);
  gpio_set_value(PIN_HOST_DAC_CLR,HIGH);
  //gpio_set_value(PIN_HOST_DAC_CLR,HIGH);
  m_sleep(100);
  gpio_set_value(PIN_HOST_DAC_PD,HIGH);
}
int THw::getWDO(void)
{
  unsigned int val;
  if(!gpio_get_value(PIN_WDO_H,&val)) return val;
  return -1;
}
int THw::SPI_getHW(void)
{
  return SPI_readReg(R_FW_REG);
}
void THw::SPI_writeReg(u_int32_t data, u_int32_t reg)
{
  u_int32_t awd,ard,*ar,*aw;
  ar=&ard; aw=&awd;
  awd=(reg&0xf00000)|(data&0xffff);
  transfer(fd_spi,aw,ar,4);
}

u_int32_t  THw::SPI_readReg(u_int32_t reg)
{
  u_int32_t awd,ard,*ar,*aw;
  ar=&ard; aw=&awd;
  awd=reg&0xf00000;
  transfer(fd_spi,aw,ar,4);
  return (ard&0xffff);
}

void THw::SPI_writeReadData(u_int32_t *dataw, u_int32_t *datar,u_int32_t len,u_int32_t reg)
{
  //const u_int32_t fifo_size=512;
  u_int32_t *ar,*aw,sz,sz1;
  ar=datar; aw=dataw;
  for(u_int32_t i=0;i<len;i++){
   *dataw=(reg&0xf00000)|(*dataw&0xffff);
   *datar=0;
   dataw++;datar++;
  }
  sz=len/fifo_size;
  sz1=len%fifo_size;
  while(sz>0){
    transfer(fd_spi,aw,ar,4*fifo_size);
    aw+=fifo_size; ar+=fifo_size;
    sz--;
  }
  if(sz1>0){
    transfer(fd_spi,aw,ar,4*sz1);
  }
}

void THw::spiRegsTest(void)
{
  u_int32_t td;

  td=55;
  SPI_writeReg(td, W_ADDR_REG);
  qDebug()<<"Write address reg "<<td<<" Read address reg "<<SPI_readReg(R_ADDR_REG);

  td=3;
  SPI_writeReg(td, W_MODE_REG);
  qDebug()<<"Write mode reg "<<td<<" Read mode reg "<<SPI_readReg(R_MODE_REG);

  td=8000;
  SPI_writeReg(td, W_STROBE_REG);
  qDebug()<<"Write strobe reg "<<td<<" Read strobe reg "<<SPI_readReg(R_STROBE_REG);

  td=5000;// 250us
  SPI_writeReg(td, W_SAMPLE_REG);
  qDebug()<<"Write sample reg "<<td<<" Read sample reg "<<SPI_readReg(R_SAMPLE_REG);

  qDebug()<<"Read status reg "<<SPI_readReg(R_STATUS_REG);
}

void THw::spiDacMemTest(void)
{

    u_int32_t addr,dw,dr;

    u_int32_t wd[size], rd[size];
    qDebug()<<"Mem test 1 rnd write read is begin";
    addr=0;
  /*  for(addr=0;addr<size;addr++){
      dw=rand()%16384;
      SPI_writeReg(addr, W_ADDR_REG);
      SPI_writeReg(dw, W_DAC_DATA);
      if((dr=SPI_readReg(R_DAC_DATA))!=dw) {
        qDebug()<<"Error mem test 1! Mem R/W address"<<addr<<"Write data"<<dw<<"Read data"<<dr;
        return;
      }
    }
    qDebug()<<"Mem test 1 Ok. Write/Read rnd data. Ok";

    qDebug()<<"Mem 2 write/delay/read is begin";

    for(addr=0;addr<size;addr++){
        wd[addr]=rand()%16384;
        SPI_writeReg(addr, W_ADDR_REG);
        SPI_writeReg(wd[addr], W_DAC_DATA);
    }
    m_sleep(1000);
    for(addr=0;addr<size;addr++){
      SPI_writeReg(addr, W_ADDR_REG);
      if((dr=SPI_readReg(R_DAC_DATA))!=wd[addr]) {
        qDebug()<<"Error mem test 2! Mem R/W address"<<addr<<"Write data"<<wd[addr]<<"Read data"<<dr;
        return;
      }
    }
    qDebug()<<"Mem test 2 Ok. Write/Read rnd data with delay. Ok.";
*/
    qDebug()<<"Mem test 3 with address increment is begin.";
    for(addr=0;addr<size;addr++){
        wd[addr]=rand()%16384;
    }
    SPI_writeReg(0, W_ADDR_REG);
    SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);

    for(addr=0;addr<size;addr++){
        SPI_writeReg(addr, W_ADDR_REG);
        if((dr=SPI_readReg(R_DAC_DATA))!=(wd[addr]&0xffff)) {
          qDebug()<<"Error mem test 3! Mem R/W address"<<addr<<"Write data"<<wd[addr]<<"Read data"<<dr;
          return;
        }
    }
    qDebug()<<"Mem test 3 Ok. Write/Read rnd data with address increment. Ok.";
}
void THw::spiDacMemTest(int rep)
{
  //const u_int32_t size=8192;
  u_int32_t addr;
  u_int32_t wd[size], rd[size];
  for(addr=0;addr<size;addr++){
    wd[addr]=addr;
  }
  if(!rep)  {
    qDebug()<<"Loop wrie DAC operation. Ctrl-C for exit";
    while(1){
      SPI_writeReg(0, W_ADDR_REG);
      SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
      m_sleep(50);
    }
  }
  qDebug()<<"Write DAC operation for "<<rep<<"ones";
  while (rep-->0){
    SPI_writeReg(0, W_ADDR_REG);
    SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
    qDebug()<<rep+1;
  }
}
void THw::spiWrMemConst(u_int32_t d)
{
  u_int32_t wd[size], rd[size];
  u_int32_t addr,dw;
  for(addr=0;addr<size;addr+=2){
      wd[addr]=addr;
      wd[addr+1]=4095-addr;
  }
 /*for(addr=0;addr<20;addr+=2){
    dw=addr*100;
    SPI_writeReg(addr, W_ADDR_REG);
    SPI_writeReg(dw, W_DAC_DATA);
    SPI_writeReg(addr+1, W_ADDR_REG);
    SPI_writeReg(dw, W_DAC_DATA);
  }
*/
  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);

}

void THw::spiRdDacMem(int cnt)
{
  u_int32_t wd[size], rd[size];
  u_int32_t addr;
  for(addr=0;addr<size;addr++){
      wd[addr]=0;
  }
  for(int i;i<cnt;i++){
    SPI_writeReg(0, W_ADDR_REG);
    qDebug()<<"dac"<<SPI_readReg(R_DAC_DATA);
  }
}
void THw::spiRdAdcMem(int cnt)
{
  u_int32_t wd[size], rd[size];
  u_int32_t addr;
  for(addr=0;addr<size;addr++){
      wd[addr]=0;
  }
  for(int i;i<cnt*2;i+=2){
    SPI_writeReg(i, W_ADDR_REG);
    qDebug()<<"adc1"<<(SPI_readReg(R_ADC_DATA)&0xfff);
    SPI_writeReg(i+1, W_ADDR_REG);
    qDebug()<<"adc2"<<(SPI_readReg(R_ADC_DATA)&0xfff);
  }
}
void THw::spiWrDAC(int dig)
{
   gpio_set_value(PIN_HOST_RST,HIGH); // clk 1
   gpio_set_value(PIN_HOST_DAC_CLR,HIGH); // sync 1
   gpio_set_value(PIN_HOST_DAC_PD,LOW); // sdata 0

   gpio_set_value(PIN_HOST_DAC_CLR,LOW); // sync 0

   for(int i=0;i<16;i++){
     if(dig&0x8000) gpio_set_value(PIN_HOST_DAC_PD,HIGH); // sdata 0
     else
       gpio_set_value(PIN_HOST_DAC_PD,LOW); // sdata 0

     gpio_set_value(PIN_HOST_RST,LOW); // clk 0
     // m_sleep(1);
     dig<<=1;
     gpio_set_value(PIN_HOST_RST,HIGH); // clk 1
      //m_sleep(1);
   }

   gpio_set_value(PIN_HOST_DAC_CLR,HIGH); // sync 0
   m_sleep(100);

}
void THw::spiRdStatus(void)
{m_sleep(6000);
    //while((SPI_readReg(R_STATUS_REG)&0x20)){ qDebug() <<"1"<<SPI_readReg(R_STATUS_REG); m_sleep(1);}
  while((SPI_readReg(R_STATUS_REG)&0x40)==0) { qDebug() <<"2"<< SPI_readReg(R_STATUS_REG); m_sleep(1);}
  //qDebug()<<"Read status reg "<<SPI_readReg(R_STATUS_REG);
}
