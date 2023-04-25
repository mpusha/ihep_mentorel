#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <getopt.h>
#include <sys/poll.h>


#include <QDebug>
#include <QDateTime>
//#include "qextserialport.h"
//#define FALSE 0
//#define TRUE 1

int UART_Open(int fd, char *port)
{
	fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd==FALSE)
	{
		perror("Can't open the serial port\n");
		return(FALSE);
	}
	if(fcntl(fd, F_SETFL, 0)<0)
	{
		printf("Fcntl failed\n");
		return(FALSE);
	}
	else
	{
		printf("Fcntl=%d\n", fcntl(fd,F_SETFL,0));
	}
	if(isatty(STDIN_FILENO)==0)
	{
		printf("Standard input is not a terminal device\n");
		return(FALSE);
	}
	else
	{
		printf("Isatty success\n");
	}
	printf("fd->open=%d\n", fd);
	return(fd);
}

int UART_Set(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
	int i;
	int status;
	int speed_arr[] = {B38400, B19200, B9600, B4800, B2400, B1200, B300};
	int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300};
	struct termios options;
	if(tcgetattr(fd, &options)!=0)
	{
		perror("Setup UART\n");
		return(FALSE);
	}

	for( i=0; i < sizeof(speed_arr)/sizeof(int); i++)
	{
		if(speed == name_arr[i])
		{
			cfsetispeed(&options, speed_arr[i]);
			cfsetospeed(&options, speed_arr[i]);
		}
	}     

	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;

	options.c_lflag &=~(ECHO|ICANON|ISIG); //!!!! non canonical input for ignore simbols<30
	switch(flow_ctrl)
	{
		case 0:	// No flow control is used
			options.c_cflag &= ~CRTSCTS;
			break;
		case 1:	// Hardware flow control
			options.c_cflag |= CRTSCTS;
			break;
		case 2:	// Software flow control
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
	}

	options.c_cflag &= ~CSIZE;	// Mask other flag bits
	switch (databits)
	{
		case 5:
			options.c_cflag |= CS5;
			break;
		case 6:
			options.c_cflag |= CS6;
			break;
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:    
			options.c_cflag |= CS8;
			break;  
		default:   
			fprintf(stderr, "Unsupported data size\n");
			return(FALSE);
	}

	switch(parity)
	{  
		case 'n':
		case 'N':	//No Parity bit
			options.c_cflag &= ~PARENB; 
			options.c_iflag &= ~INPCK;    
			break; 
		case 'o':  
		case 'O':	//Odd parity check
			options.c_cflag |= (PARODD | PARENB); 
			options.c_iflag |= INPCK;             
			break; 
		case 'e': 
		case 'E':	//Even parity check
			options.c_cflag |= PARENB;       
			options.c_cflag &= ~PARODD;       
			options.c_iflag |= INPCK;       
			break;
		case 's':
		case 'S':	//Space
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break; 
		default:  
			fprintf(stderr, "Unsupported parity\n");   
			return(FALSE); 
	} 

	//Set stop bit
	switch (stopbits)
	{  
		case 1:   
			options.c_cflag &= ~CSTOPB; 
			break; 
		case 2:   
			options.c_cflag |= CSTOPB; 
			break;
		default:   
			fprintf(stderr,"Unsupported stop bits\n"); 
		return (FALSE);
	}

    //Modify output mode, RAW data mode
        options.c_oflag &= ~OPOST;

	//set the minimum waiting time and minimum receiving bytes before unblocking
	options.c_cc[VTIME] = 1;
	options.c_cc[VMIN] = 1;

	tcflush(fd,TCIFLUSH);
	//active the configuration
	if(tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("com set error!\n");  
		return (FALSE); 
	}

	return(TRUE);
}
int UART_Recv(int fd, char *rcv_buf, int data_len)
{
	int len,fs_sel;
	fd_set fs_read;
	struct timeval time;

	FD_ZERO(&fs_read);
	FD_SET(fd, &fs_read);

	time.tv_sec = 0;
	time.tv_usec = 1000000;

	// Using Select() function to realize the multiply channels' communication

	fs_sel = select(fd+1, &fs_read, NULL, NULL, &time);
	if(fs_sel)
	{
		len = read(fd, rcv_buf, data_len);
		return(len);
	}
	else
	{
		return(FALSE);
	}     
}
int UART_Read(int fd, char *rcv_buf, int data_len)
{
  int ret=-1;
  struct pollfd fds;
  fds.fd=fd;
  fds.events=POLLIN;
  poll(&fds,1,10000); //delay in ms
  if(fds.revents&POLLIN){
    ret=read(fd,rcv_buf,data_len);
  }
  return ret;

}

int UART_Send(int fd, char *send_buf, int data_len)
{
	int len = 0;
	len = write(fd, send_buf, data_len);
	if(len == data_len)
	{
		return(len);
	}     
	else
	{
		tcflush(fd,TCOFLUSH);
		return(FALSE);
	}
}

void UART_Close(int fd)
{
	close(fd);
}

void display_usage(void)
{
  printf("\nUse wbtfbs --cmd=command string \n"
         "             --a =address\n");
}

int main(int argc, char* argv[])
{
  int fd;
  /*QextSerialPort *port;
  port=new QextSerialPort("/dev/ttyS4",QextSerialPort::EventDriven);
  port->setBaudRate(BAUD9600);
  port->open(QIODevice::ReadWrite);
  char buf[5]={'1','2','3','4','5'};
  port->write(buf,6);
  port->flush();
  port->close();
*/
  if(argc==1) {
    display_usage();
    exit(0);
  }
  char devName[50]={"/dev/ttyS4"};
  char address[]={"00:"};
  char addressI=0;
  char command[28];
  char str[32];
  char req=0;
  const struct option longOpts[] = {
    {"help",no_argument,NULL,'h'},
    {"write",no_argument,NULL,'w'},
    {"addr",required_argument,NULL,'a'},
    {"cmd",required_argument,NULL,'c'},
    {"dev",required_argument,NULL,'d'},
    {NULL,0,NULL,0}
  };
  int opt,longIndex;
  do {
    opt=getopt_long(argc,argv,"hwa:c:d:",longOpts, &longIndex);
    if (opt!=-1)
      switch(opt)  {

            case 'a':
              addressI=atoi(optarg);
              if((addressI<0)||(addressI>100)) addressI=11;
              sprintf(address,"%02d:",addressI);
            break;
            case 'c':
              req=1; // get command
              strncpy(command,optarg,28);
            break;
            case 'w':
                qDebug()<<"cmd";
              break;
            case 'd':
              strcpy(devName,optarg);
/*              if(optarg!=NULL){

              }
              else{
                fprintf(stderr,"invalid -d argument");
                exit(1);
              }
              qDebug()<<"cmd";*/
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

  //if(addressI>10) {fprintf(stderr,"Incorrect address of device %d\n",addressI); exit(2);}
  if(!req) {fprintf(stderr,"Not operation\n"); exit(3);}
  fd= UART_Open(fd, devName);
  if(!fd){fprintf(stderr,"Can't open device %s\n",devName); exit(1);}

  UART_Set(fd, 9600, 0, 8, 1, 'n');
  char buf[32];
  int d;
  sprintf(str,"%s%s\n",address,command);
  printf("cmd=%s %d\n",str,strlen(str));


      clock_t start,end;
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
  UART_Close(fd);
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
