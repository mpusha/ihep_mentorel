#include <stdio.h>
#include <stdlib.h>
#include "typdef.h"
#define r_n 1 /* rel.name */
#define a_n 2 /* atr.name */
#define t_n 3 /* tup.name */
#define t_f 4 /* text file */
#define r_l 5 /* rel.list */
#define a_l 6 /* atr.list */
#define r_d 7 /* datas */
#define t_l 8 /* tup.list */
#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 1550

#define FALSE 0
#define TRUE 1

extern int exten();
extern int bufk[catdli];
extern short error;
extern int er,bptr,bptr0,bptrcf;
extern short cmdfl,now,glnogl;
extern FILE *output;
extern char buf[],work[];

extern struct sddbd pdd1,*pdd2;
extern struct timdat *ptd;
extern short adlina[];
char *ref2[]={
	"MATRIX",
	"TABLE"
	};
char *ref3[]={
	"BY NAME",
	"BY NUMBER"
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
	"COMPUTER + MICRO RAM",
	"MY + COMPUTER RAM",
 	"MY + MICRO RAM",
 	"MY + MICRO + COMPUTER RAM"
	};
short ctpd[]={64,16,128,144,80,192,208};
static char *ref16[]={
	"NOBODY'S",
	"MINE",
	"SOMEBODY'S"
	};
static char *ref17[]={
	"N*name",
	"N*M-K"
	};
static char *ref18[]={
	"REPEAT FROM FIRST ATTRIBUTE",
	"REPEAT FROM PREVIOUS ATTRIBUTE",
	"IGNORE THE PROBLEM",
	"CHANGE THIS ATTRIBUTE TYPE",
	"ADD BLANK BYTES TO THE ATTRIBUTE"
	};
FILE *fcom,*fsave;
extern short *totup,mlen,numow,tofind;
extern int _a,_b,_c;

static   char c,c1,c2;
static   short i,i1,i2,ii,j,n,m,q,numr,*wref,*poi,nn,k;
static   int lo;
static short ii_item,ii_struct,item_boundary,struct_boundary;
inclur(p3)

 struct relis *p3;
  {
   struct atli *patli;
ista:
	relna(buf,&i);
	if(!i) goto inco1;
inc1:
	oumes(3);
	c=inpnum(&p3->nat);
	if(c=='\n' && !p3->nat)
	   goto ista;
	   if(!(p3->nat!=0 && c==' '))
		 {
einc1:
		  oumes(6);		/* error on entered param. */
		  goto inc1;
		 }
 	   scanf("%d %d",&_a,&_b);  p3->ntup=_a; p3->third=_b;
	   while(getchar()!='\n')
	     ;
	   if(p3->ntup==0 || p3->third==0)goto einc1;
	   fprintf(fsave,"%d %d %d\n",p3->nat,p3->ntup,p3->third);
	   totup=malloc((p3->nat+1)*sizeof(struct atli));
	   patli=(PATLI)totup;
	   if((p3->rtype=oumenu(2,ref2,1))==0)goto einc1;
	   p3->rstate=1;		/* you can write */
ico0:
	p3->nowner=0;
	p3->nsub=p3->res1=p3->res2=0;
	if(!codial(p3,numr,0))return(0);
	printf("\nSUBSYSTEM-");
	fprintf(fsave,".SUBSYSTEM\n");
	i=intext(buf,0);
	printf("\nOWNER OF RELATION:\n");
	fprintf(fsave,".OWNER OF RELATION\n");
	if((n=oumenu(3,ref16,1))==0)goto ico0;
	if(n==3)
	   i=intext(buf,0);
	p3->nexlis.nrr=p3->nexlis.nrt=p3->nexlis.nra=0;
	nn=0;
	n=ntp(p3->ntup);
	if(n==1)goto einc1;
	q=k=nn=0;
	n=dat(p3);
	if(n==1)goto einc1;
	p3->rtype += q;
	p3->lotup = k;                 /* num.of bytes in tuple */
       lo=(long)p3->ntup*(short unsigned)k*p3->third; /* num.of bytes in data */
inco1:
	fprintf(fsave,".END OF INCLUDE\n");
	return(0);
  }
set_it_st(patli)
  struct atli *patli;
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
    }
comyx()
  {
printf("Error on the length of structure or boundary alignment");
printf(". Stop process??? (y or n)=\n");
 	if((c1=getchar())!='\n') while(getchar()!='\n');
	if(c1=='y' || c1=='Y') exit(0); 
  }
dat(p3,patli)
 struct relis *p3;
 struct atli *patli;
  {
astar:
	q=k=nn=ii_item=ii_struct=0;
	item_boundary=struct_boundary=1;
	   for(n=1;n<=p3->nat;n++)
		 {
ina1:
		  if(!fcom) vucle();
		  printf("\n\nATTRIBUTE %d\n",n);
		  if((p3->rtype&1)==0 || n==1)
			{
			 if((i=oumenu(10,ref4,1))==0)return(1);
			 if(i>7)q=2;
			 patli->atip=i--;
			 if((patli->adli=adlina[i])==0)
			   {
ina2:
				nutu(&i,(short)10);
				if(i>255 || i<=0)
				  {
				   oumes(11);
				   goto ina2;
				  }
				patli->adli=i;
			   }
			}
	if(patli->atip>1 && patli->atip <6)
	  {
	   set_it_st(patli);
	   if(ii_item)
	     {
	      if(fcom) comyx();
	      printf("\n\nBoundary alignment problem!!!!\n\n");
	      if((ii=oumenu(4,ref18,1))==0)
	        {
dastr:
	         patli=(PATLI)wref;
 	         goto astar;
	        }
	      switch (ii)
	        {
	         case 1:
	         goto dastr;
	         case 2:
	         if(n==1) goto dada;
	         patli=(PATLI)wref;
	         k=q=ii_item=ii_struct=0;
	         --n;
	         ii=item_boundary=struct_boundary=1;
	         while(ii < n)
	           { 
	            ii++;
	            set_it_st(patli);
	            k += (patli->adli)&255;
	            if(patli->atip > 7) q=2;
	            ++patli;
	           }
	         goto ina1;
	         case 3:
	         break;
	         case 4:
dada:
	         goto ina1;
	        }
	     }
	   if(ii_struct && n==p3->nat)
	     {
	      if(fcom) comyx();
	      printf("\n\nStruct length problem!!!\n\n");
	      if((ii=oumenu(5,ref18,1))==0) goto dastr;
	      switch (ii)
	        {
	         case 1:
	         goto dastr;
	         case 2:
	         --patli;
	         k -= ((short)patli->adli)&255;
	         --n;
	         goto ina1;
	         case 3:
	         break;
	         case 4:
	         goto ina1;
	         case 5:
	         patli->adli += (struct_boundary-ii_struct);
	        }
	     }
	  }
		 patli->aoff=k;
		 k += ((short)patli->adli)&255;
	  if(nn==0)
	  if((ii=oumenu(2,ref3,1))==0)goto ina1;
	  printf("\nATTRIBUTE #%d\nmay be %s\n",n,ref17[ii-1]);
	  switch(ii)
		{
		 case 1:
		 if((i=intext(buf,0))==0)goto ina1;
		 sscanf(buf,"%d%s",&_a,buf+sizeb);  m=_a;
		 if(m!=0 && *(buf+sizeb)=='*')
		   {
			nn=m;
			m=i=0;
			while(buf[m++]!='*')
			  ;
			while((buf[i]=buf[m++])!=0)
			  ++i;
		   }
		 if(i==0)goto ina1;
		 break;
		 case 2:
ina3:
		 if(!(i=intext(buf,0))) goto ina1;
		 buf[i]='\n';
	sscanf(buf,"%d%c%d%c%d%c",&_a,&c,&_b,&c1,&_c,&c2);	/* N*M-K */
		j=_a;  i1=_b;  i2=_c;
		 if(c=='*')
		   {
			nn=j;
			j=i1;
			c=c1; c1=c2; i1=i2;
		   }
		 if((i=j)==0) goto ina1;
		 if(c=='\n') goto ina4;
		 if(c=='-')
		   {
		    if(!(p3->rtype&1)) goto ina04;
		    i=i1;
		    c=c1;
		   }
		 if(c!='\n' || j>i)
		   {
ina04:
			oumes(11);
			goto ina3;
		   }
ina4:
		 while(j<=i)
		   {
		    ++j;
		    if((p3->rtype&1)==0)break;
		    if(j<=i) ++n;
		    if(n > p3->nat)break;
		   }
		 break;
		}
	  if(nn!=0) --nn;
	 }
	return(0);
  }
ntp(zx)
 short zx;
  {
	for(n=1;n<=zx;n++)
	{
int1:
	 if(!fcom) vucle();
	 printf("\n\nTUPLE %d\n",n);
	 fprintf(fsave,".TUPLE %d\n",n);
	 if(nn==0)
	   if((ii=oumenu(2,ref3,1))==0) return(1);
	 printf("\nTUPLE #%d\nmay be %s\n",n,ref17[ii-1]);
	 switch(ii)
	   {
	    case 1:
	    if((i=intext(buf,0))==0)goto int1;
	    *(buf+sizeb)=0;
	    sscanf(buf,"%d%s",&_a,buf+sizeb);
	    k=_a;
	    if(k!=0 && *(buf+sizeb)=='*')
	      {
printf("nn=%d k=%d i=%d ii=%d _a=%d %s\n",nn,k,i,ii,_a,buf+sizeb);
	       nn=k;
	       k=i=0;
	       while(buf[k++]!='*')
		 ;
	       while((buf[i]=buf[k++])!=0)
		 ++i;
	      }
	    if(i==0)goto int1;
	    break;
	    case 2:
int2:
	    if(!(i=intext(buf,0))) goto int1;
	    buf[i]='\n';
	    sscanf(buf,"%d%c%d%c%d%c",&_a,&c,&_b,&c1,&_c,&c2);
	    j=_a;  i1=_b;  i2=_c;
	    if(c=='*')
	      {
	       nn=j;
	       j=i1;
	       c=c1; i1=i2; c1=c2;
	      }
	    if((i=j)==0)goto int1;
	    if(c=='\n')goto int3;
	    if(c=='-')
	      {
	       i=i1;
	       c=c1;
	      }
	    if(c!='\n' || j>i)
	      {
	       oumes(6);
	       goto int2;
	      }
int3:
	    while(j<=i)
	      {
	       ++j;
	       if(j<=i) ++n;
	       if(n > zx)break;
	      }
	   }
	 if(nn!=0) --nn;
	} 
	return(0);
  }

