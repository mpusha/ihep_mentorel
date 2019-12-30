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
#define subs_name 16

extern int exten();
extern int bufk[catdli];
extern short error;
extern int er,bptr,bptr0,bptrcf;
extern short now,glnogl;
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
	"64-BIT REAL",		/* 5-8*/
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
        "MY + MICRO + COMPUTER RAM"
	};
short ctpd[]={64,16,128,144,80,192,208};
static char *ref16[]={
	"NOBODY'S",
	"MINE",
	"SOMEBODY'S"
	};
static char *ref17[]={
	"REPEAT FROM FIRST ATTRIBUTE",
	"REPEAT FROM PREVIOUS ATTRIBUTE",
	"IGNORE THE PROBLEM",
	"CHANGE THIS ATTRIBUTE TYPE",
	"ADD BLANK BYTES TO THE ATTRIBUTE"
	};
static char *ref18[]={
	"N*name",
	"N*M or M-K"
	};
FILE *fcom,*fsave;
extern short *totup,mlen,numow,tofind,nsubs,ncoms,nddbd;

static   char c,c1,c2;
static   short _a,_b,_c,i,i1,i2,ii,j,n,m,q,numr,*wref,*poi,nn,k;
static   int lo;
static short ii_item,ii_struct,item_boundary,struct_boundary;
inclur(p3)

 struct relis *p3;
  {
   struct atli *patli;
ista:
	tofind=0;
	if(relna(buf,&i))goto inco1;
	if(!i) goto inco1;
	numr=n_incl(buf,i,buf+((i+3)>>2<<2),r_n);
inc1:
	oumes(3);
	c=inpnum(&p3->nat);
	if(c=='\n' && !p3->nat)
	  {
	   n_del(numr,buf,(short)r_n);
	   goto ista;
	  }
	   if(!(p3->nat!=0 && c==' '))
		 {
einc1:
		  oumes(6);		/* error on entered param. */
		  goto inc1;
		 }
	   if(fcom)
	     fscanf(fcom,"%d %d",&_a,&_b);
	   else
	      scanf("%d %d",&_a,&_b);
	      p3->ntup=_a;
	      p3->third=_b;
	   if(fcom)
	     while(fgetc(fcom)!='\n')
	       ;
	   else
	     while(getchar()!='\n')
	       ;
	   if(p3->ntup==0 || p3->third==0)goto einc1;
	   fprintf(fsave,"%d %d\n",p3->ntup,p3->third);
	   i=(p3->ntup<<1)+(p3->nat+1)*sizeof(struct atli);
	   totup=malloc((mlen=i));
	   if(!totup || mlen<i)
	     {
yxod1:
	     n_del(numr,buf,(short)r_n);
	     oumes(4);
yxod:
	     if(fsave)fclose(fsave);
	     if(fcom)fclose(fcom);
	     fcom=fsave=0;
	     return(1);
	    }
	  patli=(PATLI)(totup+p3->ntup);
	  if((p3->rtype=oumenu(2,ref2,1))==0)goto einc1;
	  p3->rstate=1;		/* you can write */
ico0:
	p3->nowner=0;
	if(ncoms && bptr!=bptrcf)
	  if(!codial(p3,numr,1))return(0);
	if(nsubs && bptr!=bptrcf)
	  {
	   fprintf(fsave,"SUBSYSTEM-");
	   if(!fcom)
	     {
	      printf("SUBSYSTEM:\n");
	      p3->nsub=tab_menu(nsubs,subs_name);
	      resyba();
	      redshr(1,bufk,sizeof(bufk));
	     }
	   else
	     if(intext(buf,0)) p3->nsub=fiow(nsubs);
	  }
	if(now)
	  {
	   fprintf(fsave,"OWNER OF RELATION:\n");
	   if(!fcom) printf("OWNER OF RELATION:\n");
	   if((n=oumenu((short)3,ref16,(short)1))==0)goto ico0;
	   switch (n)
		 {
		  case 2:
		  p3->nowner=numow;
		  break;
		  case 3:
		  if(fcom)
		    i=intext(buf,0);
		  else
		    i=gtpswd();
		  if(i) p3->nowner=fiow(now);
		 }
	  }
	p3->nexlis.nrr=p3->nexlis.nrt=p3->nexlis.nra=0;
	wref=totup;
	nn=0;
	n=ntp(p3->ntup);
	if(n==1)goto einc1;
	wref=(short *)patli;		/* start of attribute list */
	n=dat(p3,patli);
	if(n==1)goto einc1;
	p3->rtype += q;
	p3->lotup = k;                 /* num.of bytes in tuple */
	lo=(long)p3->ntup*(unsigned)k*p3->third; /* num.of bytes in data */
	if(p3->rtype & 32)             /* global */
	  if(!fidd((short)(-1)))
	    {
	     fprintf(fsave,"\n ? CAN'T WRITE IN 'DDBD'\n");
	     if(!fcom) printf("\n ? CAN'T WRITE IN 'DDBD'\n");
	     goto yxod1;
	    }
	if(numr!=(i=r_incl(p3,buf)))
	  {
	   fprintf(fsave,"\n ? ERROR: R_N=%d R_L=%d \n",numr,i);
	   if(!fcom) printf("\n ? ERROR: R_N=%d R_L=%d \n",numr,i);
	   goto yxod1;
	  }
	if(numr!=(i=a_incl(totup,(int)(p3->ntup<<1),buf,t_l)))
	  {
	   fprintf(fsave,"\n ? ERROR: R_N=%d T_L=%d\n",numr,i);
	   if(!fcom) printf("\n ? ERROR: R_N=%d T_L=%d\n",numr,i);
	   goto yxod;
	  }
	if(numr!=(i=a_incl(wref,(int)(p3->nat*sizeof(struct atli)),buf,a_l)))
	  {
	   fprintf(fsave,"\n ? ERROR: R_N=%d A_L=%d\n",numr,i);
	   if(!fcom) printf("\n ? ERROR: R_N=%d A_L=%d\n",numr,i);
	   goto yxod;
	  }
	for(i=0;i<sizeb;i++)
	  work[i]=0;
	if(numr!=(i=a_incl(work,lo,buf,r_d)))
	  {
	   fprintf(fsave,"\n ? ERROR: R_N=%d R_D=%d\n",numr,i);
	   if(!fcom) printf("\n ? ERROR: R_N=%d R_D=%d\n",numr,i);
	   goto yxod;
	  }
inco1:
	return(0);
  }
set_it_st(patli)
  struct atli *patli;
    {
	switch (patli->atip)
	  {
	   case 2:
	   item_boundary=2;
	   if(struct_boundary < 2) struct_boundary=2;
	   goto acont;
	   case 3:
	   case 4:
	   item_boundary=4;
	   if(struct_boundary < 4) struct_boundary=4;
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
 char c1;
printf("Error on the length of structure or boundary alignment");
printf(".\n Stop process??? (y or n)=");
 	if((c1=getchar())!='\n') while(getchar()!='\n');
	if(c1=='y' || c1=='Y') exit(0); 
  }
dat(p3,patli)
 struct relis *p3;
 struct atli *patli;
  {
    int ii1=0;
astar:
	q=k=nn=ii_item=ii_struct=0;
	item_boundary=struct_boundary=1;
	for(n=1;n<=p3->nat;n++)
	  {
ina1:
	     if(!fcom) vucle();
	     fprintf(fsave,"\n\nATTRIBUTE %d\n",n);
	     if(!fcom) printf("\n\nATTRIBUTE %d\n",n);
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
	           if(fcom)
	             comyx();
	           fprintf(fsave,"\n\nBoundary alignment problem!!!!\n\n");
	           if(!fcom) printf("\n\nBoundary alignment problem!!!!\n\n");
	           if((ii1=oumenu(4,ref17,1))==0)
	             {
dastr:
	              patli=(PATLI)wref;
 	              goto astar;
	             }
	           switch (ii1)
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
	           if(fcom)
	             comyx();
	           fprintf(fsave,"\n\nStruct length problem!!!\n\n");
	           if(!fcom) printf("\n\nStruct length problem!!!\n\n");
	           if((ii1=oumenu(5,ref17,1))==0) goto dastr;
	           switch (ii1)
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
	if(nn==0)
	  if((ii=oumenu(2,ref3,1))==0)goto ina1;
	if(c!='+')
	  {
	   fprintf(fsave,"\nATTRIBUTE #%d\nmay be %s\n",n,ref18[ii-1]);
	   if(!fcom) printf("\nATTRIBUTE #%d\nmay be %s\n",n,ref18[ii-1]);
	  }
	switch(ii)
	  {
	   case 1:
	   if((i=intext(buf,0))==0)goto ina1;
           _a=0;
           *(buf+sizeb)=0;
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
	   if((i=n_incl(buf,i,buf+((i+3)>>2<<2),a_n))==0)
	     {
	      oumes(9);
	      goto ina1;
	     }
	   patli->annu=i;
	   patli->aoff=k;
	   k += ((short)patli->adli)&255;
	   movch(patli+1,patli,(short)sizeof(struct atli));
	   ++patli;
	   break;
	   case 2:
ina3:
	   if(c=='+')
	     {
	      j= _a;
	      i= _b;
	      goto ina4;
	     }
	   if(!(i=intext(buf,0))) goto ina1;
	   buf[i]='\n';
	   sscanf(buf,"%d%c%d%c%d%c",&_a,&c,&_b,&c1,&_c,&c2);	/* N*M-K */
	   j=_a; i1=_b; i2=_c;
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
	      i=i1;
	      c=c1;
	     }
	   if(c!='\n' || j>i)
	     {
	      c=0;
	      oumes(11);
	      goto ina3;
	     }
	   if(j!=i && (p3->rtype&1)==0)
	     {
	      nn=i-j+1;
	      c='+';
	     }
ina4:
/*printf("nn=%d i=%d j=%d ii=%d c=%c _a=%d _b=%d\n",nn,i,j,ii,c,_a,_b);
*/	   while(j<=i)
	     {
	      patli->annu=j;
	      if((m=nnam(buf,sizeb,buf+sizeb,a_n,j))==0)
	        {
	         c=nn=0;
	         oumes(12);
	         goto ina1;
	        }
	      if((m=n_incl(buf,m,buf+((m+3)>>2<<2),a_n))==0)
	        {
	         c=0;
	         oumes(9);
	         goto ina3;
	        }
	      ++j;
	      _a=j;	/* save for next if not matrix */
	      _b=i;
	      patli->aoff=k;
	      k += ((short)patli->adli)&255;
	      movch(patli+1,patli,(short)sizeof(struct atli));
	      ++patli;
	      if((p3->rtype&1)==0)break;
	      if(j<=i) ++n;
	      if(n > p3->nat)break;
	     }
	   break;
	  }
	if(nn!=0) --nn;
        if(!nn) c=0;
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
	   fprintf(fsave,"\n\nTUPLE %d\n",n);
	   if(!fcom) printf("\n\nTUPLE %d\n",n);
	   if(nn==0)
	     if((ii=oumenu(2,ref3,1))==0) return(1);
	   fprintf(fsave,"\nTUPLE #%d\nmay be %s\n",n,ref18[ii-1]);
	   if(!fcom) printf("\nTUPLE #%d\nmay be %s\n",n,ref18[ii-1]);
	   switch(ii)
	     {
	      case 1:
	      if((i=intext(buf,0))==0)goto int1;
	      *(buf+sizeb)=0;
	      sscanf(buf,"%d%s",&_a,buf+sizeb);
	      k=_a;
	      if(k!=0 && *(buf+sizeb)=='*')
	        {
		 nn=k;
		 k=i=0;
		 while(buf[k++]!='*')
		   ;
		 while((buf[i]=buf[k++])!=0)
		   ++i;
	        }
	      if(i==0)goto int1;
	        if((k=n_incl(buf,i,buf+((i+3)>>2<<2),t_n))==0)
	          {
eint1:
	           oumes(9);
		   goto int1;
		  }
	      *wref++ = k;
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
		 if((k=nnam(buf,(short)sizeb,buf+sizeb,(short)t_n,j))==0)
		   {
		    oumes(8);
		    goto int1;
		   }
	         if((k=n_incl(buf,k,buf+((k+3)>>2<<2),t_n))==0)goto eint1;
		   ++j;
	         *wref++ = k;
	         if(j<=i) ++n;
	         if(n > zx)break;
		}
	     }
	   if(nn!=0) --nn;
	  }
	return(0);
  }
