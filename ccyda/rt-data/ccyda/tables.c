#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "typdef.h"
#include <sys/types.h>

#define r_n 1 /* rel.name */
#define a_n 2 /* atr.name */
#define t_n 3 /* tup.name */
#define t_f 4 /* text file */
#define r_l 5 /* rel.list */
#define a_l 6 /* atr.list */
#define r_d 7 /* datas */
#define t_l 8 /* tup.list */

#define true 1
#define false 0

#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 1600

extern int bptr,bufk[catdli];
extern struct sddbd pdd1,*pdd2;
extern char buf[bufdli],base_name[],*ref12[],*ref13[];
extern short nsubs,nddbd,ncoms,nbss,glnogl,ctpd[],base_number,computer;
extern FILE *fsave,*fcom;

fiow(j)
 short j;
  {
   short n=1;
	tosyba();
	redshr(1,bufk,sizeof(bufk));
	while(rwstr(buf+100,0,n++,1,j,1,buf+1000))
	  if(strcmp(buf,buf+1000)==0) goto ex;
	n=1;
ex:
	resyba();
	if(bptr != -1) redshr(1,bufk,sizeof(bufk));
	return(n-1);
  }

fidd(key)	/* key=0 - find and delete */
 short key;	/* >0 - find, <0 - find free and write */
  {
   short n=2,i;	/* DDBD find from second tuple */
   tosyba();
   pdd2=(PDD)buf;
   redshr(1,bufk,sizeof(bufk));
   while(rwstr(buf+100,(char)0,n++,(short)1,nddbd,(short)1,buf))
     {
      if(key<0) /* find free and write */
        {
         if(*(short *)buf!=0) goto nex;
         rwstr(buf,(char)1,n-1,(short)1,nddbd,(short)1,&pdd1);
         wrdt();
         goto ex;
        }
      if(pdd1.lena!=pdd2->lena || pdd1.ncrc!=pdd2->ncrc) goto nex;
      for(i=0;i<20;i++)
        if(pdd1.rena[i]!=pdd2->rena[i]) goto nex;
      if(!key) /* find and delete */
        {
         key=n-1;
         while(rwstr(buf+100,(char)0,key+1,(short)1,nddbd,(short)1,pdd2))
         rwstr(buf+100,(char)1,key++,(short)1,nddbd,(short)1,pdd2);
         *(short *)buf=0;
         rwstr(buf+100,(char)1,key,(short)1,nddbd,(short)1,pdd2);	/* clear last */
         wrdt();
        }
      goto ex;		/* DDBD tuple is found */
nex:
      continue;
     }
   if(key<0)
     {
      key=1;
      if(tupl_a(nddbd,(short)1,buf,0,&key))
        {
         rwstr(buf,(char)1,n-1,(short)1,nddbd,(short)1,&pdd1);
         wrdt();
         goto ex;
        }
      else
        {
         fprintf(fsave,"\nCAN'T EXTEND 'DDBD'\n");
         if(!fcom) printf("\nCAN'T EXTEND 'DDBD'\n");
        }
     }
   n=1;
ex:
   resyba();
   redshr(1,bufk,sizeof(bufk));
   return(n-1);
  }
wrdt()	/* write first tuple of DDBD */
  {	    /* pdd2 is setted */
	pdd2->lena=pdd2->ncrc=255;
	time(pdd2->rena);
   	rwstr(buf+100,1,1,1,nddbd,1,pdd2); /* new date */
  }

codial(p3,numr,fl_1)
 struct relis *p3;
 short numr,
       fl_1;	/* =0 --> no file read/write for DSD */
                /* =1 --> include global */
                /* =2 --> edit global */
  {
    short ii,
          dd_N;    /* num in DDBD from fidd */
	if(oumenu(2,ref12,1)==1)
  	  {
	   if(fl_1)
	     if((dd_N=fidd(1)))
	       {
                if(fl_1 == 2) /* edit net address */
                  {
                   memcpy(&pdd1,pdd2,sizeof(pdd1));
                   goto edit;
                  }
	        fprintf(fsave,"\nTHERE IS SUCH GLOBAL RELATION");
	        if(!fcom) printf("\nTHERE IS SUCH GLOBAL RELATION");
	        n_del(numr,buf,(short)r_n);
	        return(0);
	       }
           pdd1.n_parent=0;
	   ficom(p3,-1,fl_1);	/* fill pdd1.dbti & pdd1.n_parent*/
edit:
           p3->rtype &= 7;
	   p3->rtype |= 32;	/* global */
           pdd1.n_child=pdd1.n_micro=0;
	   if((ii=oumenu(7,ref13,1))==0)
             return(1);
	   p3->rtype |= ctpd[ii-1];
	   if(p3->rtype & 16)
	     {			  /* there is computer */
	      ficom(p3,0,fl_1);   /* fill pdd1.n_child */
	     }
	   if(p3->rtype & 128)
	     {			  /* there is micro */
              if(p3->rtype & 16 == 0)
	        ficom(p3,0,fl_1);   /* fill pdd1.n_child */
	      ficom(p3,128,fl_1); /* fill pdd1.n_micro */
	     }
           if(fl_1 == 2)  /* end of edit */
             {
              tosyba();
              redshr(1,bufk,sizeof(bufk));
              rwstr(buf+100,(char)1,dd_N,(short)1,nddbd,(short)1,&pdd1);
              resyba();
              redshr(1,bufk,sizeof(bufk));
             }
	  }
	return(1);
  }
ficom(p3,comi,fl_1)		  /* find in COMPUTERS */
 struct relis *p3;
 short comi;		/* 0-computer,128-micro -1-this computer */
 short fl_1;     	/* 0-no read/write for DSD */
  {
   int n=1,i,j,k;
	if(fl_1)
	  {
	   tosyba();
	   redshr(1,bufk,sizeof(bufk));
	  }
	if(comi>0)
	  {
	   fprintf(fsave,"\nMICRO NAME-");
	   if(!fcom) printf("\nMICRO NAME");
	  }
	else
	  {
	   if(comi<0)
	     {
	      fprintf(fsave,"\n!!!  THIS COMPUTER NAME-");
	      if(!fcom) printf("\n!!!  THIS COMPUTER NAME");
	     }
	   else
	     {
	      fprintf(fsave,"\nCOMPUTER NAME-");
	      if(!fcom) printf("\nCHILD NAME");
	     }
	  }
	if(fl_1 && !fcom)
	  {
	   printf("\n");
	   i=1;
echemen:
	   j=0;
	   while(rwstr(buf+100,0,i,1,ncoms,1,buf))
	    {
	     if(k=nnam(buf,100,buf+100,t_f,*(short *)(buf+6)))
	       {
	        buf[k]=0;
	        printf(" %d. %s\n",i,buf);
	        if(++j == 20) break;
	       }
	     i++;
	    }
bcemen:
	   if(i==1) goto exi;
	   printf("enter number or RETURN/Enter:");
	   k=0;
	   while((buf[k++]=getchar())!='\n')
	     ;
	   if(buf[0]=='\n')
	     if(j!=20)
	       goto bcemen;
	     else
	       goto echemen;
	   i=0;
	   sscanf(buf,"%d",&i);
	   fprintf(fsave,"computer number %d\n",i);
	   if(i)
	     {
	      rwstr(buf+100,0,i,1,ncoms,1,buf);
	      nnam(buf,100,buf+100,t_f,*(short *)(buf+6));
              if(comi < 0)
                {
                 gethostname(buf+100,70);
                 for(i=100;i<100+70;i++)
                   if(buf[i] == '.')
                     {
                      buf[i]=0;
                      break;
                     }
                 if(strcmp(buf,buf+100))
                    printf("\n\n%s  ==> is not THIS COMPUTER (%s) !!! \n\n",buf,buf+100);
                }
	     }
	  }
	else   	/* for DSD and from command file */
	  {
	   i=intext(buf,0);
	   if(!i || !fl_1) goto exi;
	  }
	while(rwstr(buf+80,0,n++,1,ncoms,1,buf+50))
	  if(nnam(buf+80,50,buf+140,t_f,*(short *)(buf+56)))
	    if(strcmp(buf,buf+80)==0)
	      {
	       if(comi<0 && !buf[50])
	         {
	          computer = *(short *)(buf+56);
	          pdd1.dbti= base_number;
                  pdd1.n_parent= computer;
	          goto exi;
	         }
	       if(comi>0 && buf[50])
	         {
	          pdd1.n_micro= *(short *)(buf+56);
                  if(!pdd1.n_child)
                    pdd1.n_child = *(short *)(buf+52);
                  if(pdd1.n_child != *(short *)(buf+52))
                    {
                     printf("CHILD is wrong for MICRO --> changed for real\n");
                     pdd1.n_child = *(short *)(buf+52);
                    }
	          goto exi;
	         }
	       if(!comi && !buf[50])
	         {
	          pdd1.n_child= *(short *)(buf+56);
	          goto exi;
	         }
	       fprintf(fsave,"\n***%s IS NOT ",buf);
	       if(comi>0) fprintf(fsave,"MICRO***");
	       else fprintf(fsave,"COMPUTER***");
	       if(!fcom)
	         {
	          printf("\n%s IS NOT ",buf);
	          if(comi>0) printf("MICRO");
	          else printf("COMPUTER");
	         }
	       goto exi;
	      }
	if(!fcom) printf("\nNAME %s IS NOT FOUND\n",buf);
	fprintf(fsave,"***NAME IS NOT FOUND***\n");
exi:
	if(!fl_1) return(0);
	resyba();
	redshr(1,bufk,sizeof(bufk));
  }

tab_menu(nbss,n)
 short nbss;
 int n;
  {
   int i,j,k;
	tosyba();
	redshr(1,bufk,sizeof(bufk));
        if(fcom)
           inpnum(&i);
	else
          {
	   i=1;
echemen:
	   j=0;
	   while(k=rwstr(buf+100,0,i,1,nbss,1,buf))
	     {
	      buf[k]=0;
	      if(buf[0])
                {
                 printf(" %d. %s\n",i,buf);
                 ++j;
                }
              ++i;
	      if(j == 20) break;
	     }
bcemen:
	   if(i==1 || j==0) return(0);
	   printf("enter number or RETURN/Enter:");
	   k=0;
	   while((buf[k++]=getchar())!='\n')
	     ;
	   if(buf[0]=='\n')
	     if(j!=20)
	       goto bcemen;
	     else
	       goto echemen;
	   i=0;
	   sscanf(buf,"%d",&i);
          }
        fprintf(fsave,"%d\n",i);
        return(i);
  }

