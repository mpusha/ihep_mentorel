/* create description file of existing table with/without data */

#include "typdef.h"
#include <stdio.h>
#include <stdlib.h>
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
#define bases_name 60
#define nsub_name 16

extern FILE *stdout1;
#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 1600
int er,_a,_b,_c,bufk[catdli];
extern short error;
extern int bptr,bptr0,bptrcf;
extern int begb,lasb,curb,lor,lorab,lowo;
short base_number,computer;
FILE *output,*fsave,*fcom;	/* fcom for tables.o only */
char buf[bufdli],work[size2b],inpnum();
static char genpol=0x39;	/* x5+x4+x3+1 */

struct sddbd pdd1,*pdd2;
struct timdat *ptd;
struct ncat *p1;
struct atc  *patc;
static struct atli *patli;
static struct relis p3,*p30 ;
char *cpt;
short adlina[]={1,2,4,4,8,0,0,2,4,6};

static char *ref1[]={
		"INCLUDE REL.",
		"WRITE DATA"
		};
char *ref2[]={
	"MATRIX",
	"TABLE"
	};
char *ref4[]={
	"8-BIT INTEGER",	/* 1-1*/
	"16-BIT INTEGER",	/* 2-2*/
	"32-BIT INTEGER",	/* 3-4*/
	"32-BIT REAL",		/* 4-4*/
	"64-BIT REAL",  	/* 5-8*/
	"HEXADECIMAL",		/* 6-FROM 1 TO 255 */
	"ASCII TEXT",		/* 7-FROM 1 TO 255*/
	"REF.TO TEXT",		/* 8-2*/
	"TO HARDWARE",		/* 9-4*/
	"TO RELATION"		/*10-6*/
	};
char *ref12[]={
	"GLOBAL",
	"LOCAL"
	};
char *ref13[]={
	"MY RAM",
	"COMPUTER RAM",
	"MICRO RAM",
	"COMPUTER+MICRO RAM",
	"MY + COMPUTER RAM",
	"MY + MICRO RAM",
	"MY + MICRO + COMPUTER RAM",
	"! exit from menu"
	};
short ctpd[]={64,16,128,144,80,192,208};
char *ref14[]={
	"MY RAM COPY",
	"COMPUTER RAM COPY",
	"MY RAM + COMPUTER RAM",
	"! exit from menu"
	};
static char *ref16[]={
	"NOBODY'S",
	"MINE",
	"SOMEBODY'S"
	};
static char *ref15[]={
		"TABLE DESCRIPTION ONLY",
		"TABLE DATA ONLY",
		"DESCRIPTION & DATA"
		};
static char ref17[]={"IGNORE THE PROBLEM\n"};
static short ii_item,ii_struct,item_boundary,struct_boundary;

char sysbas[]={"/usr/users/ssuda/sys/config.dbm"};

short *totup=0,glnogl=0,mlen=0,numow=0,tofind;
short nsubs,ncoms,nddbd,nhoms,now,nbss;
static char subsys[]={"SUBSYSTEMS"};
static char owner[]={"OWNERS"};
static char comput[]={"COMPUTERS"};
static char ddbd[]={"DDBD"};
static char hous[]={"HOUSES"};
static char bases[]={"BASES"};
static int lo,fcod=0;	/* your rights */
static FILE *lstfdb=NULL;
static short i,ii,j,k,kk,n,nn=0,m,q,r,s,t,numr,il,lil;
main()
  {
	printf("\nOUTPUT FILE NAME-");
	if(!intext(buf,0))exit(0);
	if(!(fsave=fopen(buf,"w"))) 
	  {
	   printf("\nOPEN ERROR\n");
	   exit(0);
	  }
	bptr=bptr0=bptrcf= -1;
	bptrcf=open(sysbas,2);
	if(bptrcf != -1)
	  {
	   tosyba();
	   redshr(1,bufk,sizeof(bufk));
	   nddbd=namn(ddbd,(short)(sizeof(ddbd)-1),buf,(short)r_n);
	   nsubs=namn(subsys,(short)(sizeof(subsys)-1),buf,(short)r_n);
	   ncoms=namn(comput,(short)(sizeof(comput)-1),buf,(short)r_n);
	   nhoms=namn(hous,(short)(sizeof(hous)-1),buf,(short)r_n);
	   now=namn(owner,(short)(sizeof(owner)-1),buf,(short)r_n);
	   nbss=namn(bases,sizeof(bases)-1,buf,r_n);
	   if(now)
	     {
	      printf("\n PASSWORD-");
	      if(i=gtpswd())
		{
		 ++i;
		 j=0;
		 while(i--)
		   {
		    buf[600+j]=buf[j]^0xa1;
		    j++;
		   }
		 fprintf(fsave,"+%s\n",buf+600);
		 if((numow=fiow(now)))
	 	 fcod= *(int *)(buf+1016);
		}
	     }
	   resyba();
	  }
	else
	  {
	   printf("There is no system DB\n");
	   exit(-1);
	  }
	if(!(fcod&0x80)) exit(-1);
	printf("\n BASE NAME\n");
        fprintf(fsave,"\n");
	if(!(i=tab_menu(nbss,bases_name))) goto yxod;
	rwstr(buf+100,0,i,1,nbss,1,buf);
	er=opnshr(buf);
	if(er!=0)
	  {
	   printf("\n ? OPEN ERROR:%x\n",er);
	   exit(-1);
	  }
	output=stdout1;
	rek();
begin:
	numr=0;
	if((i=oumenu(3,ref15,1))==0)
	  {
yxo:
	   fprintf(fsave,"!\n");
yxod:
	   if(bptr != -1)close(bptr);
	   exit(0);
	  }
        struct_boundary=0;
	switch (i)
	  {
	   case 1:		/* table description */
ct:
	   fprintf(fsave,"%s\n",ref1[0]);
	   if(!edrw())break;
	   if(!rdtab()) exit(-1);
	   if(nn) goto ct;
	   break;
	   case 2:		/* data description */
ct1:
	   fprintf(fsave,"%s\n",ref1[1]);
rw1:
	   if(!edrw())break;
	   if(nn) goto rw2;
	   if(!setpar())goto rw1;
rw2:
	   patli=(PATLI)(buf+1590);
	   rdsub();
	   if(nn) goto ct1;
	   break;
	   case 3:		/* table + data */
ct2:
	   fprintf(fsave,"%s\n",ref1[0]);
	   if(!edrw())break;
	   if(!rdtab()) exit(-1);
	   fprintf(fsave,"%s\n",ref1[1]);
	   movch(buf,pdd1.rena,pdd1.lena);
	   buf[pdd1.lena]=0;
	   oulin(buf);
	   if(nn) goto rw3;
	   if(!setpar()) exit(-1);
rw3:
	   patli=(PATLI)(buf+1590);
	   rdsub();
	   if(nn) goto ct2;
	   break;
  	  }
	goto begin;
  }
oulin(adst)
 char *adst;
  {
	fprintf(fsave,"=%s\n",adst);
  }
ou_name(n)
 short n;
  {
	tosyba();
	redshr(1,bufk,sizeof(bufk));
	rwstr(buf+50,0,n,1,ncoms,1,buf);
	nnam(buf+10,90,buf+100,t_f,*(short *)(buf+6));
	oulin(buf+10);
	resyba();
	if(bptr != -1) redshr(1,bufk,sizeof(bufk));
  }
rdtab()
  {
	fprintf(fsave,"%d %d %d\n",p3.nat,p3.ntup,p3.third);
	oulin(ref2[1-(p3.rtype&1)]);    /* matrix or teble */
	j=(p3.rtype&0x20) >> 5;		/* global = 1 */
	oulin(ref12[1-j]);
	if(j==1)
	  {
           if(!fidd(1))
             printf("Table not found in the DDBD!\n");
           pdd1.n_parent=pdd2->n_parent;
           pdd1.n_child=pdd2->n_child;
           pdd1.n_micro=pdd2->n_micro;
	   ou_name(pdd1.n_parent);
	   if(!(j=(p3.rtype&208)))
	     {
	      j=0;   /* no distribution */
	      i=7;
	      goto ectc;
	     }
	   for(i=0;i<7;i++)
	     if(j==ctpd[i]) goto ectc;
	   return(0);
ectc:
	   oulin(ref13[i]);
	   if(j&0x10) ou_name(pdd1.n_child);
	   if(j&0x80) ou_name(pdd1.n_micro);
	  }
	if(p3.nsub)
	  {
	   tosyba();
	   redshr(1,bufk,sizeof(bufk));
	   rwstr(buf+100,0,p3.nsub&255,1,nsubs,1,buf);
	   buf[nsub_name]=0;
	   resyba();
	   if(bptr != -1) redshr(1,bufk,sizeof(bufk));
	   oulin(buf);
	  }
	else
	  fprintf(fsave,"\n");
	if(!p3.nowner)
	  oulin(ref16[0]);
	else
	  {
	   oulin(ref16[2]);
	   tosyba();
 	   redshr(1,bufk,sizeof(bufk));
	   rwstr(buf+100,0,p3.nowner&255,1,now,1,buf);
	   buf[17]=0;
	   resyba();
	   if(bptr != -1) redshr(1,bufk,sizeof(bufk));
	   for(j=15;j>=0;j--)
	     buf[j+1]=buf[j]^0xa1;
	   buf[0]='+';
	   oulin(buf);
	  }
	begb=bufk[t_l<<1];
	patc=(PATC)(buf+toc(buf,sizeof(struct atc),numr));
	lo=begb+patc->alb;
	j=patc->alo;
	redshr(lo++,work,sizeb);
	for(i=p3.ntup;i>0;i--)
	  {
	   fprintf(fsave,"BY NAME\n");
	   n=nnam(buf,100,buf+100,t_n,*(short *)(work+j));
	   buf[n]=0;
	   oulin(buf);
	   j += 2;
	   if(j >= sizeb)
	     {
	      j = 0;
	      redshr(lo++,work,sizeb);
	     }
	  }
	begb=bufk[a_l<<1];
	patc=(PATC)(buf+toc(buf,sizeof(struct atc),numr));
	lo=begb+patc->alb;
	patli=(PATLI)(work+patc->alo);
	redshr(lo,work,size2b);
	lo += 2;
	k=ii_item=ii_struct=0;
	for(i=p3.nat;i>0;i--)
	  {
	   switch (patli->atip)
	     {
	      case 2:
	      item_boundary=2;
	      if(struct_boundary <= 2) struct_boundary=2;
	      goto acont;
	      case 3:
	      case 4:
	      item_boundary=4;
	      if(struct_boundary <= 4) struct_boundary=4;
	      goto acont;
	      case 5:
	      item_boundary=4;
	      struct_boundary=8;
acont:
	      ii_item= k%item_boundary;
	      ii_struct= (k+patli->adli)%struct_boundary;
	     }
	   j=(patli->atip&255)-1;
	   oulin(ref4[j]);
	   if(!adlina[j])
	     fprintf(fsave,"%d\n",patli->adli&255);
	   if(j>0 && j<5)
	     if(ii_item || (ii_struct && i==1))
	       fprintf(fsave,"%s",ref17);
	   fprintf(fsave,"BY NAME\n");
	   n=nnam(buf,100,buf+100,a_n,patli->annu);
	   buf[n]=0;
	   oulin(buf);
	   k += ((short)patli->adli)&255;
	   ++patli;
	   if(((char *)patli - work) > (size2b-sizeof(struct atli)))
	     {
	      movch(work,work+sizeb,sizeb);
	      redshr(lo++,work+sizeb,sizeb);
	      patli = (PATLI)((char *)patli - sizeb);
	     }
	  }
	printf("\nTable description of the %s is done\n",pdd1.rena);
	return(1);
  }
nutu()
  {
  }
edrw()
  {
	if((numr=relna(buf,&i))==0)return(0);
	if(!(rlread(&p3,numr,buf) && (p3.nowner==numow || numow==1)))
	  return(0);
	return(1);
  }
setpar()		/* param.'s for read/write data */
  {
   char c;
pa1:
	 printf("\nATTRIBUTES (N OR N-M)=");
	 scanf("%d",&_a);   q=_a;
	 if((c=getchar())=='\n')
	   t=q;
	 else
	  {
	   if(c!='-')goto pa1;
	   scanf("%d",&_a);   t=_a;
	   if(getchar()!='\n')goto pa1;
	  }
	if(q<=0 || q>t || t>p3.nat)goto pa1;
	fprintf(fsave,"%d-%d\n",q,t);
pt1:
	printf("\nTUPLES (N OR N-M)=");
	scanf("%d",&_a);   m=_a;
	if((c=getchar())=='\n')
	  s=m;
	else
	  {
	   if(c!='-')goto pt1;
	   scanf("%d",&_a);   s=_a;
	   if(getchar()!='\n')goto pt1;
	  }
	if(m<=0 || m>s || s>p3.ntup)goto pa1;
	fprintf(fsave,"%d-%d\n",m,s);
pl1:
	printf("\nPLANES (N OR N-M)=");
	scanf("%d",&_a);  n=_a;
	if((c=getchar())=='\n')
	  r=n;
	else
	  {
	   if(c!='-')goto pl1;
	   scanf("%d",&_a);  r=_a;
	   if(getchar()!='\n')goto pl1;
	  }
	if(n<=0 || n>r || r>p3.third)goto pt1;
	fprintf(fsave,"%d-%d\n",n,r);
	return(1);
  }
rdsub()
  {
	if(nn)
	  {
	   n=m=q=1;
	   r=p3.third;
	   s=p3.ntup;
	   t=p3.nat;
	   fprintf(fsave,"1-%d\n1-%d\n1-%d\n",t,s,r);
	  }
	while(n<=r)
	  {
	   fprintf(fsave,".********** PLANE = %d\n",n);
	   ii=m;
	   while(ii<=s)
	     {
	      fprintf(fsave,".---------- ROW = %d\n",ii);
	      j=q;
	      while(j<=t)
		{
		 if(numatr(buf,patli,j,numr)==0)goto erw;
		 cpt=buf+sizeb;
		 il=patli->atip;
		 il &= 255;		/* type of attr. */
		 kk=patli->adli;
		 kk &= 255;		/* length */
	/* read */
		    if(rwelm(cpt,(char)0,ii,j,numr,n,buf)==0)goto erw;
		    for(i=0;i<sizeb;i++)
		      cpt[i]=0;
		    switch(il)
		      {
		       case 1:
		       sprintf(cpt,"%3d",*buf&255);
		       break;
		       case 2:
		       sprintf(cpt,"%5d",*(short *)buf);
		       break;
		       case 3:
		       sprintf(cpt,"%d",*(int *)buf);
		       break;
		       case 5:
		       sprintf(cpt,"%lg",*(double *)buf);
		       break;
		       case 4:
		       sprintf(cpt,"%g",*(float *)buf);
		       break;
		       case 6:
		       i=kk;
		       lil=0;
		       while(i--)
			 {
			  binhex(buf[lil],cpt+2*lil);
			  ++lil;
			 }
		       break;
		       case 8:
		       kk=nnam(buf,sizeb,buf+sizeb,t_f,*(short *)buf);
		       case 7:
		       movch(cpt,buf,kk);
		       cpt[kk]=0;
		       while(kk--)
		         if(cpt[kk-1]==0xd || cpt[kk-1]==0xa) 
			   cpt[kk-1]=255;
		       break;
		       case 9:
		       sprintf(cpt,"%2x %2x %2x %2x",*buf,*(buf+1),
			 *(buf+2),*(buf+3));
		       break;
		       case 10:
		       sprintf(cpt,"%5d %5d %5d",*(short *)buf,
			 *(short *)(buf+2),*(short *)(buf+4));
		      }
		       oulin(cpt);
		 ++j;
		}
	      ++ii;
	     }
	   ++n;
	  }
	printf("\nData description of the %s is done\n",pdd1.rena);
erw:
	return;
  }
binhex(c,bb)
 char c,*bb;
  {
   char d;
	d=(c>>4)&15;
	c &= 15;
	if(d<=9)
	  d += '0';
	else
	  d += 'A'-10;
	if(c<=9)
	  c += '0';
	else
	  c += 'A'-10;
	*bb++ =d;
	*bb=c;
  }
hexbin(dlb,bb,bsym)
 short dlb;
 char *bb,*bsym;
  {
   short i=dlb;
   char c;
	while(i--)
	  bb[i-1]=0;
	i=0;
	while((bsym[i]>='0' && bsym[i]<='9') ||
	  (bsym[i]>='A' && bsym[i]<='F') ||
	  (bsym[i]>='a' && bsym[i]<='f') )
	  ++i;
	--i;		/* last hexa digit */
	while(dlb--)
	  {
	   if(i<0)return;
	   if(bsym[i]>='0' && bsym[i]<='9')
	     c=bsym[i]-'0';
	   else
	     {
	      if(bsym[i]>='A' && bsym[i]<='F')
	        c=bsym[i]-'A'+10;
	      else
	        c=bsym[i]-'a'+10;
	     }
	   --i;
	   if(i>=0)
	     {
	      if(bsym[i]>='0' && bsym[i]<='9')
	        c += (bsym[i]-'0')<<4;
	      else
	        {
	         if(bsym[i]>='A' && bsym[i]<='F')
	           c += (bsym[i]-'A'+10)<<4;
	         else
	           c += (bsym[i]-'a'+10)<<4;
	        }
	      --i;
	     }
	   bb[dlb]=c;
	  }
  }
relna(buf,i)
 char *buf;
 short *i;
  {
   short j,ix,reln=0,k;
   register crc=0;
	for(j=0;j<20;j++)
	  pdd1.rena[j]=0;
	if(nn) goto bla;
	oumes(13);
	if(*i=k=pdd1.lena=intext(buf,0))
	  {
	   if(buf[0]=='*' && buf[1]==0)
	     {
	      redshr(bufk[r_n<<1],buf,sizeb);
	      p1=(P1)buf;
	      nn=p1->nnum;
bla:
	      reln=numr+1;
bla1:
	      if(reln>nn)
	        {
	         nn=0;
		 fprintf(fsave,"\n");
	         return(0);
	        }
	      if(!( *i=k=pdd1.lena=nnam(buf,100,buf+100,r_n,reln)))
	        {
	         ++reln;
	         goto bla1;
	        }
	      if(k>20) k=20;
	      oulin(buf);
	      movch(pdd1.rena,buf,k);
	      goto rex;
	     }
	   if(k>20)k=20;
	   movch(&pdd1.rena[0],buf,k);
	   if(!(reln=namn(buf,*i,buf+((*i+3)>>2<<2),(short)r_n)))
	     oumes(5);
	  }
rex:
	ix=0;
	while(k--)
   	  {
	   j=0;
	   crc ^= pdd1.rena[ix++];
	   if(crc & 0x80) j=genpol;
	   crc = crc<<1^j;
	  }
	pdd1.ncrc=crc;
	pdd1.n_parent=pdd1.n_child=pdd1.n_micro=0;
	return(reln);
  }

oumes(n)
 short n;
  {
   static char *tomes[]={
	"enter text=",              	/*  0 */
	"enter decimal=",           	/*  1 */
	"\n ? there is relation\n",  	/*  2 */
	"NUMBER OF ATTRIBUTES TUPLES PLANES",/*  3 */
	"insuffisient dynamic memory\n",/*  4 */
	"\n ? no such relation\n",  	/*  5 */
	"\n ? parameter input error\n",	/*  6 */
	"\n ? rel.inf. read error\n",	/*  7 */
	"\n ? no such tuple\n",     	/*  8 */
	"\n ? name including error\n",	/*  9 */
	"[size]",                   	/* 10 */
	"\n ? repeate mode error\n",	/* 11 */
	"\nno such attribute\n",    	/* 12 */
	"[RELATION]",             	/* 13 */
	"\ntuples",                 	/* 14 */
	"\nplanes",                  	/* 15 */
	"\n ? ERR.ON DELETE\n",      	/* 16 */
	"\n ? NO SUCH TEXT\n",      	/* 17 */
	"\n ? OPEN ERROR\n"         	/* 18 */
		};
  	  printf(" %s",tomes[n]);
  }
intext(buf,i)
 char *buf;
 short i;
  {
   short j,ii;
intx:
	oumes(0);
	j=0;
	while((buf[j]=getchar())!='\n')
		j++;
tyx:
	buf[j]='\0';
	if(buf[0]=='*') return(j);
	if(fsave)
	oulin(buf);
	return(j);
  }
char inpnum(j)
 short *j;
  {
   char c;
	oumes(1);
	*j=0;
	scanf("%d",&_a);   *j=_a;
	c=getchar();
	if(fsave)
	  fprintf(fsave,"%d%c",*j,c);
	return(c);
  }
oumenu(n,legptr,k)
 char *legptr[];
 short n,k;
  {
   char *p,**p1;
   short i,n1;
   int r;
	p1=legptr;
	n1=n;
	printf("\n");
	for(i=1;n1>0;n1--)
	  {
	   p= *legptr;
	   if((*legptr)[0])
	     printf("%3d.%s\n",i,*legptr++);
	   ++i;
	  }
i2:
	r=0;
	scanf("%d",&r);
	if(getchar()!='\n')goto i2;
	if(r <0 || r > n)goto i2;
i3:
	legptr=p1;
	return(r);
  }
