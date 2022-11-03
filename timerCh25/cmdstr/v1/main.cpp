#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <getopt.h>



#include <QDebug>
#include "timer.h"

//#include "qextserialport.h"
//#define FALSE 0
//#define TRUE 1



void display_usage(void)
{
  printf("\nUse ./cmdCtrlT25 -c 'command' or --cmd='command' : execute command on hardware (HW) \n"
         "                 format of command AA:CMD where AA-address in range 1-99, CMD-command\n"
         "                 -a address or --addr=address : setup address of HW in range (1-99)\n"
         "                 -d device or --dev=device: setup UART device for ex. /dev/ttyS4\n"
         "                 -f filename or --file=filename : file name for write/read operation\n"
         "                 -w or --write : write file into HW with number address\n"
         "                 -r or --read : read file from HW with number address into file\n"
         "                 -u or --update : update data from register into HW\n"
         "                 -h or --help : this help\n");
}

int main(int argc, char* argv[])
{
  if(argc==1) {
    display_usage();
    exit(0);
  }
  char devName[50]={"/dev/ttyS4"};
  char filenameWr[255];
  char addressI=-1;
  char command[28];
  char req=0;
  const struct option longOpts[] = {
    {"help",no_argument,NULL,'h'},
    {"write",no_argument,NULL,'w'},
    {"read",no_argument,NULL,'r'},
    {"update",no_argument,NULL,'u'},
    {"addr",required_argument,NULL,'a'},
    {"file",required_argument,NULL,'f'},
    {"cmd",required_argument,NULL,'c'},
    {"dev",required_argument,NULL,'d'},
    {NULL,0,NULL,0}
  };
  int opt,longIndex;
  do {
    opt=getopt_long(argc,argv,"hwrua:c:d:f:",longOpts, &longIndex);
    if (opt!=-1)
      switch(opt)  {
      case 'a':
        addressI=atoi(optarg);
      break;
      case 'c':
       req=1; // get command
       strncpy(command,optarg,28);
      break;
      case 'f':
        strcpy(filenameWr,optarg);
      break;
      case 'w':
        req=2;
      break;
      case 'r':
        req=3;
      break;
      case 'd':
        strcpy(devName,optarg);
      break;
      case 'u':
        req=4;
      break;
      case 'h':
      case '?':
        display_usage();
        exit(0);
      break;
      default:
      break;
    }
   // longIndex=-1;
  }while(opt!=-1);

  if(!req) {fprintf(stderr,"Not operation\n"); exit(3);}

  TTimer *timer;
  int retcode;
  timer=new TTimer(devName);
  if((retcode=timer->initHW())) {
    qDebug()<<"Error code"<<retcode<<timer->getQStrErr(retcode);
    delete timer;
    exit(1);
  }
  if((retcode=timer->setAddress(addressI))){
    qDebug()<<"Error code"<<retcode<<timer->getQStrErr(retcode);
    delete timer; exit(1);
  }
  uint64_t t1,t2;
  t1=QDateTime::currentMSecsSinceEpoch();
  switch(req){
  case 1:
    qDebug()<<timer->sendReadRawCmd(command);
  break;
  case 2:
    if((retcode=timer->writeFile(filenameWr))){
      qDebug()<<"Error code"<<retcode<<timer->getQStrErr(retcode);
    }
  break;
  case 3:
    if((retcode=timer->readFile(filenameWr))){
      qDebug()<<"Error code"<<retcode<<timer->getQStrErr(retcode);
    }
  break;
  case 4:
    if((retcode=timer->wrHW())){
      qDebug()<<"Error code"<<retcode<<timer->getQStrErr(retcode);
    }
  break;
  }
  t2=QDateTime::currentMSecsSinceEpoch();
  printf("TIME spend %6.1lf ms\n",(double)t2-(double)t1);
  delete timer;
  return(0);


  /*qDebug()<<timer->sendReadRawCmd(command);
  uint64_t t1,t2;
  if(timer->initHW()) {
    qDebug()<<"Error code"<<retcode<<timer->getQStrErr(retcode);
    delete timer; exit(1);
  }
  t1=QDateTime::currentMSecsSinceEpoch();
  for(int z=0;z<10;z++)
  for(int k=1;k<17;k++){
    if((retcode=timer->writeRegTime(k,40000))){
      qDebug()<<k<<"Error code"<<retcode<<timer->getQStrErr();
    }
    if((retcode=timer->writeRegWidth(k,40))){
      qDebug()<<k<<"Error code"<<retcode<<timer->getQStrErr();
    }
    qDebug()<<k<<"code"<<retcode;
  }
  t2=QDateTime::currentMSecsSinceEpoch();
  printf("TIME spend %lf\n",(double)t2-(double)t1);
*/
/*  clock_t start,end;
  start=clock();
  uint64_t t1,t2;
  t1=QDateTime::currentMSecsSinceEpoch();

  UART_Send(fd, str, strlen(str)+1);
  //exit(0);
  char ch=1;
  char retstr[32];
  int i=0;
  bzero(retstr,32);
  bzero(buf,32);
  char bf=0;

  while(ch!=0){
    d=UART_Read(fd,&bf,1);
    //printf("RD %d %d\n",i,d);
    if(d<1) break;
    //while(buf[i])
    if(!bf) break;
    {
      retstr[i]=bf;
      i++;
      if(i>32) break;
    }
    //ch=0;
  }
  end=clock();
  printf("ret=%s\n",retstr);
  t2=QDateTime::currentMSecsSinceEpoch();
  printf("TOUT %lf %ld \n",(double)(end-start)/CLOCKS_PER_SEC,t2-t1);
  */
  //printf("ret=%s\n",retstr);
/*  clock_t start,end;
  uint64_t t1,t2;
  while(1) {
  start=clock();
  t1=QDateTime::currentMSecsSinceEpoch();
  d=UART_Recv(fd,buf,1);
  t2=QDateTime::currentMSecsSinceEpoch();
  end=clock();
  printf("TOUT %d %lf ",d,(double)(end-start)/CLOCKS_PER_SEC);
  printf("%lf %ld\n",(double)(clock()-end)/CLOCKS_PER_SEC,t2-t1);
  }
  //UART_Send(fd, buf, 6);
*/
 // UART_Close(fd);
  /*
  while(ch!=0){
    i=0;
    bzero(buf,32);
    d=UART_Read(fd,buf,32);
    //d=j;
    //printf("RD %d %d\n",j,d);
    if(d<1) break;
    if(!buf[i]) break;
    while(1)
    {
    //  printf("%d %d %d %d \n",i,j,buf[i],retstr[j]);
      if(i>=d)  break;
      if(!buf[i]) { ch=0; break;}

      retstr[j]=buf[i];
      j++;
      i++;
    }
  }
*/
  return 0;
}
