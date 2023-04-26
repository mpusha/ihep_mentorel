
/*
 * hw.cpp
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
THw::THw(int size, double coeffDac, int dac1Sh, int dac2Sh):QObject(),size(size),coeffDac(coeffDac),dac1Shift(dac1Sh),dac2Shift(dac2Sh)
{
  qDebug()<<"Start constructor of object THw";
  fd_spi=0;
  coeffAdc=1000.0/2048.0;
  adc1Shift=2047;
  adc2Shift=2047;
  qDebug()<<"Constructor of object THw was finished";
}

/****************************************************************
 * destructor
 * Unexport pins and free spi1
 ****************************************************************/
THw::~THw()
{
  qDebug()<<"Start destructor of object THw.";
  pins_unexport();
  if(fd_spi>0) close(fd_spi);
  qDebug()<<"Destructor of object THw was finished";
}

//------------------------------------------------- Common functions ---------------------------------
/****************************************************************
 * initialisation of bbb hardware (SPI, GPIO)
 ****************************************************************/
int THw::initalBBHW(void)
{
  qDebug()<<"Initialisation Base Board";
  if(pins_export()) return 1;
  if(pins_set_dir()) return 1;
  fd_spi=setSpi();
  if(fd_spi<=0) return 1;
  return 0;
}

//------------------------------------------------- Host GPIO functions ---------------------------------
/****************************************************************
 * gpio_export
 ****************************************************************/
int THw::pins_export(void)
{
  qDebug()<<"Set pins export";
  if(gpio_export(PIN_HOST_RST)) return 1;
  if(gpio_export(PIN_HOST_DAC_PD)) return 1;
  if(gpio_export(PIN_HOST_WD)) return 1;
  if(gpio_export(PIN_HOST_ALIVE))return 1;
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
    gpio_unexport(PIN_HOST_WD);
    gpio_unexport(PIN_HOST_ALIVE);
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
  if(gpio_set_dir(PIN_HOST_WD,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_HOST_ALIVE,OUTPUT_PIN)) return 1;
  if(gpio_set_dir(PIN_WDO_H,INPUT_PIN)) return 1;

  return 0;
}

/****************************************************************
 * Form reset pulse for FPGA with reset of DAC register clr_dac form in FPGA on reset pulse High
 ****************************************************************/
void THw::usr_rst(void)
{
  gpio_set_value(PIN_HOST_RST,HIGH);
  m_sleep(10);// sleep 10 ms
  gpio_set_value(PIN_HOST_RST,LOW);
}

/****************************************************************
 * DAC power down mode on (output 0)
 ****************************************************************/
void THw::usr_pdOn()
{
  gpio_set_value(PIN_HOST_DAC_PD,LOW);
}

/****************************************************************
 * DAC power down mode off (output can be control)
 ****************************************************************/
void THw::usr_pdOff()
{
  gpio_set_value(PIN_HOST_DAC_PD,HIGH);
}

/****************************************************************
 * Host Watch Dog forming pulse (call from program state macheine
 * pulse on pin host_dac_clr P2_27 GPIO3_19 for fpga V1.4!!! clear dac in reset time
 ****************************************************************/
void THw::usr_hostWd(void)
{
  gpio_set_value(PIN_HOST_WD,HIGH);
  m_sleep(1);
  gpio_set_value(PIN_HOST_WD,LOW);
}

/****************************************************************
 * Host led indication activity of program Led On
 ****************************************************************/
void THw::usr_hostAliveOn(void)
{
  gpio_set_value(PIN_HOST_ALIVE,HIGH);
}

/****************************************************************
 * Host led indication activity of program Led Off
 ****************************************************************/
void THw::usr_hostAliveOff(void)
{
  gpio_set_value(PIN_HOST_ALIVE,LOW);
}

/****************************************************************
 * Host get WDO from ADS706 if high then all Ok. if low we have problems
 * may be Altera or clock 20 MHz or output power. Try reload FPGA
 ****************************************************************/
int THw::usr_getWDO(void)
{
  unsigned int val;
  if(!gpio_get_value(PIN_WDO_H,&val)) return val;
  return -1;
}

/****************************************************************
 * Get HW version from Altera two bytes 10100101&HIGH&LOW (HIGH LOW 4 bits width)
 ****************************************************************/
int THw::usr_getHW(void)
{
  return SPI_readReg(R_FW_REG);
}


//------------------------------------------------- Host SPI functions ---------------------------------
/****************************************************************
 * SPI write register
 ****************************************************************/
void THw::SPI_writeReg(u_int32_t data, u_int32_t reg)
{
  u_int32_t awd,ard,*ar,*aw;
  ar=&ard; aw=&awd;
  awd=(0x00e531); // can write on SPI
  transfer(fd_spi,aw,ar,4);

  awd=(reg&0xf00000)|(data&0xffff);
  transfer(fd_spi,aw,ar,4);

  awd=(0x00e532); // can't write on SPI
  transfer(fd_spi,aw,ar,4);
}

/****************************************************************
 * SPI read register
 ****************************************************************/
u_int32_t  THw::SPI_readReg(u_int32_t reg)
{
  u_int32_t awd,ard,*ar,*aw;
  ar=&ard; aw=&awd;
  awd=reg&0xf00000;
  transfer(fd_spi,aw,ar,4);
  return (ard&0xffff);
}

/****************************************************************
 * SPI write/read data (big arrays)
 ****************************************************************/
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
  u_int32_t awd,ard,*arr,*awr;
  arr=&ard; awr=&awd;
  if(reg==WR_DACADC_DATA) awd=(0x00e531); // can write on SPI for DAC
  transfer(fd_spi,awr,arr,4);
  while(sz>0){
    transfer(fd_spi,aw,ar,4*fifo_size);
    aw+=fifo_size; ar+=fifo_size;
    sz--;
  }
  if(sz1>0){
    transfer(fd_spi,aw,ar,4*sz1);
  }
  awd=(0x00e532); // can't write on SPI
  transfer(fd_spi,awr,arr,4);
}

/****************************************************************
 * write sample time in us (min 32 max 800)
 ****************************************************************/
void THw::usr_WrSample(u_int32_t sample)
{
  SPI_writeReg(sample*20, W_SAMPLE_REG);
}

/****************************************************************
 * read sample time in us (min 32 max 800)
 ****************************************************************/
u_int32_t THw::usr_RdSample(void)
{
  return SPI_readReg(R_SAMPLE_REG)/20;
}

/****************************************************************
 * write strobe (width of output pulse pulse) min 1 max 32767 in therm of sample timer_t
 * e.g. if strbe=10 then output pulse = 320 us
 ****************************************************************/
void THw::usr_WrStrobe(u_int32_t strobe)
{
  SPI_writeReg(strobe, W_STROBE_REG);
}

/****************************************************************
 * read strobe (width of output pulse pulse)
 ****************************************************************/
u_int32_t THw::usr_RdStrobe(void)
{
  return SPI_readReg(R_STROBE_REG);
}

/****************************************************************
 * write mode of work 2 bits low resolve power for PS1
 ****************************************************************/
void THw::usr_writeMode(int data)
{
  SPI_writeReg(data, W_MODE_REG);
}

/****************************************************************
 * write CSV file in DAC memory
 ****************************************************************/
int THw::usr_WrDACFile(QString fname)
{
  u_int32_t wd[size], rd[size];//,wd_extra[size];
  int i=0,j=0;

  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {qDebug()<<"Can't read DAC csv file"<<fname; return(1); }
  for(int i=0;i<size;i+=2){
   wd[i]=dac1Shift;
   wd[i+1]=dac2Shift;
  }
  QTextStream in(&file);
  QString line=in.readLine();
  QStringList list1;
  while (!in.atEnd()) {
    line = in.readLine();
    list1=line.split(",");
    wd[i++]+=(int)(list1.at(1).toFloat()*coeffDac);
    wd[i++]+=(int)(list1.at(2).toFloat()*coeffDac);
    //wd_extra[j++]=(int)(list1.at(3).toFloat()*coeffDac);
    //wd_extra[j++]=(int)(list1.at(4).toFloat()*coeffDac);
    if(i>=size) break;
  }
  if(i>2)i-=2;
  int sample=(int)(list1.at(0).toFloat()*2000000/i);
  usr_WrSample(sample); // in us
  usr_WrStrobe(i/2);
  // write odd part of memory
  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
  // write hight part of memory
  SPI_writeReg(0x8000, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,WR_DACADC_DATA);
  file.close();
  return(0);
}

/****************************************************************
 * read ADC data and write into CSV file on ram disk
 ****************************************************************/
int THw::usr_RdADCFile(QString fname)
{
  u_int32_t wd[size], rd[size];

  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { qDebug()<<"Can't create file on Ram disk"<<fname; return(1); }

  SPI_writeReg(0, W_ADDR_REG);
  SPI_writeReadData(wd,rd,size,R_ADCINC_DATA);
  u_int32_t tsample=usr_RdSample(); //in us
  u_int32_t strobe=usr_RdStrobe();
  QTextStream out(&file);

  double t=0;
  double d1,d2;
  out<<"t,y1,y2"<<'\n';
  for(u_int32_t i=0;i<strobe*2;i+=2){
    d1=((float)((rd[i])&0xfff)-adc1Shift)*coeffAdc;
    d2=((float)((rd[i+1])&0xfff)-adc2Shift)*coeffAdc;
    QString tmp=QString("%1,%2,%3").arg(t,0,'f',6).arg(d1,0,'f',1).arg(d2,0,'f',1);
    out<<tmp<<'\n';
    t+=tsample/1e6;
  }

  file.close();
  return(0);
}

/****************************************************************
 * read status register
// 0-PS1 status On/off from source 1 on, 0 off
// 1-PS1 status error from source 1 error
// 2-PS2 status On/off from source 1 on, 0 off
// 3-PS2 status error from source 1 error
// 4-synchronisation is absent 1 error not go start pulse
// 5-window strobe 1 strobe (work PS)
// 6-window can read 1 can read/write
// 7-reserve
 ****************************************************************/
u_int32_t THw::usr_RdStatus(void)
{
  return SPI_readReg(R_STATUS_REG);
}

/****************************************************************
 * wait time when can read data
 ****************************************************************/
void THw::usr_waitCanRead(void)
{
  while((SPI_readReg(R_STATUS_REG)&0x40)==0) {  m_sleep(1);}
}

/****************************************************************
 * wait strobe present
 ****************************************************************/
void THw::usr_waitStrobe(void)
{
  while((SPI_readReg(R_STATUS_REG)&0x20)==0) {  m_sleep(1);}
}

/*void THw::usr_waitCanWrite(void)
{
  canWr=false;
  int reg=0;
  QTimer timer;
  bool tm=true;
  connect(&timer, SIGNAL(timeout()), this, SLOT(boosterCycleEnd()));
  while(true){
    reg=SPI_readReg(R_STATUS_REG);
    if(reg&0x10) { qDebug()<<"SYNCHR";break;} // synchronisation is absent. Can write data
    if((reg&0x40)&&tm) { timer.start(2000); tm=false; }
    if(canWr) { qDebug()<<"CAN WRITE";break;}
    m_sleep(5);
  }
  timer.stop();
}
*/
bool THw::usr_waitCanWrite(void)
{
  canWr=false;
  int reg=0;
  QTimer timer;
  bool tm=true;

  printf("Status=0x%x\n",SPI_readReg(R_STATUS_REG)&0xff);
  connect(&timer, SIGNAL(timeout()), this, SLOT(boosterCycleEnd()));
  while(true){
    usr_hostWd();
    reg=SPI_readReg(R_STATUS_REG);
    if(reg&0x10) { qDebug()<<"SYNCHR ERROR";break;} // synchronisation is absent.
    if((reg&0x40)&&tm) { timer.start(20); tm=false; }
    //if((reg&0x40)&&tm) { timer.start(2000); tm=false; }
    if(canWr) { qDebug()<<"SYNCHR OK.";break;}
    m_sleep(5);
  }
  timer.stop();
  return(canWr);
}

void THw::boosterCycleEnd()
{
  canWr=true;
}
