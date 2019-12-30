#include "typdef.h"
#include <stdio.h>
#include <stdlib.h>
extern short nddbd,ncoms,nhoms,er,error1,ncom_li,nos_li,nbss;
extern FILE *fcom,*fsave;
extern int bufk[19];
static char *work;
extern struct sddbd *pdd2;
static struct comput *toco;
static struct relis *p1;
static char *st[]={
 "READ ONLY",
 "READ/WRITE"
  } ;
static char *tt[]={
 "MATRIX",
 "TABLE",
 "MATRIX WITH REF.",
 "TABLE WITH REF."
  } ;
rlprint(buf,num)
 char *buf;
 short unsigned num;
  {
   short n,i,j;
	work=buf+((sizeof(struct relis)+3)>>2<<2);
	if((n=nnam(work,(short)20,work+20,(short)1,num))==0)
	  {
	   fprintf(fsave,"\nRELATION NAME ISN'T FOUND\n");
	   if(!fcom) printf("\nRELATION NAME ISN'T FOUND\n");
	   exit(0);
	  }
	p1=(P3)buf;
	if(rlread(p1,num,work+20)==0)exit(0);
	fprintf(fsave,"\n\n   %s - %s",work,tt[((short)p1->rtype&7)-1]);
	printf("   %s - %s",work,tt[((short)p1->rtype&7)-1]);
	if(p1->rtype&32)
	  {
	   fprintf(fsave," - GLOBAL");
	   if(!fcom) printf(" - GLOBAL");
	  }
	else
	  {
	   fprintf(fsave," - LOCAL");
	   if(!fcom) printf(" - LOCAL");
	  }
	fprintf(fsave,"\n   ATTRIBUTES = %d",p1->nat);
	fprintf(fsave,"\n   TUPLES     = %d",p1->ntup);
	fprintf(fsave,"\n   PLANES     = %d",p1->third);
	fprintf(fsave,"\nLEN.OF TUPLE  = %d",p1->lotup);
	fprintf(fsave,"\nOWNER=%d   SUBSYSTEM=%d   STATE=%s",
	  (int)p1->nowner&255,(int)p1->nsub&255,st[(int)p1->rstate&1]);
	if(!fcom)
	  {
	   printf("\nATTRIBUTES = %d",p1->nat);
	   printf("  TUPLES = %d",p1->ntup);
	   printf("  PLANES = %d",p1->third);
	   printf("  LEN.OF TUPLE = %d",p1->lotup);
	   printf("\nOWNER=%d   SUBSYSTEM=%d   STATE=%s",
	     (int)p1->nowner&255,(int)p1->nsub&255,st[(int)p1->rstate&1]);
	  }
	if(p1->rtype&0xf0)
	  {
	   fprintf(fsave,"\n\n  DISTRIBUTION:");
	   if(!fcom) printf("\n  DISTRIBUTION:");
	   tosyba();
	   redshr(1,bufk,sizeof(bufk));
	   fprintf(fsave,"\n           PARENT COMPUTER");
	   if(!fcom) printf("\n           PARENT COMPUTER");
	   if(p1->rtype&64)
	     {
	      fprintf(fsave," + RAM");
	      if(!fcom) printf(" + RAM");
	     }
	   i=1;
	   pdd2=(PDD)(work+20);
           while(rwstr(pdd2+sizeof(struct sddbd),(short)0,i++,(short)1,nddbd,(short)1,pdd2))
	     {
	      if(n==pdd2->lena)
	        {
		 for(j=0;j<n;j++)
		   if(work[j]!=pdd2->rena[j]) goto nexs;
		 prcoho(pdd2->n_parent);
	         break;
		}
nexs:
	      continue;
	     }
	   if(p1->rtype&16)
	     {
	      fprintf(fsave,"\n           CHILD ");
	      if(!fcom) printf("\n           CHILD ");
	      prcoho(pdd2->n_child);	  
	     }
	   if(p1->rtype&128)
	     {
	      fprintf(fsave,"\n           MICRO  ");
	      if(!fcom) printf("\n           MICRO  ");
	      prcoho(pdd2->n_micro);
	     }
	   resyba();
	   redshr(1,bufk,sizeof(bufk));
	  }
	if(!fcom)printf("\n\n");
  }
prcoho(co)
 short co; 	/* ref.to COMPUTERS */
  {
     char *pp;
	if(nhoms && ncoms)
	  {
	   toco=(TOCO)work;
	   pp=work+sizeof(struct comput);
	   rwstr(work+100,0,co,1,ncoms,1,work);
	   nnam(pp,100,pp+100,4,toco->ncompu);
	   fprintf(fsave,"\n ---> %s ",pp);
	   if(!fcom) printf("\n ---> %s ",pp);
	   pp[rwstr(pp+100,0,toco->type_pro&255,1,ncom_li,1,pp)]=0;
	   fprintf(fsave,"( %s with ",pp);
	   if(!fcom) printf("( %s with ",pp);
	   pp[rwstr(pp+100,0,toco->nos&127,1,nos_li,1,pp)]=0;
	   fprintf(fsave," %s ) ",pp);
	   if(!fcom) printf(" %s ) ",pp);
	   pp[rwstr(pp+100,0,toco->nhom&255,1,nhoms,1,pp)]=0;
	   fprintf(fsave,"AT BUILDING %s \n",pp);
	   if(!fcom)
	     printf("AT BUILDING %s \n",pp);
	   if(toco->bus_c)
             {
	      nnam(pp,100,pp+100,4,toco->nhost);
	      fprintf(fsave,"Host %s BC=%d RT=%d\n",pp,toco->bus_c,toco->rem_t);
	      if(!fcom)
	        printf("Host %s BC=%d RT=%d\n",pp,toco->bus_c,toco->rem_t);
             }
	  }
  }

