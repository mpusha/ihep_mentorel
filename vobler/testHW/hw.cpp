
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
 * gpio_export
 ****************************************************************/
int THw::pins_export(void)
{
  qDebug()<<"Set pins export";
  if(gpio_export(PIN_HOST_RST)) return 1;
  if(gpio_export(PIN_HOST_DAC_PD)) return 1;
  if(gpio_export(PIN_HOST_DAC_CLR))return 1;
  if(gpio_export(PIN_WDO_H)) return 1;
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
  if(gpio_set_dir(PIN_HOST_RST,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_HOST_DAC_PD,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_HOST_DAC_CLR,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_WDO_H,INPUT_PIN)) return 1;
  return 0;
}

/****************************************************************
 * initialisation of bbb hardware (SPI, GPIO)
 ****************************************************************/
int THw::usr_initalBBHW(void)
{
  if(pins_export()) return 1;
  if(pins_set_dir()) return 1;
  fd_spi=setSpi();
  if(fd_spi<=0) return 1;
  usr_clrOff();
  usr_pdOff();
  return 0;
}

void THw::usr_rst(void)
{
  gpio_set_value(PIN_HOST_RST,HIGH);
  m_sleep(10);// sleep 10 ms
  gpio_set_value(PIN_HOST_RST,LOW);
}
void THw::usr_pdOn()
{
  gpio_set_value(PIN_HOST_DAC_PD,LOW);
}
void THw::usr_pdOff()
{
  gpio_set_value(PIN_HOST_DAC_PD,HIGH);
}
void THw::usr_clrOn()
{
  gpio_set_value(PIN_HOST_DAC_CLR,LOW);
}
void THw::usr_clrOff()
{
  gpio_set_value(PIN_HOST_DAC_CLR,HIGH);
}

int THw::usr_getWDO(void)
{
  unsigned int val;
  if(!gpio_get_value(PIN_WDO_H,&val)) return val;
  return -1;
}
int THw::usr_getHW(void)
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
// write sample time in us (min 32 max 800)
void THw::usr_WrSample(u_int32_t sample)
{
  SPI_writeReg(sample*20, W_SAMPLE_REG);
}
// write strobe register (width of pulse) min 1 max 32767
void THw::usr_WrStrobe(u_int32_t strobe)
{
  SPI_writeReg(strobe, W_STROBE_REG);
}

u_int32_t THw::usr_RdSample(void)
{
  return SPI_readReg(R_SAMPLE_REG)/20;
}

u_int32_t THw::usr_RdStrobe(void)
{
  return SPI_readReg(R_STROBE_REG);
}

void THw::usr_RdRegs(int reg)
{
  switch(reg){
  case 0: qDebug()<<"Read mode reg "<<SPI_readReg(R_MODE_REG); break;
  case 1: qDebug()<<"Read strobe reg "<<SPI_readReg(R_STROBE_REG); break;
  case 2: qDebug()<<"Read strobe reg "<<SPI_readReg(R_SAMPLE_REG); break;
  }

}

void THw::usr_RegsTest(void)
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
  qDebug()<<"Write strobe reg "<<td<<" Read strobe reg "<<usr_RdStrobe();

  td=5000;// 250us
  SPI_writeReg(td, W_SAMPLE_REG);
  qDebug()<<"Write sample reg "<<td<<" Read sample reg "<<usr_RdSample();

  qDebug()<<"Read status reg "<<SPI_readReg(R_STATUS_REG);
}
// complex test of dac memory
void THw::usr_DacMemTest(void)
{

    u_int32_t addr,dw,dr;
    u_int32_t wd[size], rd[size];
    qDebug()<<"Mem test 1 rnd write read is begin";
    usr_pdOn(); // operation only from mem
    addr=0;
    for(addr=0;addr<size;addr++){
      dw=rand()%4096;
      SPI_writeReg(addr, W_ADDR_REG);
      SPI_writeReg(dw, W_DAC_DATA);
      if((dr=SPI_readReg(R_DAC_DATA))!=dw) {
        usr_pdOff();
        qDebug()<<"Error mem test 1! Mem R/W address"<<addr<<"Write data"<<dw<<"Read data"<<dr;
        return;
      }
    }
    qDebug()<<"Mem test 1 Ok. Write/Read rnd data. Ok";

    qDebug()<<"Mem 2 write/delay/read is begin";

    for(addr=0;addr<size;addr++){
        wd[addr]=rand()%4096;
        SPI_writeReg(addr, W_ADDR_REG);
        SPI_writeReg(wd[addr], W_DAC_DATA);
    }
    m_sleep(1000);
    for(addr=0;addr<size;addr++){
      SPI_writeReg(addr, W_ADDR_REG);
      if((dr=SPI_readReg(R_DAC_DATA))!=wd[addr]) {
        usr_pdOff();
        qDebug()<<"Error mem test 2! Mem R/W address"<<addr<<"Write data"<<wd[addr]<<"Read data"<<dr;
        return;
      }
    }
    qDebug()<<"Mem test 2 Ok. Write/Read rnd data with delay. Ok.";

    qDebug()<<"Mem test 3 with address increment is begin.";
    for(addr=0;addr<size;addr++){
        wd[addr]=rand()%4096;
    }
    SPI_writeReg(0, W_ADDR_REG);
    SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);

    for(addr=0;addr<size;addr++){
        SPI_writeReg(addr, W_ADDR_REG);
        if((dr=SPI_readReg(R_DAC_DATA))!=(wd[addr]&0xffff)) {
          usr_pdOff();
          qDebug()<<"Error mem test 3! Mem R/W address"<<addr<<"Write data"<<wd[addr]<<"Read data"<<dr;
          return;
        }
    }
    qDebug()<<"Mem test 3 Ok. Write/Read rnd data with address increment. Ok.";
    usr_pdOff();
}

void THw::usr_DacMemTest(int rep)
{
  //const u_int32_t size=8192;
  u_int32_t addr;
  u_int32_t wd[size], rd[size];
  usr_pdOn(); // operation only from mem
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
  usr_pdOff();
}
void THw::usr_WrDACConst(u_int32_t d)
{
  u_int32_t wd[size], rd[size];
  u_int32_t addr;
  for(addr=0;addr<size;addr+=2){
      wd[addr]=d;
      wd[addr+1]=d;
  }
  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
}
void THw::usr_WrDACSaw(void)
{
  u_int32_t wd[size], rd[size];
  u_int32_t addr,par=(size+1)/2048;
  //if(par>1)par--;
  for(addr=0;addr<size;addr+=2){
    wd[addr]=2048-addr/7;
    wd[addr+1]=addr/7+2048;
  }
  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
}
void THw::usr_WrDACRnd(void)
{
  u_int32_t wd[size], rd[size];
  u_int32_t addr;
  for(addr=0;addr<size;addr++){
     wd[addr]=rand()%4096;
  }
  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
}
void THw::usr_WrDACFile(QString fname)
{
  u_int32_t wd[size], rd[size];
  int i=0;

  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

  QTextStream in(&file);
  QString line=in.readLine();
  qDebug()<<line;
  while (!in.atEnd()) {
    line = in.readLine();
    QStringList list1=line.split(",");
    //qDebug()<<list1.at(0)<<list1.at(1)<<list1.at(2);
    wd[i++]=(int)(list1.at(1).toFloat()/100.0*2047)+2047;
    wd[i++]=(int)(list1.at(2).toFloat()/100.0*2047)+2047;
  }
/*  for(int i=0;i<30;i+=2){

    qDebug()<<(rd[i]&0xfff)<<(rd[i+1]&0xfff);
  }*/
  usr_WrSample(250); // in us
  usr_WrStrobe(i/2);
  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
  file.close();
}
void THw::usr_RdADCFile(QString fname)
{
  u_int32_t wd[size], rd[size];
  int i=0;

  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,R_ADCINC_DATA);
  u_int32_t tsample=usr_RdSample(); //in us
  u_int32_t strobe=usr_RdStrobe();
  QTextStream out(&file);

  double t=0; double shADC1=2033,shADC2=2042;
  double kTr=1;//10/4096.;
  double d1,d2;
  out<<"x,y1,y2"<<'\n';
  for(int i=0;i<strobe*2;i+=2){
    d1=(((rd[i])&0xfff)-shADC1)*kTr;
    d2=(((rd[i+1])&0xfff)-shADC2)*kTr;
    QString tmp=QString("%1,%2,%3").arg(t,0,'f',6).arg(d1,0,'f',0).arg(d2,0,'f',0);
    out<<tmp<<'\n';
    t+=tsample/1e6;
  }

  file.close();
}

void THw::usr_RdDacMem(int cnt)
{
  int d1,d2;
  for(int i=0;i<cnt*2;i+=2){
    SPI_writeReg(i, W_ADDR_REG);
    d1=(SPI_readReg(R_DAC_DATA)&0xfff);
    SPI_writeReg(i+1, W_ADDR_REG);
    d2=(SPI_readReg(R_DAC_DATA)&0xfff);
    qDebug()<<"Read Dac1"<<d1<<"read Dac2"<<d2;
  }
}
void THw::usr_RdAdcMem(int cnt)
{
  int d1,d2;
  for(int i=0;i<cnt*2;i+=2){
    SPI_writeReg(i, W_ADDR_REG);
    d1=((SPI_readReg(R_ADC_DATA))&0xffff);
    SPI_writeReg(i+1, W_ADDR_REG);
    d2=((SPI_readReg(R_ADC_DATA))&0xffff);
    qDebug()<<"Read Adc1"<<d1<<"Adc2"<<d2;
  }
}

void THw::usr_RdStatus(void)
{
  qDebug()<<"Read status reg"<<SPI_readReg(R_STATUS_REG);
    //while((SPI_readReg(R_STATUS_REG)&0x20)){ qDebug() <<"1"<<SPI_readReg(R_STATUS_REG); m_sleep(1);}
  while((SPI_readReg(R_STATUS_REG)&0x40)==0) { qDebug() <<"2"<< SPI_readReg(R_STATUS_REG); m_sleep(1);}
}
void THw::usr_waitCanRead(void)
{
  while((SPI_readReg(R_STATUS_REG)&0x40)==0) {  m_sleep(1);}
}
void THw::usr_waitStrobe(void)
{
  while((SPI_readReg(R_STATUS_REG)&0x20)==0) {  m_sleep(1);}
}
void THw::usr_waitForever(void)
{
  qDebug()<<"Ctrl-C for exit.";
  while(1) m_sleep(100);
}
