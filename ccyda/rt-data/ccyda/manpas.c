/* create description file of T_MAIN, P_MAIN, T_PASSPORT and P_PASSPORT tables */

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
#define P_nat 11  /* num. of attributes of P_PASSPORT */
#define T_nat 8   /* num. of attributes of T_PASSPORT */

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
static char c;
short ctpd[]={64,16,128,144,80,192,208};

struct sddbd pdd1,*pdd2;
struct timdat *ptd;
struct ncat *p1;
struct atc  *patc;
static struct atli *patli;
static struct relis p3,*p30 ;
char *cpt;
short adlina[]={1,2,4,4,8,0,0,2,4,6};
static char *comname={"ssuda"};
static char *ref1={
		"INCLUDE REL."
		};
char *ref2={
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
char *ref12={
	"GLOBAL"
	};
char *ref13={
	"! exit from menu"
	};
char *ref14[]={
	"MY RAM COPY",
	"COMPUTER RAM COPY",
	"MY RAM + COMPUTER RAM",
	"! exit from menu"
	};
static char *ref16={
	"NOBODY'S"
	};
static char *ref15[]={
		"P_MAIN_TABLE",
		"T_MAIN_TABLE",
		"P_PASSPORT_TABLE",
		"T_PASSPORT_TABLE"
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
static short i,ii,j,k,kk,n,nn=0,m,q,r,s,t,numr,lil;
static short nat,ntup,nplain,npar;
main()
  {
	printf("\nOUTPUT FILE NAME-");
	if(!intext(buf,0))
          exit(0);
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
	if(!(fcod&0x80))
          exit(-1); /* this user has not rights to edit DB's */
	printf("\n BASE NAME\n");
        fprintf(fsave,"\n");
	if(!(i=tab_menu(nbss,bases_name)))
          goto yxod;
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
	if((lil=oumenu(4,ref15,1))==0)
	  {
yxo:
	   fprintf(fsave,"!\n");
yxod:
	   exit(0);
	  }
	switch (lil)
	  {
	   case 1:		/* P_MAIN */
	   case 2:		/* T_MAIN */
           oumes(7);
           c=inpnum(&npar);
           if(!npar)
             goto yxo;
           nat = 5 + 4*npar;
	   break;
	   case 3:		/* P_PASSPORT */
           nat = P_nat;
	   break;
	   case 4:		/* T_PASSPORT */
           nat = T_nat;
	   break;
  	  }
	fprintf(fsave,"%s\n",ref1);   /* INCLUDE REL. */
        if((numr=relna(buf,&i))==0)   /* relation name */
          goto yxo;
ist:
        oumes(3);
        c=inpnum(&ntup);
        if(c=='\n' && !ntup)
          goto yxo;
        if(!(ntup!=0 && c==' '))
          {
einc1:
           oumes(6);             /* error on entered param. */
           goto ist;
          }
        scanf("%d",&_a);
        nplain= _a;
        while(getchar()!='\n')
          ;
        if(nplain==0)goto einc1;
        if(!rdtab())
          goto yxo;
        printf("\n*** Description for table %s is prepared ***\n\n",pdd1.rena);
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
  }
void set_tups(int name)
  {
   for(j=1;j<=ntup;j++)               /* set names of tuples */
     {
      fprintf(fsave,"BY NAME\n");
      if(!name)
        fprintf(fsave,"=K%d\n",j);   /* MAIN */
      else
        fprintf(fsave,"=str%d\n",j); /* PASSPORT */
     }
  }
void set_fl(int fl_num)
  {
   oulin("HEXADECIMAL");
   fprintf(fsave,"%d\n",2);
   fprintf(fsave,"BY NAME\n");
   sprintf(buf,"flag%d",fl_num);
   oulin(buf);  
  }
void set_8_bit(char *name)
  {
   oulin("8-BIT INTEGER");
   fprintf(fsave,"BY NAME\n");
   oulin(name);
  }
void set_16_bit(char *name)
  {
   oulin("16-BIT INTEGER");
   fprintf(fsave,"BY NAME\n");
   oulin(name);
  }
void set_64_real(char *name)
  {
   oulin("64-BIT REAL");
   fprintf(fsave,"BY NAME\n");
   oulin(name);
  }
void set_ASCII(char *name, int len)
  {
   oulin("ASCII TEXT");
   fprintf(fsave,"%d\n",len);
   fprintf(fsave,"BY NAME\n");
   oulin(name);
  }
void set_flags(int name)
  {
   if(!name)   /* MAIN */
     set_fl(0);
   set_fl(1);
   set_fl(2);
  }
void set_pas(int tip)  /* 0-physical, 1-technological */
  {
   set_16_bit("copy");
   set_ASCII("Parameters",16);
   set_ASCII("OutFormat",10);
   set_16_bit("ParamNum");
   if(!tip)
     set_16_bit("VarType");
   else
     set_16_bit("PlaneNum");
   set_16_bit("res");
   if(!tip)
     {
      set_16_bit("Control");
      set_64_real("Min");
      set_64_real("Max");
     }
  }
rdtab()
  {
	fprintf(fsave,"%d %d %d\n",nat,ntup,nplain);
	oulin(ref2);            /* TABLE            */ 
	oulin(ref12);           /* GLOBAL           */
	oulin(comname);
	oulin(ref13);           /* ! exit from menu */
	fprintf(fsave,"\n");    /* no subsystem     */
	oulin(ref16);           /* NOBODY'S         */
	switch (lil)
	  {
	   case 1:		/* P_MAIN */
	   case 2:		/* T_MAIN */
           set_tups(0);
           set_flags(0);
           set_8_bit("OutAxis");
           set_8_bit("ParamNum");
           for(j=1;j<=npar;j++)  /* set parameters description */
             {
              sprintf(buf,"TabName%d",j);
              set_ASCII(buf,20);
              sprintf(buf,"plane%d",j);
              set_16_bit(buf);
              sprintf(buf,"tuple%d",j);
              set_16_bit(buf);
              sprintf(buf,"DatTabName%d",j);
              set_ASCII(buf,20);
             }
	   break;
	   case 3:		/* P_PASSPORT */
           set_tups(1);
           set_flags(1);
           set_pas(0);
	   break;
	   case 4:		/* T_PASSPORT */
           set_tups(1);
           set_flags(1);
           set_pas(1);
	   break;
  	  }

	return(1);
  }
nutu()
  {
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
   short j,reln=0,k;
   register crc=0;
	for(j=0;j<20;j++)
	  pdd1.rena[j]=0;
	oumes(13);
	if(*i=k=pdd1.lena=intext(buf,0))
	  {
           reln=1;
	   if(k>20) k=20;
	   movch(pdd1.rena,buf,k);
	  }
	return(reln);
  }

oumes(n)
 short n;
  {
   static char *tomes[]={
	"enter text=",              	/*  0 */
	"enter decimal=",           	/*  1 */
	"\n ? there is relation\n",  	/*  2 */
	"NUMBER OF TUPLES PLANES",      /*  3 */
	"insuffisient dynamic memory\n",/*  4 */
	"\n ? no such relation\n",  	/*  5 */
	"\n ? parameter input error\n",	/*  6 */
	"NUMBER OF PARAMETERS", 	/*  7 */
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
	return(c);
  }
oumenu(n,legptr,k)
 char *legptr[];
 short n,k;
  {
   char *p,**p1;
char cc;
   short i,r,n1;
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
	scanf("%d",&_a);
        r= _a;
	if((cc=getchar())!='\n')
          {
           goto i2;
          }
	if(r <0 || r > n)goto i2;
i3:
	legptr=p1;
	return(r);
  }
