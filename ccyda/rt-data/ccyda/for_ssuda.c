#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SS_stop 6
#define SS_start 7
#define rep_stop 88
#define rep_start 89

struct hostent *gethostbyname();
 FILE *fopen();
void ss_wait(int,int);
void Send_1com(short,int);

void ssuda_stop()
  {
   Send_1com(SS_stop,rep_stop);
  }
void ssuda_start()
  {
   Send_1com(SS_start,rep_start);
  }
void Send_1com(short com,int reply)
   {
 int    i,k,svt;
 char buf[256];
 struct sockaddr svt_from;
 struct sockaddr_in sin;
 struct hostent *hp;
 FILE *fpt;
    memset(&sin,0,sizeof(sin));
    fpt=fopen("/usr/users/ssuda/sys/ssuda.srv","r");
    if(fpt==NULL)
      {
       printf("Can't open /usr/users/ssuda/sys/ssuda.srv\n");
       return;
      }
    if((svt=socket(AF_INET,SOCK_DGRAM,0)) < 0)
      perror("socket");
    sin.sin_family=AF_INET;
    sin.sin_port  = htons(0);
    if(bind(svt,(struct sockaddr*)&sin,sizeof(sin)))
      perror("bind");
    k=0; /* for first host */
   if(com==SS_stop)
     {
      printf("Try to stop applications\n");
      system("/usr/usera/klimenkov/u/apkill/apkill sem=0 nocmt");
      sleep(5);
      printf("Try to kill applications\n");
      system("/usr/usera/klimenkov/u/apkill/apkill kill=0 nocmt");
      sleep(5);
     }
cyc:
    if(!fgets(buf,sizeof(buf),fpt))
       goto yx;
    if(buf[0]==0 || buf[0]=='\n')
      {
yx:
       fclose(fpt);
       return;
      }
    for(i=0;i<sizeof(buf);i++)
      if(buf[i]=='\n')
        {
         buf[i]=0;
         break;
        }
    printf("Host:%s \n",buf);
    if(!(hp=gethostbyname(buf)))
      {
       perror("DSC:gethostbyname");
       return;
      }
    bcopy(hp->h_addr,&sin.sin_addr,hp->h_length);
    if(k==0)  /* ssuda server */
      {
       buf[2]=com;
       sin.sin_port  = htons(5995);  /* ddbd_mem */
       if((i=sendto(svt,buf,10,0,(struct sockaddr*)&sin,sizeof(sin))) < 0)
         perror("send");
       printf("     ddbd_mem  ");
       ss_wait(svt,reply);
       if(com==SS_start)
         sleep(3);
      }
    buf[2]=com;
    sin.sin_port  = htons(5996);     /* ws_rwr */
    if((i=sendto(svt,buf,10,0,(struct sockaddr*)&sin,sizeof(sin))) < 0)
      perror("sendto");
    printf("     ws_rwr  ");
    ss_wait(svt,reply);
    if(k==0)  /* ssuda server */
      {
       if(com==SS_start)
         sleep(3);
       buf[0]=0;
       buf[2]=com;
       sin.sin_port  = htons(5993);  /* alarmd */
       if((i=sendto(svt,buf,10,0,(struct sockaddr*)&sin,sizeof(sin))) < 0)
         perror("send");
       printf("     alarmd  ");
       ss_wait(svt,reply);
       if(com==SS_start)
         sleep(3);
       buf[3]=com;
       sin.sin_port  = htons(5998);  /* ws_lall */
       if((i=sendto(svt,buf,10,0,(struct sockaddr*)&sin,sizeof(sin))) < 0)
         perror("send");
       k=1;
       printf("     ws_lall  ");
       ss_wait(svt,reply);
      }
    goto cyc;
   }
void ss_wait(svt,reply)
  {
 fd_set read_template;
 struct timeval wait;
 int s1,k;
 char buf[25];
    s1=svt;
    wait.tv_sec=5;
    wait.tv_usec=0;
    FD_ZERO(&read_template);
    FD_SET(s1,&read_template);
    k=select(FD_SETSIZE,&read_template,(fd_set*)0,(fd_set*)0,&wait);
    if(k<0)
      {
       perror("select");
       return;
      }
    if(FD_ISSET(svt,&read_template))
      {
       if((k=read(svt,buf,25))<0)
         perror("read");
       if(buf[0]==buf[2] && buf[2]==reply)
         {
          if(reply==rep_start)
            printf(" started\n");
          else
            printf(" stopped\n");
         }
       else
         printf(" wrong answer %d = %d = %d\n",reply,buf[0],buf[2]);
       return;
      }
    printf(" no answer (Time-out)\n");
  }

