#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int bptr= -1,bptr0= -1,bptrcf= -1;
static int flaop,safl,safs;
extern char sysbas[];
extern int errno;
char naf[50],*naf1;
fop(nam)
 char *nam;
  {
 	   if((bptr=open(nam,2))== -1)
	     {
	      printf("\nCAN'T OPEN %s\n",nam);
	      exit(0);
 	     }
  }
creshr(nam)
 char *nam;
  {
	bptr=creat(nam,0666);
	close(bptr);
	fop(nam);
	sanam(nam);
  }
opnshr(nam)
 char *nam;
  {
	fop(nam);
	sanam(nam);
	return(0);
  }
sanam(nam)
 char *nam;
  {
   short i;
	for(i=0;i<50;i++)
	  naf[i]=nam[i];		/* save file name */
  }
redshr(n,buf,len)
 int n,len;
 char *buf;
  {
int x;
	n=512*(n-1);
	if(lseek(bptr,n,SEEK_SET)== -1) errd(n,0);
	x=read(bptr,buf,len);
/*printf("len=%d x=%d n=%d bptr=%d buf=%x\n",len,x,n,bptr,buf);
*/        if(x!=len)
          errd(n,2);
	return(0);
  }
wrishr(n,buf,len)
 int n,len;
 char *buf;
  {
	n=512*(n-1);
	if(lseek(bptr,n,SEEK_SET)== -1)errd(n,0);
	if(write(bptr,buf,len)!=len) errd(n,1);
	return(0);
  }
cpshr(int b1,short o1,int b2,short o2,int len)
  {
   int from,t,l,k;
   char *ref;
   from=512*(b1-1)+o1; /* start byte of hole */
   t=512*(b2-1)+o2;    /* start of old free */
   l=t-from;           /* lenght of data to move */
   t=from+len;         /* where to move */
   if(l>len)
     k=l;
   else
     k=len;
   if((ref=malloc(k))==NULL)
     {
      printf("Error malloc\n");
      exit(-1);
     }
   if(lseek(bptr,from,SEEK_SET)== -1)errd(from,0);
   if(read(bptr,ref,l)!=l) errd(from,2);
   if(lseek(bptr,t,SEEK_SET)== -1)errd(t,0);
   if(write(bptr,ref,l)!=l) errd(t,1);
   memset(ref,0,k);
   if(lseek(bptr,from,SEEK_SET)== -1)errd(from,0);
   if(write(bptr,ref,len)!=len) errd(from,1);
   free(ref);
  }
closhr()
  {
	close(bptr);
  }
tosyba()
  {
	if(bptr==bptrcf)return;
	bptr0=bptr;
	safl=flaop;
	flaop=safs;
	bptr=bptrcf;
	naf1=sysbas;
  }
resyba()
  {
	if(bptr==bptr0)return;
	bptr=bptr0;
	safs=flaop;
	flaop=safl;
	naf1=naf;
  }
/*---------------------------------------*/
static char *er_n[]={
	"on seek",
	"on write",
	"on read"
		};
errd(n,rw)
 short rw;
 int n;
  {
	perror("disc err");
	printf("disc error %s byte number=%d (or block=%d) file descr.=%d\n",
	er_n[rw],n,n/512+1,bptr);
	closhr();
	exit(-1);
  }
