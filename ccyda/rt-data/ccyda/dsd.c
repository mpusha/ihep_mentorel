#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "typdef.h"
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

char inpnum();

extern int exten();
int _a,_b,_c,bufk[catdli];
extern short error;
extern int begb,lasb,curb,lor,lorab,lowo;
extern FILE *fcom,*fsave;
short er,nddbd,ncoms,computer,base_number;
char buf[bufdli],work[sizeb],base_name[62];
static char genpol=0x39;	/* x5+x4+x3+1 */
struct sddbd pdd1,*pdd2;
static struct timdat *ptd;
static struct atli *patli;
static struct relis p3,*p30;

char *cpt;
short adlina[]={1,2,4,4,8,0,0,2,4,6};

static char *ref0[]={""};
static char *ref1[]={
		"INCLUDE REL.",
		"DELETE REL.",
		"TEXT SUBFILES",
		"INCREASE/DECREASE",
		"SHOW REL.",
		"PRINT REL.",
		"EDIT REL.INFO.",
		"READ DATA",
		"WRITE DATA",
		"STATE LOOK/MODIFY",
		""
		};
extern char *ref2[];
extern char *ref3[];
extern char *ref4[];
static char *ref5[]={
		"TYPE",
		"INCLUDE",
		"DELETE",
		"PRINT"
		};
static char *ref6[]={
		"RELATION'S NAMES",
		"ATTRIBUTE'S NAMES",
		"TUPLE'S NAMES",
		"FIXED TEXTS"
		};
static char *ref7[]={
		"EXTEND TUPLES",
		"EXTEND PLANES",
		"DELETE TUPLES",
		"DELETE PLANES"
		};
static char *ref8[]={
		"INFORMATION",
		"TUPLES",
		"ATTRIBUTES"
		};
static char *ref9[]={
		"SHOW",
		"CHANGE"
		};
static char *ref10[]={
		"NET ADDRESS",
		"SUBSYSTEM",
		"OWNER",
		"CONTINUATION",
		"TUPLE NAME",
		"ATTR.NAME"
		};
char sysbas[]={"config.dbm"};
static char *relst[]={
		"READ ONLY",
		"READ/WRITE"
		};

short *totup=0,glnogl=0,mlen=0,numow=0,tofind;
static short k,*toin,nn;
static int fcod=0l;	/* your rights */
static int lo;
static short i,ii,j,n,m,q,r,s,t,numr,kk,il,lil;
main()
  {
	printf("\nOUTPUT FILE NAME-");
	if(!intext(buf,0))exit(0);
	if(!(fsave=fopen(buf,"w")))
	  {
er_op:
	   printf("\n ? OPEN ERROR\n");
	   exit(-1);
	  }

	printf("\n PASSWORD-");
	fprintf(fsave,".PASSWORD\n");
	j=gtpswd()+1;
	ii=0;
	while(j--)
	  {
	   buf[600+ii]=buf[ii]^0xa1;
	   ++ii;
	  }
	fprintf(fsave,"+%s\n",buf+600);
	printf("\n BASE NAME-");
	fprintf(fsave,".BASE NAME\n");
	if(intext(buf,0)==0) exit(0);
	movch(base_name,buf,60);
	glnogl=1;	/* MAY BE GLOBAL FOR = 1,2 */
begin:
	tofind=true;
	if((i=oumenu(10,ref1,1))==0)
	  {
yxod:
	   fprintf(fsave,".END OF SESSION\n");
	   fclose(fsave);
	   exit(0);
	  }
	vucle();
	switch (i)
	  {
	   case 1:		/* dialogue include */
	   if(inclur(&p3)) goto yxod;
	   break;
	   case 2:			/* dialogue delete */
	   dide();
	   break;
	   case 3:			/* dialogue for texts */
	   dtex();
	   break;
	   case 4:		/*dialogue of increase/decrease */
inde:
	   relna(buf,&i);
	   if(!i)break;
idopt:
	   if((i=oumenu(4,ref7,1))==0)goto inde;
	   switch(i)
	     {
	      case 1:
	      nutu(&j,14);
	      if(!j)goto idopt;
	      if(j>10)goto idopt;
	      intuna(j,buf);
	      break;
	      case 2:
	      nutu(&j,15);
	      if(!j)goto idopt;
	      break;
	      case 3:
	      nutu(&j,14);
	      if(!j)goto idopt;
	      break;
	      case 4:
	      nutu(&j,15);
	      if(!j)goto idopt;
	     }
	   goto inde;
	   case 5:		/*show rel.inf. */
	   case 6:		/* print */
	   patli=(PATLI)(buf+1200);
look:
	   relna(buf,&i);
	   if(!i)break;
look1:
	   if((i=oumenu(3,ref8,1))==0)goto look;
  	   goto look1;
	   case 7:		/* edit rel.info. */
edir1:
	   if(!edrw())break;
	   if((i=oumenu(6,ref10,1))==0)goto edir1;
	   eddi();
	   break;
	   case 8:		/* read data */
	   k=0;
	   goto rw1;
	   case 9:		/* write data */
	   k=1;
rw1:
	   if(!edrw())break;
	   if(!setpar())goto rw1;
	   patli=(PATLI)(buf+1590);
	   rdsub();
	   break;
	   case 10:		/* state of rel. */
lkmod:
	   relna(buf,&i);
	   if(!i)break;
	   oumenu(2,ref9,1);
	   goto lkmod;
	  }
	goto begin;
  }
dtex()
  {
tex1:
	   if((j=oumenu(4,ref6,1))==0)return;
	   vucle();
tex2:
	   if((i=oumenu(4,ref5,1))==0)goto tex1;
	   ii=i;
	   switch(i)
	     {
	      case 4:	/* print texts */
	      case 1:	/* type texts */
  	      break;
	      case 2:		/*include text(not rel.names)*/
	      if(j==1) break;
itex:
	      i=intext(buf,j==4? 1:0);
	      if(i)goto itex;
	      break;
	      case 3: 		/*delete text*/
	      if(j==1) break;
	      if((ii=oumenu(2,ref3,1))==0)goto tex2;
dnam:
	      switch(ii)
	        {
	         case 1:
	         if((i=intext(buf,0))==0)break;
	         goto dnam;
	         case 2:
	         if(inpnum(&i)!='\n')break;
	         fprintf(fsave,"%d\n",i);
	         if(!i)break;
	         goto dnam;
	        }
	  }
	goto tex2;
  }
dide()
  {
rdloop:
   relna(buf,&i);
   if(!i)return;
   goto rdloop;
  }

edrw()
  {
	relna(buf,&i);
	if(!i)return(0);
	return(1);
  }
eddi()
  {
   switch(i)
	 {
	  case 1:	/*net address*/
	  codial(&p3);
	  break;
	  case 2:	/*subsystem*/
	  printf("\nSUBSYSTEM-");
	  fprintf(fsave,".SUBSYSTEM\n");
	  if(!intext(buf,0))break;
	  break;
	  case 3:	/*owner*/
	  printf("\nOWNER-");
	  fprintf(fsave,".OWNER\n");
	  intext(buf,0);
	  break;
	  case 4:	/*continuation*/
	  break;
	  case 5:	/*tuple name*/
	  case 6:	/*attr.name*/
	  break;
	 }
  }
setpar()		/* param.'s for read/write data */
  {
   char c;
pa1:
	   printf("\nATTRIBUTES (N OR N-M)=");
	   fprintf(fsave,".ATTRIBUTES\n");
	   scanf("%d",&_a);   q=_a;
	   if((c=getchar())=='\n')
	     t=q;
	   else
	     {
	      if(c!='-')goto pa1;
	      scanf("%d",&_a);   t=_a;
	      if(getchar()!='\n')goto pa1;
	     }
	   if(q<=0 || q>t)goto pa1;
	   fprintf(fsave,"%d-%d\n",q,t);
pt1:
	   printf("\nTUPLES (N OR N-M)=");
	   fprintf(fsave,".TUPLES\n");
	   scanf("%d",&_a);  m=_a;
	   if((c=getchar())=='\n')
	     s=m;
	   else
	     {
	      if(c!='-')goto pt1;
	      scanf("%d",&_a);   s=_a;
	      if(getchar()!='\n')goto pt1;
	     }
	   if(m<=0 || m>s)goto pa1;
	   fprintf(fsave,"%d-%d\n",m,s);
pl1:
	   printf("\nPLANES (N OR N-M)=");
	   fprintf(fsave,".PLANES\n");
	   scanf("%d",&_a); n=_a;
	   if((c=getchar())=='\n')
	     {
	      if(!n)return(0);
	      r=n;
	     }
	   else
	     {
	      if(c!='-')goto pl1;
	      scanf("%d",&_a);  r=_a;
	      if(getchar()!='\n')goto pl1;
	     }
	   if(n<=0 || n>r)goto pt1;
	   fprintf(fsave,"%d-%d\n",n,r);
	return(1);
  }
rdsub()
  {
	while(n<=r)
	  {
	   ii=m;
	   while(ii<=s)
	     {
	      j=q;
	      fprintf(fsave,".A=%d T=%d P=%d-%d\n",j,ii,n,t);
	      while(j<=t)
	        {
	         if(k)
	           {			/* write */
	            printf("\nA=%d T=%d P=%d",j,ii,n);
repea:
	            for(i=0;i<512;i++)
	            buf[i]=0;
	            intext(buf,0);
	           }
	         ++j;
	        }
	      ++ii;
	     }
	   ++n;
	  }
erw:
	return;
  }
relna(buf,i)
 char *buf;
 short *i;
  {
   short j,reln=0,k;
   register crc=0;
	for(j=0;j<20;j++)
	  pdd1.rena[j]=0;
	oumes(13);
	if(*i=k=pdd1.lena=intext(buf,0))
	  {
	   if(k>20)k=20;
	   movch(&pdd1.rena[0],buf,k);
	   buf[k]=0;
	   while(k--)
	     {
	      j=0;
	      crc ^= buf[reln++];
	      if(crc & 0x80) j=genpol;
	      crc = crc<<1^j;
	     }
	   pdd1.ncrc=crc;
	  }
	return(reln);
  }

nutu(j,i)
 short *j,i;
  {
	do
	  {
	   oumes(i);
	  }
	while(inpnum(j)!='\n');
	fprintf(fsave,"%d\n",*j);
  }
intuna(j,buf)
 short j;
 char *buf;
  {
	while(j--)
	  {
	   while(!intext(buf,0))
	     ;
	  }
	toin= (short *)(&p3);
  }


oumes(n)
 short n;
  {
   static char *tomes[]={
	"enter text=",              	       /*  0 */
	"enter decimal=",                      /*  1 */
	"\n ? there is relation\n",   	       /*  2 */
	"\nNUMBER OF ATTRIBUTES TUPLES PLANES",/*  3 */
	"insuffisient dynamic memory\n",       /*  4 */
	"\n ? no such relation\n",             /*  5 */
	"\n ? parameter input error\n",        /*  6 */
	"\n ? rel.inf. read error\n",	       /*  7 */
	"\n ? no such tuple\n",                /*  8 */
	"\n ? name including error\n",         /*  9 */
	"[size]",                              /* 10 */
	"\n ? repeate mode error\n",           /* 11 */
	"\nno such attribute\n",               /* 12 */
	"\n[RELATION]",                        /* 13 */
	"\ntuples",                            /* 14 */
	"\nplanes",                            /* 15 */
	"\n ? ERR.ON DELETE\n",                /* 16 */
	"\n ? NO SUCH TEXT\n",                 /* 17 */
	"\n ? OPEN ERROR\n"                    /* 18 */
		};
  	  printf(" %s",tomes[n]);
  }
intext(buf,i)
 char *buf;
 short i;
  {
   short j;
intx:
	oumes(0);
	j=0;
	while((buf[j]=getchar())!='\n')
	  {
	   j++;
	   if(j>1000) printf("Too long string\n");
	  }
	buf[j]='\0';
	if(fsave)
	  fprintf(fsave,"%s\n",buf);
	return(j);
  }
char inpnum(j)
 short *j;
  {
   char c;
	oumes(1);
	*j=0;
	if(fcom)
	  {
	   fscanf(fcom,"%d",&_a);   *j=_a;
	   c=fgetc(fcom);
	   printf("%d%c",*j,c);
	   return(c);
	  }
	scanf("%d",&_a);  *j=_a; 
	c=getchar();
	return(c);
  }
oumenu(n,legptr,k)
 char *legptr[];
 short n,k;
  {
   char *p,**p1;
   short i,r,n1;
	p1=legptr;
	n1=n;
	printf("\n");
	for(i=1;n1>0;n1--)
	  {
	   p= *legptr;
	   if((*legptr)[0])
	     printf("%3d.%s\n",i,legptr[0]);
	   ++i;
	   ++legptr;
	  }
i2:
	r=0;
	if(inpnum(&r)!='\n')goto i2;
	if(r <0 || r > n)goto i2;
i3:
	legptr=p1;
	if(r)
	  fprintf(fsave,"%s\n",legptr[r-1]);
	else
	  fprintf(fsave,"! exit from menu\n");
	return(r);
  }

