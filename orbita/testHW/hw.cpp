
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


/****************************************************************
 * Object constructor
 * c - size of array for DAC/ADC data max 32768
 ****************************************************************/
THw::THw(int c):size(c)
{
  fd_spi=0;
  abort=false;
}
/****************************************************************
 * destructor
 * Unexport pins and free spi1
 ****************************************************************/
THw::~THw()
{
   abort=true;
   wait();
   gpio_fd_close(fd_d0);
  pins_unexport();
  if(fd_spi>0) close(fd_spi);
}


void THw::run()
{
    QEventLoop *loop;
    loop=new QEventLoop();
    while(!abort){
        loop->processEvents();
        int ev= polling_gpios(10);
        if(ev) {
            qDebug()<<"Event WDO";
            qDebug()<<"Event rising status="<<usr_rdStatus();
            //if((usr_RdStatus()&1)==0)
            //configureFPGA("orbita_project.rbf");
            //usr_rst();

        }
    }
}

/****************************************************************
 * gpio_export
 ****************************************************************/
int THw::pins_export(void)
{
  qDebug()<<"Set pins export";
  if(gpio_export(PIN_HOST_RST)) return 1;
  if(gpio_export(PIN_HOST_START)) return 1;
  if(gpio_export(PIN_HOST_STOP))return 1;
  if(gpio_export(PIN_LED_SYSTEM)) return 1;
  if(gpio_export(PIN_WDO)) return 1;
  return 0;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int THw::pins_unexport(void)
{
    qDebug()<<"Set pins unexport";
    gpio_unexport(PIN_HOST_RST);
    gpio_unexport(PIN_HOST_START);
    gpio_unexport(PIN_HOST_STOP);
    gpio_unexport(PIN_LED_SYSTEM);
    gpio_unexport(PIN_WDO);
    return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int THw::pins_set_dir(void)
{
  qDebug()<<"Set pins direction";
  if(gpio_set_dir(PIN_HOST_RST,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_HOST_START,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_HOST_STOP,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_LED_SYSTEM,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_WDO,INPUT_PIN)) return 1;

  return 0;
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
  if(gpio_set_edge(PIN_WDO,"rising")) return 1;
  fd_d0=gpio_fd_open(PIN_WDO);
  epfd=epoll_create(1);
  ev_d0.events=EPOLLET|EPOLLIN;
  ev_d0.data.fd=fd_d0;
  epoll_ctl(epfd,EPOLL_CTL_ADD,fd_d0,&ev_d0);
  fd_spi=setSpi();
  if(fd_spi<=0) return 1;

  usr_hostStartOn();
  //usr_hostStopOff();
  usr_sysLedOff();
  return 0;
}

void THw::usr_rst(void)
{
  gpio_set_value(PIN_HOST_RST,HIGH);
  m_sleep(10);// sleep 10 ms
  qDebug()<<"RST";
  gpio_set_value(PIN_HOST_RST,LOW);
}
void THw::usr_hostStartOn()
{
  gpio_set_value(PIN_HOST_START,HIGH);
}
void THw::usr_hostStartOff()
{
  gpio_set_value(PIN_HOST_START,LOW);
}
void THw::usr_hostStartPulse()
{
  gpio_set_value(PIN_HOST_START,HIGH);
  gpio_set_value(PIN_HOST_START,LOW);
}

void THw::usr_hostStopOn()
{
  gpio_set_value(PIN_HOST_STOP,HIGH);
}
void THw::usr_hostStopOff()
{
  gpio_set_value(PIN_HOST_STOP,LOW);
}
void THw::usr_hostStopPulse()
{
   gpio_set_value(PIN_HOST_STOP,HIGH);
  gpio_set_value(PIN_HOST_STOP,LOW);
}

void THw::usr_sysLedOn()
{
  gpio_set_value(PIN_LED_SYSTEM,LOW);
}
void THw::usr_sysLedOff()
{
  gpio_set_value(PIN_LED_SYSTEM,HIGH);
}

int THw::usr_getWDO(void)
{
  unsigned int val;
  if(!gpio_get_value(PIN_WDO,&val)) return val;
  return -1;
}

int THw::polling_gpios(int timePolling)
{
   int value = 0 ,n,i;

   n = epoll_wait(epfd, events, 10, timePolling);

  for ( i = 0;  i < n; ++i) {
    if (events[i].data.fd == ev_d0.data.fd) value|=1;
  }
  return value;
}

//------------------------------------------------- Host SPI functions ---------------------------------

/****************************************************************
 * SPI write register
 ****************************************************************/
void THw::SPI_writeReg(u_int32_t data, u_int32_t reg)
{
    u_int32_t awd,ard,*ar,*aw;
    ar=&ard; aw=&awd;
    awd=W_WEN_REG | ENABLE_WRITE; // can write on SPI
    transfer(fd_spi,aw,ar,4);

    awd=(reg&0xf00000)|(data&0xffff);
    transfer(fd_spi,aw,ar,4);

    awd=W_WEN_REG |  DISABLE_WRITE; // can't write on SPI
    transfer(fd_spi,aw,ar,4);
}

/****************************************************************
 * SPI read register
 ****************************************************************/
u_int32_t  THw::SPI_readReg(u_int32_t reg)
{
  u_int32_t awd,ard,*ar,*aw,tard;
  ar=&ard; aw=&awd;
  awd=W_WEN_REG | ENABLE_WRITE; // can write on SPI
  transfer(fd_spi,aw,ar,4);

  awd=reg&0xf00000;
  transfer(fd_spi,aw,ar,4);
  tard=ard&0xffff;
  awd=W_WEN_REG |  DISABLE_WRITE; // can't write on SPI
  transfer(fd_spi,aw,ar,4);
  return (tard);
}

/****************************************************************
 * SPI write/read data (big arrays)
 ****************************************************************/
void THw::SPI_writeReadData(u_int32_t *dataw, u_int32_t *datar,u_int32_t len,u_int32_t reg)
{
    u_int32_t *ar,*aw,sz,sz1;
    ar=datar; aw=dataw;
    for(u_int32_t i=0;i<len;i++){
     *dataw=(reg&0xf00000)|(*dataw&0xffff);
     *datar=0;
     dataw++;datar++;
    }
    sz=len/fifo_size;
    sz1=len%fifo_size;
    u_int32_t awd,ard,*arr,*awr;
    arr=&ard; awr=&awd;
    awd=W_WEN_REG | ENABLE_WRITE; // can write on SPI
    transfer(fd_spi,awr,arr,4);
    while(sz>0){
      transfer(fd_spi,aw,ar,4*fifo_size);
      aw+=fifo_size; ar+=fifo_size;
      sz--;
    }
    if(sz1>0){
      transfer(fd_spi,aw,ar,4*sz1);
    }
    awd=W_WEN_REG |  DISABLE_WRITE; // can't write on SPI
    transfer(fd_spi,awr,arr,4);
}


/****************************************************************
 * Get HW version from Altera two bytes 10100101&HIGH&LOW (HIGH LOW 4 bits width)
 ****************************************************************/
int THw::usr_getHW(void)
{
  return SPI_readReg(R_FW_REG);
}
// write timers values
void THw::usr_wrTimerVal(u_int32_t val, u_int32_t index)
{
  SPI_writeReg(index, W_ADDR_REG);  // write address register
  SPI_writeReg(val*10, W_TIME_REG); // from ms to timer value frq=10kHz
}

//read timers values
u_int32_t THw::usr_rdTimerVal( u_int32_t index)
{
  SPI_writeReg(index, W_ADDR_REG);  // write address register
  return SPI_readReg(R_TIME_REG); //read timer value
}

// write mode of operation register
void THw::usr_wrMode(u_int32_t mode)
{
  SPI_writeReg(mode, W_MODE_REG);
}

// read mode of operation register
u_int32_t THw::usr_rdMode(void)
{
  return SPI_readReg(R_MODE_REG);
}

// write addres of memory
void THw::usr_wrAddress(u_int32_t addr)
{
    SPI_writeReg(addr, W_ADDR_REG);  // write address register
}

// read address of memory
u_int32_t THw::usr_rdAddress(u_int32_t addr)
{
    return(SPI_readReg(addr)&0x3ffff);
}

// write memory with increment
void THw::usr_wrMem(u_int32_t *data,u_int32_t len)
{
    QVector <u_int32_t> datar;
    datar.resize(len);
    SPI_writeReadData(data, datar.data(), len,W_DMEM_REG);
    datar.clear();
}

//read memory with increment
void THw::usr_rdMem(u_int32_t *data, u_int32_t len)
{
   QVector <u_int32_t> dataw;
   dataw.resize(len);
   SPI_writeReadData(dataw.data(), data, len,R_DMEM_REG);
   dataw.clear();
}

void THw::usr_rdADCFile(QString fname)
{
  u_int32_t wd[size], rd[size];
  int i=0;

  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,R_DMEM_REG);

  QTextStream out(&file);

  double t=0; double shADC1=2033,shADC2=2042;
  double kTr=1;//10/4096.;
  double d1,d2;
  out<<"x,y1,y2"<<'\n';
  for(int i=0;i<10*2;i+=2){
    d1=(((rd[i])&0xfff)-shADC1)*kTr;
    d2=(((rd[i+1])&0xfff)-shADC2)*kTr;
    QString tmp=QString("%1,%2,%3").arg(t,0,'f',6).arg(d1,0,'f',0).arg(d2,0,'f',0);
    out<<tmp<<'\n'; t+=0.01;
  }

  file.close();
}

void THw::usr_rdNMem(int cnt)
{
  int d1;
  for(int i=0;i<cnt;i++){
    SPI_writeReg(i, W_ADDR_REG);
    d1=((SPI_readReg(R_DMEM_REG))&0xffff);
    qDebug()<<"Read Adc"<<d1;
  }
}

void THw::usr_rdADCBlockOnePoint(int point)
{
  QVector <u_int32_t> datar;

  u_int32_t size=32;
  datar.resize(size);
  SPI_writeReg(0, W_ADDR_REG);
  usr_rdMem(datar.data(), size);

  u_short dataLine,dataAdc0[16],dataAdc1[16];
  short dAdc1,dAdc2;
  int i;
  for(i=0;i<16;i++) {
     dataAdc0[i]=0;
     dataAdc1[i]=0;
  }
  /*for(i=0;i<31;i+=2) {
    datar[i]=0xaaa8;
  }
  for(i=1;i<32;i+=2) {
    datar[i]=0x555;
  }*/
  for(i=0;i<16;i++) {
    dataLine=(u_short )datar[i*2];
    dataAdc0[15] |= (((dataLine&0x8000)>>i));
    dataAdc0[14] |= ((dataLine&0x4000)<<1)  >>i;
    dataAdc0[13] |= ((dataLine&0x2000)<<2)  >>i;
    dataAdc0[12] |= ((dataLine&0x1000)<<3)  >>i;
    dataAdc0[11] |= ((dataLine&0x0800)<<4)  >>i;
    dataAdc0[10] |= ((dataLine&0x0400)<<5)  >>i;
    dataAdc0[9  ] |= ((dataLine&0x0200)<<6)  >>i;
    dataAdc0[8  ] |= ((dataLine&0x0100)<<7 ) >>i;
    dataAdc0[7  ] |= ((dataLine&0x0080)<<8)  >>i;
    dataAdc0[6  ] |= ((dataLine&0x0040)<<9)  >>i;
    dataAdc0[5  ] |= ((dataLine&0x0020)<<10)>>i;
    dataAdc0[4  ] |= ((dataLine&0x0010)<<11)>>i;
    dataAdc0[3  ] |= ((dataLine&0x0008)<<12)>>i;
    dataAdc0[2  ] |= ((dataLine&0x0004)<<13)>>i;
    dataAdc0[1  ] |= ((dataLine&0x0002)<<14)>>i;
    dataAdc0[0  ] |=(((dataLine&0x0001)<<15)>>i);
  }

  for(i=0;i<16;i++) {
    dataLine=(u_short )datar[i*2+1];
    dataAdc1[15] |= (((dataLine&0x8000)>>i));
    dataAdc1[14] |= ((dataLine&0x4000)<<1)  >>i;
    dataAdc1[13] |= ((dataLine&0x2000)<<2)  >>i;
    dataAdc1[12] |= ((dataLine&0x1000)<<3)  >>i;
    dataAdc1[11] |= ((dataLine&0x0800)<<4)  >>i;
    dataAdc1[10] |= ((dataLine&0x0400)<<5)  >>i;
    dataAdc1[9  ] |= ((dataLine&0x0200)<<6)  >>i;
    dataAdc1[8  ] |= ((dataLine&0x0100)<<7 ) >>i;
    dataAdc1[7  ] |= ((dataLine&0x0080)<<8)  >>i;
    dataAdc1[6  ] |= ((dataLine&0x0040)<<9)  >>i;
    dataAdc1[5  ] |= ((dataLine&0x0020)<<10)>>i;
    dataAdc1[4  ] |= ((dataLine&0x0010)<<11)>>i;
    dataAdc1[3  ] |= ((dataLine&0x0008)<<12)>>i;
    dataAdc1[2  ] |= ((dataLine&0x0004)<<13)>>i;
    dataAdc1[1  ] |= ((dataLine&0x0002)<<14)>>i;
    dataAdc1[0  ] |=(((dataLine&0x0001)<<15)>>i);
  }

  float adc1,adc2;
  for(i=0;i<16;i++)  { //qDebug()<<"Data ch"<<i<<"Adc1"<<(dataAdc0[i]&0x0fff)<<"Adc2"<<(dataAdc1[i]&0x0fff);
     //dAdc1=((~dataAdc0[i])&0xfff);  if(dAdc1&0x800) dAdc1 |=0xf000;
     //dAdc2=(~(dataAdc1[i])&0xfff);  if(dAdc2&0x800) dAdc2 |=0xf000;
    dAdc1=((dataAdc0[i])&0xfff);  if(dAdc1&0x800) dAdc1 |=0xf000; dAdc1=(~dAdc1)+1;
    dAdc2=((dataAdc1[i])&0xfff);  if(dAdc2&0x800) dAdc2 |=0xf000; dAdc2=(~dAdc2)+1;
     adc1=dAdc1*10.0/4095.0; adc2=dAdc2*10.0/4095.0;
   // printf("Ch=%2d Adc1=%6d  %5d  %6.3lf Adc2=%6d %5d %6.3lf\n",i,dataAdc0[i],dAdc1,adc1,dataAdc1[i],dAdc2,adc2);
      printf("Ch=%2d Adc1=%6.3lf Adc2=%6.3lf\n",i+1,adc1,adc2);
  }
}

void THw::usr_rdADCBlockOnePoint(QVector<floatA> &dfAdc1, QVector<floatA> &dfAdc2, int adcN)
{
// Max Code in Voltage ADC1 5.28V ADC2 5.3V
  QVector <u_int32_t> datar;
  u_int32_t size=32*16*2;

  int16_t zeroShAdc1[16] ={0,    0,    0,    1,    1,    3,   -1,   -1,   -2,    2,    2,    8,    4,    2,    4,    0} ;
  //int16_t zeroShAdc1[16] ={0,    0,   0,    0,    0,    0,    0,  0,    0,    0,   0,    0,    0,    0,    0,    0 } ;
  int16_t zeroShAdc2[16]={-3,   -2,    2,   0,   -6,    0,   -2,    2,   -4,   -1,    3,    0,    4,   -5,   -1,   -3} ;//work
 // int16_t zeroShAdc2[16] ={0,    0,   0,    0,    0,    0,    0,  0,    0,    0,   0,    0,    0,    0,    0,    0 } ;

  float kADC1[16]= {1.07669,	1.07141,	1.06607,	1.07187,	1.07508,	1.07147,	1.07301,	1.07554,	1.08436,	1.08412,	1.08040,	1.07843,	1.07474,	1.07187,	1.07581,	1};
  //float kADC1[16]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float kADC2[16]= {1.07900,	1.07843,	1.07982,	1.07577,	1.07635,	1.07428,	1.07577,	1.07359,	1.07073,	1.07336,	1.07993,	1.07359,	1.07866,	1.07050,	1.07485, 1.073};
  //float kADC1[16]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  QVector <int16_t> dIAdc1(16),dIAdc2(16);
  double fadc1[32][16],fadc2[32][16];
  datar.resize(size);
  dIAdc1.resize(32); dIAdc2.resize(32);
  SPI_writeReg(0, W_ADDR_REG);
  usr_rdMem(datar.data(), size);
  u_short dataLine1,dataLine2,dataAdc1[16],dataAdc2[16];
  int16_t dAdc1,dAdc2;
  int i,n=0;
  for(int j=0;j<32;j++) {
    for(i=0;i<16;i++) {
       dataAdc1[i]=0;
       dataAdc2[i]=0;
    }
    for(i=0;i<16;i++) {
      dataLine1=(u_short )datar[n*2];
      dataAdc1[15] |= (((dataLine1&0x8000)>>i));
      dataAdc1[14] |= ((dataLine1&0x4000)<<1)  >>i;
      dataAdc1[13] |= ((dataLine1&0x2000)<<2)  >>i;
      dataAdc1[12] |= ((dataLine1&0x1000)<<3)  >>i;
      dataAdc1[11] |= ((dataLine1&0x0800)<<4)  >>i;
      dataAdc1[10] |= ((dataLine1&0x0400)<<5)  >>i;
      dataAdc1[9  ] |= ((dataLine1&0x0200)<<6)  >>i;
      dataAdc1[8  ] |= ((dataLine1&0x0100)<<7 ) >>i;
      dataAdc1[7  ] |= ((dataLine1&0x0080)<<8)  >>i;
      dataAdc1[6  ] |= ((dataLine1&0x0040)<<9)  >>i;
      dataAdc1[5  ] |= ((dataLine1&0x0020)<<10)>>i;
      dataAdc1[4  ] |= ((dataLine1&0x0010)<<11)>>i;
      dataAdc1[3  ] |= ((dataLine1&0x0008)<<12)>>i;
      dataAdc1[2  ] |= ((dataLine1&0x0004)<<13)>>i;
      dataAdc1[1  ] |= ((dataLine1&0x0002)<<14)>>i;
      dataAdc1[0  ] |=(((dataLine1&0x0001)<<15)>>i);

      dataLine2=(u_short )datar[n*2+1];
      dataAdc2[15] |= (((dataLine2&0x8000)>>i));
      dataAdc2[14] |= ((dataLine2&0x4000)<<1)  >>i;
      dataAdc2[13] |= ((dataLine2&0x2000)<<2)  >>i;
      dataAdc2[12] |= ((dataLine2&0x1000)<<3)  >>i;
      dataAdc2[11] |= ((dataLine2&0x0800)<<4)  >>i;
      dataAdc2[10] |= ((dataLine2&0x0400)<<5)  >>i;
      dataAdc2[9  ] |= ((dataLine2&0x0200)<<6)  >>i;
      dataAdc2[8  ] |= ((dataLine2&0x0100)<<7 ) >>i;
      dataAdc2[7  ] |= ((dataLine2&0x0080)<<8)  >>i;
      dataAdc2[6  ] |= ((dataLine2&0x0040)<<9)  >>i;
      dataAdc2[5  ] |= ((dataLine2&0x0020)<<10)>>i;
      dataAdc2[4  ] |= ((dataLine2&0x0010)<<11)>>i;
      dataAdc2[3  ] |= ((dataLine2&0x0008)<<12)>>i;
      dataAdc2[2  ] |= ((dataLine2&0x0004)<<13)>>i;
      dataAdc2[1  ] |= ((dataLine2&0x0002)<<14)>>i;
      dataAdc2[0  ] |=(((dataLine2&0x0001)<<15)>>i);
      n++;
    }
    for(i=0;i<16;i++){
      dAdc1=((dataAdc1[i])&0xfff);  if(dAdc1&0x800) dAdc1 |=0xf000; dAdc1=(~dAdc1)+1-zeroShAdc1[i];
      dAdc2=((dataAdc2[i])&0xfff);  if(dAdc2&0x800) dAdc2 |=0xf000; dAdc2=(~dAdc2)+1-zeroShAdc2[i];
      fadc1[j][i]=dAdc1*10.0/4095.0;  fadc2[j][i]=dAdc2*10.0/4095.0;
      dIAdc1[j,i]=dAdc1;
      dIAdc2[j,i]=dAdc2;
      dfAdc1[j][i]=dAdc1*10.0/4095.0*kADC1[i];
      dfAdc2[j][i]=dAdc2*10.0/4095.0*kADC2[i];
    }
  }
/*
  if(adcN==1){
      for(i=0;i<16;i++)  {
//        printf("time= %2d ",i); for(int k=0;k<16;k++) printf("%6.3lf ",fadc1[i][k]*kADC1[k]); printf("\n");
      }
      for(i=0;i<16;i++)  {
        printf("time= %2d ",i); for(int k=0;k<16;k++) printf("%4d ",dIAdc1[i,k]); printf("\n");
      }
  }
  else{
    for(i=0;i<16;i++)  {
//      printf("time= %2d ",i); for(int k=0;k<16;k++) printf("%6.3lf ",fadc2[i][k]*kADC2[k]); printf("\n");
    }
    for(i=0;i<16;i++)  {
      printf("time= %2d ",i); for(int k=0;k<16;k++) printf("%4d ",dIAdc2[i,k]); printf("\n");
    }
  }
*/
  dIAdc1.clear(); dIAdc2.clear();
}

// allPoints for one ADC
//return values dfAdc1 dfAdc2 in Volts with shift and coefficient compensation
void THw::usr_rdADCData(QVector<floatA> &dfAdc1, QVector<floatA> &dfAdc2, int allPoints)
{
// Max Code in Voltage ADC1 5.28V ADC2 5.3V

  int16_t zeroShAdc1[16] ={0,    0,    0,    1,    1,    3,   -1,   -1,   -2,    2,    2,    8,    4,    2,    4,    0} ;
  //int16_t zeroShAdc1[16] ={0,    0,   0,    0,    0,    0,    0,  0,    0,    0,   0,    0,    0,    0,    0,    0 } ;
  int16_t zeroShAdc2[16]={-3,   -2,    2,   0,   -6,    0,   -2,    2,   -4,   -1,    3,    0,    4,   -5,   -1,   -3} ;//work
 // int16_t zeroShAdc2[16] ={0,    0,   0,    0,    0,    0,    0,  0,    0,    0,   0,    0,    0,    0,    0,    0 } ;

  float kADC1[16]= {1.07669,	1.07141,	1.06607,	1.07187,	1.07508,	1.07147,	1.07301,	1.07554,	1.08436,	1.08412,	1.08040,	1.07843,	1.07474,	1.07187,	1.07581,	1};
  //float kADC1[16]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  float kADC2[16]= {1.07900,	1.07843,	1.07982,	1.07577,	1.07635,	1.07428,	1.07577,	1.07359,	1.07073,	1.07336,	1.07993,	1.07359,	1.07866,	1.07050,	1.07485, 1.073};
  //float kADC1[16]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

  QVector <u_int32_t> datar;
  u_int32_t size=allPoints*2*16; // because we have 2 ADC and 16 time points
  datar.resize(size);
  SPI_writeReg(0, W_ADDR_REG);
  usr_rdMem(datar.data(), size);

  u_short dataLine1,dataLine2,dataAdc1[16],dataAdc2[16];
  int16_t dAdc1,dAdc2;
  int i,n=0;
  for(int j=0;j<allPoints;j++) {
    for(i=0;i<16;i++) {
       dataAdc1[i]=0;
       dataAdc2[i]=0;
    }
    for(i=0;i<16;i++) {
      dataLine1=(u_short )datar[n*2];
      dataAdc1[15] |= (((dataLine1&0x8000)>>i));
      dataAdc1[14] |= ((dataLine1&0x4000)<<1)  >>i;
      dataAdc1[13] |= ((dataLine1&0x2000)<<2)  >>i;
      dataAdc1[12] |= ((dataLine1&0x1000)<<3)  >>i;
      dataAdc1[11] |= ((dataLine1&0x0800)<<4)  >>i;
      dataAdc1[10] |= ((dataLine1&0x0400)<<5)  >>i;
      dataAdc1[9  ] |= ((dataLine1&0x0200)<<6)  >>i;
      dataAdc1[8  ] |= ((dataLine1&0x0100)<<7 ) >>i;
      dataAdc1[7  ] |= ((dataLine1&0x0080)<<8)  >>i;
      dataAdc1[6  ] |= ((dataLine1&0x0040)<<9)  >>i;
      dataAdc1[5  ] |= ((dataLine1&0x0020)<<10)>>i;
      dataAdc1[4  ] |= ((dataLine1&0x0010)<<11)>>i;
      dataAdc1[3  ] |= ((dataLine1&0x0008)<<12)>>i;
      dataAdc1[2  ] |= ((dataLine1&0x0004)<<13)>>i;
      dataAdc1[1  ] |= ((dataLine1&0x0002)<<14)>>i;
      dataAdc1[0  ] |=(((dataLine1&0x0001)<<15)>>i);

      dataLine2=(u_short )datar[n*2+1];
      dataAdc2[15] |= (((dataLine2&0x8000)>>i));
      dataAdc2[14] |= ((dataLine2&0x4000)<<1)  >>i;
      dataAdc2[13] |= ((dataLine2&0x2000)<<2)  >>i;
      dataAdc2[12] |= ((dataLine2&0x1000)<<3)  >>i;
      dataAdc2[11] |= ((dataLine2&0x0800)<<4)  >>i;
      dataAdc2[10] |= ((dataLine2&0x0400)<<5)  >>i;
      dataAdc2[9  ] |= ((dataLine2&0x0200)<<6)  >>i;
      dataAdc2[8  ] |= ((dataLine2&0x0100)<<7 ) >>i;
      dataAdc2[7  ] |= ((dataLine2&0x0080)<<8)  >>i;
      dataAdc2[6  ] |= ((dataLine2&0x0040)<<9)  >>i;
      dataAdc2[5  ] |= ((dataLine2&0x0020)<<10)>>i;
      dataAdc2[4  ] |= ((dataLine2&0x0010)<<11)>>i;
      dataAdc2[3  ] |= ((dataLine2&0x0008)<<12)>>i;
      dataAdc2[2  ] |= ((dataLine2&0x0004)<<13)>>i;
      dataAdc2[1  ] |= ((dataLine2&0x0002)<<14)>>i;
      dataAdc2[0  ] |=(((dataLine2&0x0001)<<15)>>i);
      n++;
    }
    for(i=0;i<16;i++){
      dAdc1=((dataAdc1[i])&0xfff);  if(dAdc1&0x800) dAdc1 |=0xf000; dAdc1=(~dAdc1)+1-zeroShAdc1[i];
      dAdc2=((dataAdc2[i])&0xfff);  if(dAdc2&0x800) dAdc2 |=0xf000; dAdc2=(~dAdc2)+1-zeroShAdc2[i];
      dfAdc1[j][i]=dAdc1*10.0/4095.0*kADC1[i];
      dfAdc2[j][i]=dAdc2*10.0/4095.0*kADC2[i];
    }
  }
}

int THw::usr_rdStatus(void)
{
  return SPI_readReg(R_STATUS_REG);
}
u_int32_t THw::usr_rdAllWords(void)
{
  return SPI_readReg(R_ALLWS_REG)&0xfff;
}

void THw::usr_waitCanRead(void)
{
  while((SPI_readReg(R_STATUS_REG)&0x40)==0) {  m_sleep(1);}
}
void THw::usr_waitStrobe(void)
{
  while((SPI_readReg(R_STATUS_REG)&0x20)==0) {  m_sleep(1);}
}

/****************************************************************
 * Load FPGA functiom
 ****************************************************************/
// 1    - DCLK      P1.33 GPIO0_9   HOST    GPIO09  OUT prog
// 3    - CONF_D P1.34 GPIO2_17 FPGA    GPIO81  IN     prog
// 5    - nCONF    P1.36 GPIO2_16 HOST   GPIO80  OUT prog
// 6    - nCE          P1.37 GPIO2_14 HOST   GPIO78  OUT LOW
// 7    - Data0      P1.38 GPIO2_15  HOST   GPIO79  OUT prog
// 8    - nCSO      P1.39 GPIO2_12 HOST    GPIO76  IN     don't use
// 9    - ASD         P1.40 GPIO2_13 HOST    GPIO77  IN     don't use
// Abs  - nSTATUS   P1.35 GPIO0_8 FPGA  GPIO08  IN     prog
//
// 2,10  - gnd
//---        --------------------------------------- nCONF (host)
//  I         I
//  I------I
//-------         ---------------------------------- nSTATUS (FPGA )
//         I         I
//         I------I
//-------                                                         --- CONF_D (FPGA)
//         I                                                         I
//         I---------------------~~~~-----------I
//
//                   ---   ---   ---   ---   ---   ---   ---- DCLK (host)
//                    I  I  I  I   I  I   I  I   I  I   I  I   I
//-------------I  I--I  I--I  I--I  I--I  I--I  I--I
//               ---------------------------------------
// ----------                                                       Data0 LSB is first (host)
//               ---------------------------------------clock data on rise DCLK
// Loading time ~150 ms (30 kB file)
int THw::configureFPGA(char * fName)
{
    volatile void  *gpio0_addr = NULL,*gpio2_addr = NULL;
    int fd,mfd,rb,i;
    unsigned long j,DataLoad;
    unsigned char dataForLoad[512];

    fd = open("/dev/mem", O_RDWR|MS_SYNC);
    gpio0_addr =  mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, fd, GPIO0_START_ADDR);
    if(gpio0_addr == MAP_FAILED) {
        perror("Unable to map GPIO0\n");
       return LOADFPGA_ERR_MEMMAP;
    }
    GPIO_SETDIRECTIONIN(gpio0_addr,(PIN_NSTAT)); //  in
    GPIO_SETDIRECTIONOUT(gpio0_addr,(PIN_DCLK)); //out
   // printf("GPIO0 pins configuration: %X\n", READREG32(gpio0_addr+GPIO_OE));

    gpio2_addr =  mmap(0, GPIO2_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, fd, GPIO2_START_ADDR);
    if(gpio2_addr == MAP_FAILED) {
        perror("Unable to map GPIO2\n");
       return LOADFPGA_ERR_MEMMAP;
    }
    GPIO_SETDIRECTIONIN(gpio2_addr,(PIN_CONFDONE | PIN_NCS0 | PIN_ASD)); //  in
    GPIO_SETDIRECTIONOUT(gpio2_addr,(PIN_NCONFIG | PIN_DATA0 |  PIN_NCE)); //out
    //printf("GPIO2 pins configuration: %X\n", READREG32(gpio2_addr+GPIO_OE));

    // printf("%s\n", *((unsigned char *) &x) == 0 ? "big-endian"\ : "little-endian");
    mfd=open(fName,O_RDONLY);
    if (mfd<0) {
      perror("Timer FPGA file open. File not found orb.rbf");
      return(LOADFPGA_ERR_FNAME);
    }

    // form nCONFIG pulse
    GPIO_CLEARBIT32(gpio0_addr,PIN_DCLK); //CLK Off
    //one bit set/clear ~0,375 us for tam3517 100us uSOMIQ from i/o 0.2 us im memory map mode

    for(i=0;i<20;i++) GPIO_SETBIT32(gpio2_addr,PIN_NCONFIG);// nConfig On and wait ~4us

    for(i=0;i<300;i++)  GPIO_CLEARBIT32(gpio2_addr,PIN_NCONFIG); // nConfig Off and wait ~60us
    for(i=0;i<200;i++)  GPIO_SETBIT32(gpio2_addr,PIN_NCONFIG);     // nConfig On and wait ~40us
    unsigned int val=0;
    while((GPIO_READDATA32(gpio0_addr)&PIN_NSTAT)==0); // wait nSTATUS going to 1

    while((rb=read(mfd,dataForLoad,512))>0){
      for(i=0;i<rb;i++) {
        DataLoad=dataForLoad[i];
        for(j=0;j<8;j++) {
          if(DataLoad&0x01)
            GPIO_SETBIT32(gpio2_addr,PIN_DATA0); // data0 On
          else
             GPIO_CLEARBIT32(gpio2_addr,PIN_DATA0); // data0 Off
          DataLoad>>=1;
          GPIO_SETBIT32(gpio0_addr,PIN_DCLK); //CLK On
          GPIO_CLEARBIT32(gpio0_addr,PIN_DCLK); //CLK Off
        }
      }
    }
    i=0;
    close(mfd);
    val=0;
    while(val&&(i++<TIMEOUT_ERR))  {
       val=GPIO_READDATA32(gpio2_addr)&PIN_CONFDONE;
       GPIO_SETBIT32(gpio0_addr,PIN_DCLK); //CLK On
       GPIO_CLEARBIT32(gpio0_addr,PIN_DCLK); //CLK Off
    }

    munmap((void*)gpio0_addr,GPIO0_SIZE);
    munmap((void*)gpio2_addr,GPIO2_SIZE);
    close(fd);
    if(i>=TIMEOUT_ERR) return(2);
  return(0);
}
