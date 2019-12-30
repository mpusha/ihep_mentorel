#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/mman.h>

#include "fpga.h"


int confFPGA(char * fName)
{
  int mfd,rb,i;
  u_int32_t *padconf=NULL;
  unsigned char *pGpIo2=NULL,*pGpIo3=NULL;
  //unsigned long regTmp;
  unsigned long j,DataLoad;
  unsigned char dataForLoad[512];
  
 // printf("%s\n", *((unsigned char *) &x) == 0 ? "big-endian"\ : "little-endian");

  mfd=open("/dev/mem",O_RDWR);
  if (mfd<0) {
    perror("mem open");
    return(LOADFPGA_ERR_MEMMAP);
  }

  padconf=  mmap(NULL,0x2000,PROT_READ|PROT_WRITE,MAP_SHARED,mfd,0x44E10000);
  if (padconf==MAP_FAILED) {
    perror("mmap padconf");
    return(LOADFPGA_ERR_MEMMAP);
  }
  pGpIo2= mmap(NULL,0x1000,PROT_READ|PROT_WRITE,MAP_SHARED,mfd,0x481ac000);
  if (pGpIo2==MAP_FAILED) {
    perror("mmap gpio2");
    return(LOADFPGA_ERR_MEMMAP);
  }
  pGpIo3=mmap(NULL,0x1000,PROT_READ|PROT_WRITE,MAP_SHARED,mfd,0x481ae000);
  if (pGpIo3==MAP_FAILED) {
    perror("mmap gpio3");
    return(LOADFPGA_ERR_MEMMAP);
  }


// printf("Load FPGA %x\n",READREG32(padconf+0x8b8));
//printf("Load FPGA %x\n",READREG32(pGpIo3+0x134));
  WRITEREG32(padconf+0x8b8,0x7);  // set mode 7 on gpio2_12 (lcddata6) clk          out
  WRITEREG32(padconf+0x8bc,0x27); // set mode 7 on gpio2_13 (lcddata7) conf_done    in
  WRITEREG32(padconf+0x8b4,0x7);  // set mode 7 on gpio2_11 (lcddata5) n_config     out
  WRITEREG32(padconf+0x8ac,0x7);  // set mode 7 on gpio2_9 (lcddata3) data0         out

  WRITEREG32(padconf+0x99c,0x27); // set mode 7 on gpio3_17 (mcasp0_ahclkr) nstatus in

  GPIO_SETDIRECTION(pGpIo2,(PIN_DCLK | PIN_DATA0 | PIN_NCONFIG)); // on output

  //GPIO_SETDIRECTION(pGpIo3,(u_int32_t)0); // on output
  //WRITEREG32(pGpIo3+0x134,0xFFFFFFFF);
  printf("Load FPGA \n");
  close(mfd);
  mfd=open(fName,O_RDONLY);
  if (mfd<0) {
    perror("FPGA config file open error. File not found");
    syslog(LOG_ERR,"FPGA config file open error. File not found %s",fName);
    return(LOADFPGA_ERR_FNAME);
  }


  // form nCONFIG pulse
    GPIO_CLEARBIT32(pGpIo2,PIN_DCLK); //CLK Off
    //one bit set/clear ~0,375 us for tam3517
    for(i=0;i<11;i++) GPIO_SETBIT32(pGpIo2,PIN_NCONFIG);  // nConfig On and wait ~4us
    for(i=0;i<160;i++) GPIO_CLEARBIT32(pGpIo2,PIN_NCONFIG); // nConfig Off and wait ~60us
    for(i=0;i<107;i++) GPIO_SETBIT32(pGpIo2,PIN_NCONFIG);   // nConfig On and wait ~40us
    while((GPIO_READDATA32(pGpIo3)&PIN_NSTAT)==0);//printf("%x\n",GPIO_READDATA32(pGpIo3)); // wait nSTATUS going to 1

    while((rb=read(mfd,dataForLoad,512))>0){
      for(i=0;i<rb;i++) {
        DataLoad=dataForLoad[i];
        for(j=0;j<8;j++) {
          if(DataLoad&0x01)
            GPIO_SETBIT32(pGpIo2,PIN_DATA0); // data0 On
          else
            GPIO_CLEARBIT32(pGpIo2,PIN_DATA0); // data0 Off
          DataLoad>>=1;
          GPIO_SETBIT32(pGpIo2,PIN_DCLK); //DCLK On
          GPIO_CLEARBIT32(pGpIo2,PIN_DCLK); //DCLK Off
        }
      }
    }
    i=0;
    close(mfd);
    while(((GPIO_READDATA32(pGpIo2)&PIN_CONFDONE)==0)&&(i++<TIMEOUT_ERR))
    {
       GPIO_SETBIT32(pGpIo2,PIN_DCLK); //DCLK On
       GPIO_CLEARBIT32(pGpIo2,PIN_DCLK); //DCLK Off
    }
    munmap(pGpIo2,0x1000);
    munmap(pGpIo3,0x1000);
    munmap(padconf,0x2000);
    if(i>=TIMEOUT_ERR) return(LOADFPGA_ERR_TIMEOUT);
    return(LOADFPGA_OK);
}
