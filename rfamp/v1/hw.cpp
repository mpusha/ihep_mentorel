
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
THw::THw(int c):QObject(),size(c)
{
  fd_spi=0;
  setingsFileName=QApplication::applicationDirPath()+"/setup.ini" ;
  IniFileRead();

}

/****************************************************************
 * destructor
 * Unexport pins and free spi1
 ****************************************************************/
THw::~THw()
{
  pins_unexport();
  if(fd_spi>0) close(fd_spi);
}
//------------------------------------------------- Common functions ---------------------------------
/****************************************************************
 * Read adc DAC settings
 * ADC1Shift middle of ADC1 ~2048 code
 * ADC2Shift middle of ADC2 ~2048 code
 * CoeffAdc MaxI(in Amps)/2048 for I=1000A => 0.0488
 * CoeffDac 2048/MaxI(in Amps) for I=1000A =>2.048 (2.05 for round)
 ****************************************************************/
void THw::IniFileRead(void)
{
  //shADC1=2033,shADC2=2042;
  QSettings setings(setingsFileName,QSettings::IniFormat) ;
  adc1Shift=setings.value("Adc1Shift",2033).toInt();
  adc2Shift=setings.value("Adc2Shift",2042).toInt();
  dac1Shift=setings.value("Dac1Shift",3).toInt();
  dac2Shift=setings.value("Dac2Shift",-4).toInt();
  coeffAdc=setings.value("CoeffAdc",1000.0/2048.0).toFloat(); // (5/2048)*(I/5)=>I/2048 I - max current in Amps 5-max 5Volts ADC 12bits
  coeffDac=setings.value("CoeffDac",2048/1000).toFloat(); // (2048/5)*(5/I)=>2048/I  I-max current in Amps
}

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
// programm FPGA
 // if(gpio_export(PIN_HOST_DCLK)) return 1;
  //if(gpio_export(PIN_FPGA_CONFDONE)) return 1;
  //if(gpio_export(PIN_HOST_NCONFIG)) return 1;
 // if(gpio_export(PIN_HOST_DATA)) return 1;
  //if(gpio_export(PIN_FPGA_NSTAT)) return 1;

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

    //gpio_unexport(PIN_HOST_DCLK);
  //  gpio_unexport(PIN_FPGA_CONFDONE);
  //  gpio_unexport(PIN_HOST_NCONFIG);
   // gpio_unexport(PIN_HOST_DATA);
  //  gpio_unexport(PIN_FPGA_NSTAT);


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

  //if(gpio_set_dir(PIN_HOST_DCLK,OUTPUT_PIN)) return 1;
  //if(gpio_set_dir(PIN_FPGA_CONFDONE,INPUT_PIN)) return 1;
 // if(gpio_set_dir(PIN_HOST_NCONFIG,OUTPUT_PIN)) return 1;
  //if(gpio_set_dir(PIN_HOST_DATA,OUTPUT_PIN)) return 1;
 // if(gpio_set_dir(PIN_FPGA_NSTAT,INPUT_PIN)) return 1;

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

/****************************************************************
 * Load FPGA functiom
 ****************************************************************/
// 1    - DCLK      P1.39 GPIO2_12 HOST
// 3    - CONF_DONE P1.40 GPIO2_13 FPGA
// 5    - nCONFIG    P1.42 GPIO2_11 HOST
// 7    - Data0     P1.44 GPIO2_9  HOST
// 6    - nCE
// 8    - nCSO
// 9    - ASD
// Abs  - nSTATUS   P1.41 GPIO2_10 FPGA
//
// 2,10  - gnd
//---      --------------------------------------- nCONFIG (host) 175
//  I      I
//  I------I
//-------      ---------------------------------- nSTATUS FPGA 119
//      I      I
//      I------I
//-------                                     --- CONF_DONE (FPGA) 116
//      I                                    I
//      I---------------------~~~~-----------I
//
//             ----  ----  ----  ----  ----  ----  ---- DCLK (host) 117
//             I  I  I  I  I  I  I  I  I  I  I  I  I
//-------------I  I--I  I--I  I--I  I--I  I--I  I--I
//            ---------------------------------------
// ----------                                        Data0 LSB is first (host) 118
//            ---------------------------------------clock data on rise DCLK
int THw::configureFPGA(char * fName)
{
/*
  unsigned char *padconf=NULL,*pin=NULL;
  unsigned char *gpio_addr = NULL;
  unsigned char *gpio_oe_addr = NULL;
  unsigned char *gpio_setdataout_addr = NULL;
  unsigned char *gpio_cleardataout_addr = NULL;
  unsigned long reg;
  int mfd,rb,i;
int fd = open("/dev/mem", O_RDWR|MS_SYNC);

  printf("Mapping %X - %X (size: %X)\n", GPIO2_START_ADDR,GPIO2_END_ADDR, GPIO2_SIZE);

  padconf=(unsigned char*) mmap(NULL,0x2000,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x44E10000);
  if(padconf == MAP_FAILED) {
      printf("Unable to map GPIO\n");
      exit(1);
  }

  printf("GPIO2 configuration: %X\n", *(unsigned long*) (padconf+0x99C));
  gpio_addr =  (unsigned char*) mmap(0, GPIO2_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, fd, GPIO2_START_ADDR);

  gpio_oe_addr = (unsigned char *) gpio_addr + GPIO_OE;
  gpio_setdataout_addr = (unsigned char *)gpio_addr + GPIO_SETDATAOUT;
  gpio_cleardataout_addr = (unsigned char *)gpio_addr + GPIO_CLEARDATAOUT;

  if(gpio_addr == MAP_FAILED) {
      printf("Unable to map GPIO\n");
      exit(1);
  }
  printf("GPIO mapped to %p\n", gpio_addr);
  printf("GPIO OE mapped to %p\n", gpio_oe_addr);
  printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
  printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);

  reg = *(unsigned long*) gpio_oe_addr;
  printf("GPIO2 configuration: %X\n", reg);
  reg = reg & (0xFFFFFFFF - PIN_DCLK)&(0xFFFFFFFF - PIN_DATA0) ;
  *(unsigned long*) gpio_oe_addr = reg;
  printf("GPIO2 configuration: %X\n", reg);
printf("GPIO2 configuration: %X\n", *(unsigned long*) gpio_oe_addr);
  printf("Start toggling PIN \n");



  unsigned long j,DataLoad;
  unsigned char dataForLoad[512];

 // printf("%s\n", *((unsigned char *) &x) == 0 ? "big-endian"\ : "little-endian");
  mfd=open(fName,O_RDONLY);
  if (mfd<0) {
    perror("Timer FPGA file open. File not found btimer.rbf");
    syslog(LOG_ERR,"Timer FPGA file open error. File not found %s",fName);
    return(1);
  }
/*printf("before clr: %X\n", *(unsigned long*) gpio_cleardataout_addr);
printf("before set: %X\n", *(unsigned long*) gpio_setdataout_addr);
while(1){
  *(unsigned long*)gpio_cleardataout_addr = (unsigned long) PIN_DATA0;
  //printf("CLR1: %X\n", *(unsigned long*) gpio_cleardataout_addr);
  //printf("set1: %X\n", *(unsigned long*) gpio_setdataout_addr);
m_sleep(1);
  *(unsigned long*)gpio_setdataout_addr = (unsigned long) PIN_DATA0;
  //printf("CLR2: %X\n", *(unsigned long*) gpio_cleardataout_addr);
  //printf("set2: %X\n", *(unsigned long*) gpio_setdataout_addr);
}



  // form nCONFIG pulse
  *(unsigned long*)gpio_cleardataout_addr = (unsigned long) PIN_DCLK; //CLK Off
  //gpio_set_value(PIN_HOST_DCLK,LOW);//CLK Off

  //one bit set/clear ~0,375 us for tam3517 100us uSOMIQ from i/o 0.3 us im memory map mode

  for(i=0;i<10;i++)
      gpio_set_value(PIN_HOST_NCONFIG,HIGH); // nConfig On and wait ~4us
  for(i=0;i<10;i++)gpio_set_value(PIN_HOST_NCONFIG,LOW); // nConfig Off and wait ~60us
  for(i=0;i<10;i++) gpio_set_value(PIN_HOST_NCONFIG,HIGH);   // nConfig On and wait ~40us
  unsigned int val=0;
  while(val==0) gpio_get_value(PIN_FPGA_NSTAT,&val); // wait nSTATUS going to 1

  while((rb=read(mfd,dataForLoad,512))>0){
    for(i=0;i<rb;i++) {
      DataLoad=dataForLoad[i];
      for(j=0;j<8;j++) {
        if(DataLoad&0x01)
          *(unsigned long*)gpio_setdataout_addr= (unsigned long) PIN_DATA0;
        //  gpio_set_value(PIN_HOST_DATA,HIGH); // data0 On
        else
          *(unsigned long*)gpio_cleardataout_addr = (unsigned long) PIN_DATA0;
          //gpio_set_value(PIN_HOST_DATA,LOW);  // data0 Off
        DataLoad>>=1;
        *(unsigned long*)gpio_setdataout_addr= (unsigned long) PIN_DCLK;    //DCLK On
        *(unsigned long*)gpio_cleardataout_addr = (unsigned long) PIN_DCLK; //DCLK Off
        //gpio_set_value(PIN_HOST_DCLK,HIGH); //DCLK On
       // gpio_set_value(PIN_HOST_DCLK,LOW);//DCLK Off
      }
    }
  }

  i=0;
  close(mfd);
  val=0;
  while(val&&(i++<TIMEOUT_ERR))  {
     gpio_get_value(PIN_FPGA_NSTAT,&val);
     *gpio_setdataout_addr= (unsigned long) PIN_DCLK;    //DCLK On
     *gpio_cleardataout_addr = (unsigned long) PIN_DCLK; //DCLK Off
     //gpio_set_value(PIN_HOST_DCLK,HIGH);//DCLK On
    // gpio_set_value(PIN_HOST_DCLK,LOW);//DCLK Off
  }

  munmap(gpio_addr,GPIO2_SIZE);
  close(fd);
  if(i>=TIMEOUT_ERR) return(2);
  */
  return(0);
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
    u_int32_t wd[size], rd[size],wd_extra[size];
  int i=0,j=0;

  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {qDebug()<<"Can't read DAC csv file"<<fname; return(1); }

  QTextStream in(&file);
  QString line=in.readLine();
  QStringList list1;
  while (!in.atEnd()) {
    line = in.readLine();
    list1=line.split(",");
    wd[i++]=(int)(list1.at(1).toFloat()*coeffDac);
    wd[i++]=(int)(list1.at(2).toFloat()*coeffDac);
    wd_extra[j++]=(int)(list1.at(3).toFloat()*coeffDac);
    wd_extra[j++]=(int)(list1.at(4).toFloat()*coeffDac);
    if(i/2>size) break;
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
  SPI_writeReadData(wd_extra,rd,size,WR_DACADC_DATA);
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

void THw::usr_waitCanWrite(void)
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
    m_sleep(1);
  }
  timer.stop();
}
void THw::boosterCycleEnd()
{
  canWr=true;
}
