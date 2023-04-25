#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

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
			cfsetispeed(&Options, speed_arr[i]);
			cfsetospeed(&Options, speed_arr[i]);
		}
	}     

	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;

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

	time.tv_sec = 10;
	time.tv_usec = 0;

	// Using Select() function to realize the multiply channels' communication

	fs_sel = select(fd+1, &fs_read, NULL, NULL, &time);
	if(fs_sel)
	{
		len = read(fd, data, data_len);
		return(len);
	}
	else
	{
		return(FALSE);
	}     
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

int main()
{
  int fd;
  fd= UART_Open(fd, "/dev/ttyS4");
  
   UART_Set(fd, 9600, 0, 8, 1, 'n');
  char buf[5]={'1','2','3','4','5'};	
  int UART_Send(fd, buf, 6);
  UART_Close(fd);
  return 0;
}