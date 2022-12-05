#include "serial.h"

TSerial::TSerial()
{
  qDebug()<<"Constructor of object TSerial start";
  fd=0;
  qDebug()<<"Constructor of object TSerial finish";
}

TSerial::~TSerial()
{
  qDebug()<<"Destructor of object TSerial start";
  if(fd) UART_Close();
  fd=0;
  qDebug()<<"Destructor of object TSerial finish";
}

int TSerial::UART_Init(QString devName, int speedBoud)
{
  device=devName;
  speed=speedBoud;
  errUART=0;
  if(fd) UART_Close();
  errUART=UART_Open(device.toAscii().data());
  if(errUART) errUART=UART_Set(speed, 0, 8, 1, 'n'); // speed set up in uC ATmega162
  return errUART;
}

int TSerial::UART_Open(char *port)
{
  fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
  if(fd==FALSE) {
    fprintf(stderr,"Can't open the serial port\n");
    return(FALSE);
  }
  if(fcntl(fd, F_SETFL, 0)<0) {
    fprintf(stderr,"Fcntl failed\n");
    return(FALSE);
  }
  /*if(isatty(STDIN_FILENO)==0) {
  syslog(LOG_INFO,"Standard input is not a terminal device");
    fprintf(stderr,"Standard input is not a terminal device\n");
    return(FALSE);
  }*/
  return(fd);
}

int TSerial::UART_Set(int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
  uint i;
  int speed_arr[] = {B38400, B19200, B9600, B4800, B2400, B1200, B300};
  int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300};
  struct termios options;
  if(tcgetattr(fd, &options)!=0){
    fprintf(stderr,"Setup UART error\n");
    return(FALSE);
  }

  for( i=0; i < sizeof(speed_arr)/sizeof(int); i++) {
    if(speed == name_arr[i]) {
      cfsetispeed(&options, speed_arr[i]);
      cfsetospeed(&options, speed_arr[i]);
    }
  }

  options.c_cflag |= CLOCAL;
  options.c_cflag |= CREAD;
  options.c_lflag &=~(ECHO|ICANON|ISIG); //!!!! non canonical input for ignore simbols<30

  switch(flow_ctrl) {
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
  switch (databits) {
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
      perror("Unsupported data size\n");
      return(FALSE);
  }

  switch(parity) {
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
      fprintf(stderr,"Unsupported parity\n");
      return(FALSE);
  }

//Set stop bit
  switch (stopbits) {
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
  if(tcsetattr(fd, TCSANOW, &options) != 0) {
     fprintf(stderr,"com set error!\n");
     return (FALSE);
  }

  return(TRUE);
}

int TSerial::UART_Recv(char *rcv_buf, int data_len)
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
  if(fs_sel) {
    len = read(fd, rcv_buf, data_len);
    return(len);
  }
  else {
    return(FALSE);
  }
}

int TSerial::UART_Read(char *rcv_buf, int data_len,int tout)
{
  int ret=-1;
  struct pollfd fds;
  fds.fd=fd;
  fds.events=POLLIN;
  poll(&fds,1,tout); //delay in ms
  if(fds.revents&POLLIN){
    ret=read(fd,rcv_buf,data_len);
  }
  return ret;
}

int TSerial::UART_Send(char *send_buf, int data_len)
{
  int len = 0;
  len = write(fd, send_buf, data_len);
  if(len == data_len) {
    return(len);
  }
  else {
    tcflush(fd,TCOFLUSH);
    return(FALSE);
  }
}

void TSerial::UART_Close(void)
{
  close(fd);
  fd=0;
}

void TSerial::UART_Flush(void)
{
  tcflush(fd,TCIFLUSH);
}
