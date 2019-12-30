#include <stdio.h>
#include <stdlib.h>
short error1;
extern short zz;
static short m,i=0;
static char zzz=0,fl[2],c,wor,buf[82];
static FILE *z1;
cstmer(n)
  short n;
   {
    char c;
	if(zzz)
	  {
	   zz=fl[n];
	   return;
	  }
	if(!(z1=fopen("/usr/users/ssuda/sys/bmpv.cnf","r")))
	  {
	   printf("FILE bmpv.cnf NOT EXIST\n");
	   goto no1;
	  }
	m=1;
	while((c=fgetc(z1))!=EOF)
	  {
	   if(m&1)
	     {
	      wor=fgetc(z1);
	      buf[i]= (c<<4)|wor;
	     }
	   else
	     buf[i]= ~c;
	   if(buf[i]=='\n')break;
	   ++i;
	   ++m;
	  }
	buf[i]=0;
	fclose(z1);
	if(!(z1=fopen(buf,"r")))
	  {
	   printf("Significant file is absent\n");
no1:
	   exit(0);
	  }
	fl[0]=fgetc(z1);
	fl[1]=fgetc(z1);
	zz=fl[n];
	zzz=1;
	fclose(z1);
	return;
   }

