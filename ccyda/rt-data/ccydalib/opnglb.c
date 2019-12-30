/*
	ss_name ==> name of SSUDA server host
	non standard use of next elements of struct relis:
		nsub ==> user,who opened the table
		nrr  ==> processor type of parent
		nrt  ==> processor type of child
		nra  ==> processor type of micro
		res1 ==> DB number
*/
#include "ccydalib.h"
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>

extern char *cel_rd();
static short fl_ini=0,l;
static char name[20];
static int s1,s_s;
static socklen_t f_N;
static fd_set read_template;
static struct timeval wait;
static struct sockaddr_in sin;
static struct sockaddr to_N,from_N;
static struct hostent *hp;

short opnglb(rel_n,use_n,ndb)
 char *rel_n,*use_n;
 short *ndb;
  {
   unsigned short j,i=0,nsu=0;
   char bffr[200];
    for(j=0;j<max_rel;j++)
      if(gtab[j].id_parent==0)
        goto ec_fr;
    return(0x8010);
ec_fr:
    if(!(*rel_n))        /* no name */
      return(0x8000);
    if(!cdb[0])
      {
       l=letini(0);
       if(l)
        return(l);
      }
    if(!my_type)
      {
       printf("My type is unknown\n");
       exit(0);
      }
    if(*use_n)
      {
       if(!set_rel(now))
         return(0x8000);
       if(!(nsu=fi_tup(0,use_n,n_tup,l_tup,to_cat[0].rel_dat_b+patc->alb,
          patc->alo)))
            return(0x8020);
      }
    if(!fl_ini)
      {
       if((s1=socket(AF_INET,SOCK_DGRAM,0)) < 0)
         return(0x8034);
/*       openlog("OPEN SOCKET S1",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE," ==> %d",s1);
       closelog();
*/
       sin.sin_family=AF_INET;
       sin.sin_port  = htons(0);
       if(bind(s1,(struct sockaddr*)&sin,sizeof(sin)))
         return(0x8035);
       if(!(hp=gethostbyname(ss_name)))
         return(0x8036);
       sin.sin_family=AF_INET;
       sin.sin_port  = htons(5995);  /* to DDBD_mem */
       memcpy(&sin.sin_addr,hp->h_addr,hp->h_length);
       fl_ini=1;   /* socket was initialized */
       if(fcntl(s1,F_SETFL,O_NONBLOCK)<0)
         return(0x8034);
      }
    ou_pac.pac_tip=1;    /* opnglb */
    memcpy(ou_pac.t_nam,rel_n,20);
    while(recvfrom(s1,bffr,200,0,&from_N,&f_N)>0)
      ;
eche0:
    s_s=sendto(s1,&ou_pac,30,0,(struct sockaddr*)&sin,
      sizeof(struct sockaddr_in));
    if(s_s <= 0 && errno==EWOULDBLOCK)
      goto eche0;
    if(s_s <= 0)
      return(0x8030);     /* error */
    memcpy(&to_N,&sin,sizeof(struct sockaddr));
one_more:
    if(i=rd_wt(&gtab[j],sizeof(struct g_main),20))
      {
       memset(&gtab[j],0,sizeof(struct g_main));
       return(i);
      }
    if(!gtab[j].id_parent) /* error from DDBD_mem */
      return(gtab[j].id_child | 0x4000);  /* error CXXX from ws_rwr */
    if(!gtab[j].tab_inf.res1)
      {
       openlog("OPNGLB",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE,"*** tab_id=%d BD=%d ***",j,gtab[j].tab_inf.res1);
       closelog();
       memset(&gtab[j],0,sizeof(struct g_main));
       goto one_more;
      }
    *ndb=gtab[j].tab_inf.res1 | 0x8000;   /* 8000 + nDB for global */
    gtab[j].tab_inf.nsub=nsu;	/* owner number */
    return(j+max_rel);   /* for global >= max_rel */
  }
rd_wt(buffer,number,sec)
  char *buffer;
  int number,sec;
   {
    int k,kk;
    if(!number)
      return(0x8001);
    wait.tv_sec= 4*sec;
    wait.tv_usec=0;
    kk=recvfrom(s1,buffer,number,0,&from_N,&f_N);
    if(kk<0 && errno==EWOULDBLOCK)
      {
eche0:
       FD_ZERO(&read_template);
       FD_SET(s1,&read_template);
       k=select(FD_SETSIZE,&read_template,(fd_set*)0,(fd_set*)0,&wait);
       if(k<0)
         return(0x8031);
       if(FD_ISSET(s1,&read_template))
         {
          kk=recvfrom(s1,buffer,number,0,&from_N,&f_N);
          if(kk<0 && errno==EWOULDBLOCK)
            goto eche0;
eche:
          if(kk<0)
            {
             openlog("rd_wt:recvfrom",LOG_CONS,LOG_USER);
             syslog(LOG_NOTICE,"kk=%d errno=%d",kk,errno);
             closelog();
             return(0x8032);
            }
          if(kk==0)
            {
             openlog("rd_wt:8070",LOG_CONS,LOG_USER);
             syslog(LOG_NOTICE,"NULL bytes on recvfrom ");
             closelog();
             return(0x8032);
            }
          return(0);       /* packet is accepted */
         }
       else
         {
          kk=recvfrom(s1,buffer,number,0,&from_N,&f_N);
          if(kk<0 && errno==EWOULDBLOCK)
            {
             openlog("rd_wt:8033",LOG_CONS,LOG_USER);
             syslog(LOG_NOTICE," sec=%d to=%d %d %d %d ",sec,
               to_N.sa_data[2]&255,to_N.sa_data[3]&255,
               to_N.sa_data[4]&255,to_N.sa_data[5]&255);
               closelog();
               return(0x8033);     /* time out */
            }
          else
            goto eche;
         }
      }
    goto eche;
   }
sd_to_cmpt(buffer,number,index) /* to EC */
 char *buffer;
 int number;
 struct sockaddr *index;
  {
   short i;
   char bffr[200]; 
   if(number > max_gb || !number)
     return(0x8001);
   while(recvfrom(s1,bffr,200,0,&from_N,&f_N)>0)
     ;
eche2:
   s_s=sendto(s1,buffer,number,0,index,
     sizeof(struct sockaddr_in));
   if(s_s <= 0 && errno==EWOULDBLOCK)
     goto eche2;
   if(s_s <= 0)
     return(0x8030);     /* error */
   memcpy(&to_N,index,sizeof(struct sockaddr));
   return(0);
  }

