#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "ccydalib.h"

int bptr;
static short flaop,safl,safs,len1;
extern char sysbas[];
char naf[50],*naf1;
extern short er;
fope(nam)
 char *nam;
  {
	if((bptr=open(nam,O_RDONLY))== -1)
	  {
	   printf("\nCAN'T OPEN %s\n",nam);
	   exit(0);
	  }
	return(bptr);
  }
w_fope(nam)
 char *nam;
  {
	if((bptr=open(nam,O_RDWR))== -1)
	  {
	   printf("\nCAN'T OPEN %s\n",nam);
	   exit(0);
	  }
	return(bptr);
  }
redbl(firef,n,buf,len)
 int firef;
 int n;
 char *buf;
 short len;
  {
   len1=len;
	n=512*(n-1);
	if(lseek(firef,n,SEEK_SET)== -1) errd(n,0);
	if(read(firef,buf,len)!=len) errd(n,2);
	return(0);
  }
wribl(firef,n,buf,len)
 int firef;
 int n;
 char *buf;
 short len;
  {
   len1=len;
   sops.sem_op=0;
   if(semop(semaph[cur_db],&sops,1) == -1 )
     perror("semop_1");
   sops.sem_op=1;
   if(semop(semaph[cur_db],&sops,1) == -1 )
     perror("semop_2");
   n=512*(n-1);
   if(lseek(firef,n,SEEK_SET)== -1) errd(n,1);
   if(write(firef,buf,len)!=len) errd(n,1);
   sops.sem_op= -1;
   if(semop(semaph[cur_db],&sops,1) == -1 )
     perror("semop_3");
	return(0);
  }
/*---------------------------------------*/
static char *er_n[]={
	"on seek",
	"on write",
	"on read"
		};
errd(nbloc,rw)
 short rw;
 int nbloc;
  {
   printf("\ndisc error %s byte=%d (or block=%d) DB=%d len=%d file=%d\n",
     er_n[rw],nbloc,(nbloc/512+1),cur_db,len1,cdb[cur_db]);
   perror("disc");
   if(rw == 1)
     {
      sops.sem_op= -1;
      if(semop(semaph[cur_db],&sops,1))
        perror("semop");
     }
   exit(0);
  }
