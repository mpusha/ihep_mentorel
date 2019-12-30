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
#define bases_name 60
#define subs_name 16

extern FILE *fcom,*fsave,*stdout1;
#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 2500
extern int exten();
int er,_a,_b,_c,bufk[catdli];
extern short error1;
extern int begb,lasb,curb,lor,lorab,lowo;
short base_number,computer;
extern int bptr,bptr0,bptrcf;
FILE *output;
char buf[bufdli],work[bufdli],inpnum(),base_name[62];
static char genpol=0x39;	/* x5+x4+x3+1 */
void ssuda_stop(),ssuda_start();
struct sddbd pdd1,*pdd2;
struct timdat *ptd;
static struct atli *patli;
static struct atc *patc;
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
                "STOP DISTRIBUTED SSUDA",
                "START DISTRIBUTED SSUDA",
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
static char *ref11[]={
		"ALL DATA",
		"INTERSECTION"
		};
static char *ref15[]={
		"FROM TERMINAL",
		"FROM FILE"
		};
char sysbas[]={"/usr/users/ssuda/sys/config.dbm"};
static char nam1[]={"CCYDA.OUT"};
static char *relst[]={
		"READ ONLY",
		"READ/WRITE"
		};
static char *txt_0[]={
		"TUPLE NUMBER",
		"ATTR.NUMBER"
		};

short *totup=0,glnogl=0,mlen=0,numow=0,tofind,for_fcom;
short nsubs,ncoms,nddbd,nhoms,now,nbss,ncom_li,nos_li;
static char subsys[]={"SUBSYSTEMS"};
static char owner[]={"OWNERS"};
static char comput[]={"COMPUTERS"};
static char ddbd[]={"DDBD"};
static char hous[]={"HOUSES"};
static char bases[]={"BASES"};
static char compu_list[]={"COMPUTER_LIST"};
static char os_list[]={"OS_LIST"};
static short k,*toin,nn;
static int fcod=0;	/* your rights */
static int lo;
static short i,ii,j,n,m,q,r,s,t,numr,kk,il,lil;
main()
  {
	fsave=fopen(nam1,"w");
mstrt:
	if(!(i=oumenu(2,ref15,1)))exit(0);
	switch(i)
	  {
	   case 1:
	   break;
	   case 2:
	   printf("INPUT FILE NAME-");
	   fprintf(fsave,"INPUT FILE NAME-");
	   ii=intext(buf,0);
	   if(!(fcom=fopen(buf,"r")))
	     {
	      printf("CAN'T OPEN %s\n",buf);
	      fprintf(fsave,"CAN'T OPEN %s\n",buf);
	      goto mstrt;
	     }
	  }
	bptr=bptr0=bptrcf= -1;
	bptrcf=open(sysbas,2);
printf("int=%d short=%d long=%d float=%d double=%d bptrcf=%d\n",sizeof(int),sizeof(short),sizeof(long),sizeof(float),sizeof(double),bptrcf);
	if(bptrcf != -1)
	  {
	   tosyba();
	   redshr(1,bufk,sizeof(bufk));
	   nddbd=namn(ddbd,(short)(sizeof(ddbd)-1),buf,(short)r_n);
	   nsubs=namn(subsys,(short)(sizeof(subsys)-1),buf,(short)r_n);
	   ncoms=namn(comput,(short)(sizeof(comput)-1),buf,(short)r_n);
	   nhoms=namn(hous,(short)(sizeof(hous)-1),buf,(short)r_n);
	   now=namn(owner,(short)(sizeof(owner)-1),buf,(short)r_n);
	   ncom_li=namn(compu_list,sizeof(compu_list)-1,buf,r_n);
	   nos_li=namn(os_list,sizeof(os_list)-1,buf,r_n);
	   nbss=namn(bases,sizeof(bases)-1,buf,r_n);
	   if(now)
	     {
	      fprintf(fsave,"\n PASSWORD-");
	      if(!fcom) printf("\n PASSWORD-");
	      if(fcom)
		i=intext(buf,0);
	      else
	        i=gtpswd();
	      if(i)
		{
		 if((numow=fiow(now)))
	 	 fcod= *(int *)(buf+1016);
		}
	     }
	   resyba();
	  }
        else
          perror("Sys.base");
	fprintf(fsave,"\n BASE NAME-");
	if(!fcom) printf("\n BASE NAME-");
	if(intext(buf,0)==0)
	  {
	   if(!(i=tab_menu(nbss,bases_name))) goto yxod;
	   rwstr(buf+100,0,i,1,nbss,1,buf);
	   er=opnshr(buf);
	  }
	else
	  {
	   if(strcmp(buf,sysbas)==0)
	     {
	      if(numow!=1) goto yxod;
	      close(bptrcf);
	      er=opnshr(buf);
	      bptrcf=bptr0=bptr;
	     }
	   else
	     er=opnshr(buf);
printf("int=%d short=%d long=%d bptr=%d\n",sizeof(int),sizeof(short),sizeof(long),bptr);
	  }
	movch(base_name,buf,60);
	computer=0;
	if(base_number=fiow(nbss))       	/* ref.to BASES */
	glnogl=1;
	if(er!=0)
	  {
	   printf("\n ? OPEN ERROR:%x\n",er);
	   exit(0);
	  }
	rek();
	output=stdout1;
	vucle();
begin:
	tofind=true;
	for_fcom=0;
	if((i=oumenu(13,ref1,1))==0)
	  {
yxod:
	   if(bptr != -1)close(bptr);
	   exit(0);
	  }
	if(fcom && i==1)
	  {
	   printf("Include relation ==> ");
	   for_fcom=1;
	  }
	vucle();
	if(i==13)printf("\n\n%ld\n\n",bufk[18]);
	switch (i)
	  {
	   case 1:		/* dialogue include */
	   if(cost())break;		/* not permissible */
	   if(inclur(&p3)) goto yxod;
	   break;
	   case 2:			/* dialogue delete */
	   if(cost())break;
	   dide();
	   break;
	   case 3:			/* dialogue for texts */
	   dtex();
	   break;
	   case 4:		/*dialogue of increase/decrease */
	   if(cost())break;
inde:
	   if((numr=relna(buf,&i))==0)break;
idopt:
	   if((i=oumenu(4,ref7,1))==0)goto inde;
	   switch(i)
	     {
	      case 1:
	      nutu(&j,14);
	      if(!j)goto idopt;
	      if(j>10)goto yx;
	      intuna(j,buf);
	      if(tupl_a(numr,j,buf,(short)0,toin)==0)
	        {
yx:
	         oumes(11);
	         goto inde;
	        }
	      break;
	      case 2:
	      nutu(&j,15);
	      if(!j)goto idopt;
	      if(plan_a(numr,j,buf,(short)0)==0)goto yx;
	      break;
	      case 3:
	      nutu(&j,14);
	      if(!j)goto idopt;
	      if(!tupl_a(numr,j,buf,(short)1,toin))goto yx;
	      break;
	      case 4:
	      nutu(&j,15);
	      if(!j)goto idopt;
	      if(plan_a(numr,j,buf,(short)1)==0)goto yx;
	     }
	   goto inde;
	   case 5:		/*show rel.inf. */
	   case 6:		/* print */
	   patli=(PATLI)(buf+1200);
look:
	   if((numr=relna(buf,&i))==0)break;
	   if(!rlread(&p3,numr,buf))
	     {
	      oumes(7);
	      if(fcom)goto yxod;
	      goto look;
	     }
look1:
	   if((i=oumenu(3,ref8,1))==0)goto look;
	   switch(i)
	     {
	      case 1:
	      rlprint(buf,numr);
	      break;
	      case 2:
	      for(j=1;j<=p3.ntup;j++)
	        {
	         if((i=namstr(buf,patli,j,numr)))
	         fprintf(fsave,"%d. %s\n",j,patli);
	         if(!fcom) printf("%d. %s\n",j,patli);
	        }
	      break;
	      case 3:
	      for(j=1;j<=p3.nat;j++)
	      if(numatr(buf,patli,j,numr))
	      apr(j);
	     }
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
rw2:
	   if(!setpar())goto rw1;
	   patli=(PATLI)(buf+1590);
	   rdsub();
	   if(!fcom) goto rw2;
	   break;
	   case 10:		/* state of rel. */
lkmod:
	   if((numr=relna(buf,&i))==0)break;
lkmod1:
	   if((i=oumenu(2,ref9,1))==0)goto lkmod;
	   switch(i)
	     {
	      case 1:		/* look state */
st_r:
	      if(!rlread(&p3,numr,buf))
	        {
	         oumes(7);
	         if(fcom)goto yxod;
	         break;
	        }
	      if(!fcom) printf("%s\n",relst[p3.rstate]);
	      fprintf(fsave,"%s\n",relst[p3.rstate]);
	      if(fcom) goto lkmod;
	      goto lkmod1;
	      case 2:		/* change state */
	      if(!cost())
	        {
	         stxor(numr,buf);
	         goto st_r;
	        }
	     }
	   case 11:		/* stop */
	   if(cost()) break;	/* not permissible */
           ssuda_stop();
	   break;
	   case 12:		/* start */
	   if(cost()) break;	/* not permissible */
           ssuda_start();
	   break;
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
	   lo=bufk[j<<1];
	   redshr(lo,buf,sizeb);
	   n= *(short *)buf;	/* number of texts */
	   ii=i;
	   switch(i)
	     {
	      case 4:	/* print texts */
	      case 1:	/* type texts */
	      vucle();
	      k=1;
	      while(n--)
	        {
	         if((i=nnam(buf,(short)sizeb,buf+sizeb,j,k))!=0)
	           {
	            if(i>=sizeb) i=sizeb-1;
	            buf[i]='\0';
	            fprintf(fsave,"%d. %s\n",k,buf);
	            if(!fcom) printf("%d. %s\n",k,buf);
	           }
	         ++k;
	        }
  	      break;
	      case 2:		/*include text(not rel.names)*/
	      if(cost())break;
	      if(j==1) break;
itex:
	      i=intext(buf,j==4? 1:0);
	      if(i)
	        {
	         i=n_incl(buf,i,buf+((i+3)>>2<<2),(int)j);
	         goto itex;
	        }
	      break;
	      case 3: 		/*delete text*/
	      if(cost())break;
	      if(j==1) break;
	      if((ii=oumenu(2,ref3,1))==0)goto tex2;
dnam:
	      switch(ii)
	        {
	         case 1:
	         if((i=intext(buf,0))==0)break;
	         if((i=namn(buf,i,buf+((i+3)>>2<<2),j))!=0)goto tndel;
	         oumes(17);
	        break;
	        case 2:
	        if(inpnum(&i)!='\n')break;
	        if(!i)break;
tndel:
	        n_del(i,buf,j);
	        goto dnam;
	       }
	  }
	goto tex2;
  }
dide()
  {
rdloop:
	if((numr=relna(buf,&i))==0)return;
	if(rlread(&p3,numr,buf))
	  {
	   if(!(p3.nowner==numow || numow==1))
	     {
	      oumes(16);
	      goto rdloop;
	     }
	  if(p3.rtype & 0x20)	/* global */
	  fidd(0);	            /* find and clear */
	  patli=(PATLI)(buf+1200);
	  i=1;
	  deltn(i,p3.ntup,buf,numr);
	  while(p3.nat--)
	    {
	     numatr(buf,patli,i++,numr);
	     if(!error1)
	       n_del(patli->annu,buf,(short)a_n);
	    }
	 }
   i=a_del(numr,buf,(short)t_l);
   j=a_del(numr,buf,(short)r_d);
   n=a_del(numr,buf,(short)a_l);
   m=r_del(numr,buf);
   q=n_del(numr,buf,(short)r_n);
   goto rdloop;
  }
apr(j)
 short j;
  {
	buf[1300]='\0';
	nnam(buf+1300,(short)200,buf,(short)a_n,patli->annu);
	fprintf(fsave,"%5d. %-20s -- %-16s(%3d), OFFSET:%d\n",
	  j,buf+1300,ref4[(int)(patli->atip-1)],
	  (int)patli->adli&255,patli->aoff);
	if(!fcom) printf("%5d. %-20s -- %-16s(%3d), OFFSET:%d\n",
	  j,buf+1300,ref4[(int)(patli->atip-1)],
	  (int)patli->adli&255,patli->aoff);
  }

edrw()
  {
	if((numr=relna(buf,&i))==0)return(0);
	if(!(rlread(&p3,numr,buf) && (p3.nowner==numow || numow==1)))
	  return(0);
	return(1);
  }
wrp3()			/* write p3 to disc */
  {
	if((i=tor(numr,buf))== -1)
	  {
	   fprintf(fsave,"NOT WRITTEN\n");
	   if(!fcom)printf("\nNOT WRITTEN");
	   return;
	  }
	movch(buf+i,&p3,(short)sizeof(struct relis));
	wrishr(curb,buf,size2b);
  }
eddi()
  {
	switch(i)
	 {
	  case 1:	/*net address*/
	  if(ncoms)
	    {
	     codial(&p3,numr,2);
	     wrp3();
	    }
	  break;
	  case 2:	/*subsystem*/
	  if(!nsubs)break;
	  printf("\nSUBSYSTEM:\n");
	  p3.nsub=tab_menu(nsubs,subs_name);
	  resyba();
	  redshr(1,bufk,sizeof(bufk));
	  wrp3();
	  break;
	  case 3:	/*owner*/
	  if(!now)break;
	  printf("\nOWNER-");
	  if(intext(buf,0))
	    if(i=fiow(now))
	      p3.nowner=i;
	  wrp3();
	  break;
	  case 4:	/*continuation*/
	  wrp3();
	  break;
	  case 5:	/*tuple name*/
	  ii=t_l;
	  k=t_n;
	  s=2;
	  goto ed_0;
	  case 6:	/*attr.name*/
	  k=a_n;
	  ii=a_l;
	  s=sizeof(struct atli);
ed_0:
	  printf("%s ",txt_0[i-5]);
	  if(inpnum(&j)!='\n') break;
 	  if(!j) break;
	  if(k==t_n && j>p3.ntup) break;
	  if(k==a_n && j>p3.nat) break;
	  printf("NEW NAME-");
	  if(!(r=intext(buf,0))) break;
	  if(!(r=n_incl(buf,r,buf+r,k))) break;
	  nini(buf,ii);
	  patc = (PATC)(buf+toc(buf,sizeof(struct atc),numr));
	  curb=begb+patc->alb+(patc->alo+s*(j-1))/sizeb;
	  j=(patc->alo+s*(j-1))%sizeb;
	  redshr(curb,buf,size2b);
	  if(k==t_n)
	    {
	     n= *(short *)(buf+j);	/* tuple old name number */
	     *(short *)(buf+j)=r;	/* new name number */
	    }
	  else
	    {
	     patli=(PATLI)(buf+j);
	     n=patli->annu;       	/* attribute old name number */
	     patli->annu=r;      	/* new name number */
	    }
	  wrishr(curb,buf,size2b);
	  n_del(n,buf,k);
	  goto ed_0;
	 }
  }
getli(a,b)
 short *a,*b;
  {
   char c;
     if(inpnum(a)=='\n')
       {
	*b = *a;
	goto tofil;
       }
     fscanf(fcom,"%d%c",&_a,&c);  *b=_a;
     if(c!='\n')
      while(fgetc(fcom)!='\n')
       ;
tofil:
      fprintf(fsave,"%d\n",*b);
      return;
  }
setpar()		/* param.'s for read/write data */
  {
   char c;
par1:
	i=2;
	if(!fcom)if((i=oumenu(2,ref11,1))==0)return(0);
	switch(i)
	  {
	   case 1:
	   n=m=q=1;
	   r=p3.third;
	   s=p3.ntup;
	   t=p3.nat;
	   break;
	   case 2:
	   if(fcom)
	     {
	      fprintf(fsave,"ATTRIBUTES  ");
	      getli(&q,&t);
	      if(q<=0 || q>t || t>p3.nat)return(0);
	      fprintf(fsave,"TUPLES  ");
	      getli(&m,&s);
	      if(m<=0 || m>s || s>p3.ntup)return(0);
	      fprintf(fsave,"PLANES  ");
	      getli(&n,&r);
	      if(n<=0 || n>r || r>p3.third)return(0);
	      return(1);
	     }
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
	   if(q<=0 || q>t || t>p3.nat)goto par1;
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
	  }
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
	      while(j<=t)
		{
		 if(numatr(buf,patli,j,numr)==0)goto erw;
		 cpt=buf+sizeb;
		 il=patli->atip;
		 il &= 255;		/* type of attr. */
		 kk=patli->adli;
		 kk &= 255;		/* length */
		 if(k)
		   {			/* write */
		    if(!fcom) printf("a=%d t=%d p=%d",j,ii,n);
		    fprintf(fsave,"a=%d t=%d p=%d",j,ii,n);
repea:
		    if(!fcom) printf(" %12s(%d) \n",ref4[il-1],kk);
		    fprintf(fsave," %12s(%d) \n",ref4[il-1],kk);
		    switch(il)
		      {
		       case 1:
		       if(inpnum(&i)!='\n')goto repea;
		       if(i<0 || i>255)goto repea;
		       *buf=i;
		       break;
		       case 2:
		       if(inpnum(&i)!='\n')goto repea;
		       *(short *)buf=i;
		       break;
		       case 3:
		       oumes(1);
		       if(fcom)
			 {
			  fist();
			  fscanf(fcom,"%ld",buf);
			  while(fgetc(fcom)!='\n')
			    ;
			  fprintf(fsave,"%d\n",*(int*)buf);
			 }
		       else
			 {
			  scanf("%d",buf);
			  if(getchar()!='\n') goto repea;
			 }
		       break;
		       case 4:
		       case 5:
		       if(!fcom) printf("enter float=");
		       if(fcom)
			 {
			  fist();
			  if(il==4)
			   {
			    fscanf(fcom,"%e",buf);
			    fprintf(fsave,"%e\n",*(float *)buf);
			   }
			  else
			   {
			    fscanf(fcom,"%le",buf);
			    fprintf(fsave,"%le\n",*(double *)buf);
			   }
			  while(fgetc(fcom)!='\n')
			    ;
			 }
		       else
			 {
			  if(il==4)
			    scanf("%e",buf);
			  else
			    scanf("%le",buf);
			  if(getchar()!='\n')goto repea;
			 }
		       break;
		       case 6:
		       i=0;
		       if(!fcom)printf("enter hex.(up to %3d letters)=",2*kk);
		       if(fcom)
			 {
			  fist();
			  fgets(cpt,100,fcom);
			  fprintf(fsave,"%s\n",cpt);
			 }
		       else
		         while((cpt[i++]=getchar())!='\n')
		           ;
		       hexbin(kk,buf,cpt);
		       break;
		       case 7:
		       for(i=0;i<512;i++)
			  buf[i]=0;
		       intext(buf,1);
		       break;
		       case 8:
		       if(!fcom)
			{
			 printf("REF.TO TEXT ");
			 inpnum(&i);
			}
		       else
			{
		         i=intext(buf,1);
		         if(i) i=n_incl(buf,i,buf+sizeb,t_f);
			}
		       *(short *)buf = i;
		       break;
		       case 9:		/* to hardware */
		       case 10:		/* to relation */
		       break;
		      }
		    if(rwelm(cpt,(char)1,ii,j,numr,n,buf)==0)
		      {
		       fprintf(fsave,"\nERR.ON WRITING");
		       if(!fcom) printf("\nERR.ON WRITING");
		       goto erw;
		      }
		   }
		 else
		   {			/* read */
		    if(rwelm(cpt,(char)0,ii,j,numr,n,buf)==0)goto erw;
		    for(i=0;i<sizeb;i++)
		      cpt[i]=0;
		    switch(il)
		      {
		       case 1:
		       sprintf(cpt,"%d",*buf&255);
		       break;
		       case 2:
		       sprintf(cpt,"%d",*(short *)buf&0xffff);
		       break;
		       case 3:
		       sprintf(cpt,"%d",*(int*)buf);
		       break;
		       case 4:
		       sprintf(cpt,"%g",*(float *)buf);
		       break;
		       case 5:
		       sprintf(cpt,"%lg",*(double*)buf);
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
		       case 7:
		       movch(cpt,buf,kk);
		       break;
		       case 8:
		       kk=0;
		       i= *(short *)buf;
		       if(i) kk=nnam(buf,sizeb,buf+sizeb,t_f,i);
		       buf[kk]=0;
		       sprintf(cpt,"%d ==> %s",i,buf);
		       break;
		       case 9:
		       sprintf(cpt,"%2x %2x %2x %2x",*buf,*(buf+1),
			 *(buf+2),*(buf+3));
		       break;
		       case 10:
		       sprintf(cpt,"%5d %5d %5d",*(short *)buf,
			 *(short *)(buf+2),*(int *)(buf+4));
		      }
		    fprintf(fsave,"a=%d t=%d p=%d %s(%d)= %-30s\n",
		      j,ii,n,ref4[il-1],kk,cpt);
		    if(!fcom) printf("\na=%-5d t=%-5d p=%-5d %14s(%3d)= %-30s",
	 	      j,ii,n,ref4[il-1],kk,cpt);
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
	   if(k>20)k=20;
	   movch(&pdd1.rena[0],buf,k);
	   if(fcom && for_fcom) printf("%s\n",buf);
	   while(k--)
	     {
	      j=0;
	      crc ^= buf[reln++];
	      if(crc & 0x80) j=genpol;
	      crc = crc<<1^j;
	     }
	   pdd1.ncrc=crc;
	   if(!(reln=namn(buf,*i,buf+((*i+3)>>2<<2),(short)r_n)))
	     {
	      if(tofind)
	      oumes(5);
	     }
	   else
	     if(!tofind)
	       oumes(2);
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
  }
intuna(j,buf)
 short j;
 char *buf;
  {
   short i;
	toin = (short *)&p3;
	while(j--)
	  {
intu:
	   while(!(i=intext(buf,0)))
		 ;
	   if((i=n_incl(buf,i,buf+((i+3)>>2<<2),t_n))==0)goto intu;
	   *toin++ = i;
	  }
	toin= (short *)&p3;
  }


cost()
  {
	if(now && (fcod&0x80)) return(0);
	if(!now) return(0);
	fprintf(fsave,"YOU HAVE NOT RIGHTS\n");
	if(!fcom) printf("YOU HAVE NOT RIGHTS\n");
	return(1);
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
  	  fprintf(fsave," %s",tomes[n]);
  	  if(!fcom) printf(" %s",tomes[n]);
  }
intext(buf,i)
 char *buf;
 short i;
  {
   short j,ii;
intx:
	oumes(0);
	j=0;
	if(fcom)
	  {
	   fist();
	   fgets(buf,520,fcom);
	   while(buf[j] && buf[j]!='\n')
	     if((buf[j++]&255)==255)
	       buf[j-1]='\n';
	   goto tyx;
	  }
	if(i==0)
	  while((buf[j]=getchar())!='\n')
	    j++;
	else
	  {
	   while((buf[j]=getchar())!='\002')	/* ctrl/b */
	     ++j;
	   if(getchar()!='\n')goto intx;
	  }
tyx:
	buf[j]='\0';
	fprintf(fsave,"%s\n",buf);
	if(*buf=='+')
	   for(ii=0;ii<=j;ii++)
	     buf[ii]=buf[ii+1]^0xa1;
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
	   fist();
	   fscanf(fcom,"%d",&_a);   *j=_a;
	   c=fgetc(fcom);
	   fprintf(fsave,"%d%c",*j,c);
	   return(c);
	  }
	scanf("%d",&_a);   *j=_a;
	c=getchar();
	fprintf(fsave,"%d%c",*j,c);
	return(c);
  }
fist()
  {
   char c;
next:
	c=fgetc(fcom);
	ungetc(c,fcom);
	switch (c)
	  {
	   case '[':
	   fgets(buf,520,fcom);
	   while((c=fgetc(fcom))!=']')
	     {
	      ungetc(c,fcom);
	      fgets(buf,520,fcom);
	     }
	   fgets(buf,520,fcom);
	   break;
	   case '.':
	   case ';':
	   case '$':
	   fgets(buf,520,fcom);
	   break;
	   case '=':
	   fgetc(fcom);
	   return(c);
	   case '!':
	   default:
	   return(c);
	  }
	goto next;
  }
oumenu(n,legptr,k)
 char *legptr[];
 short n,k;
  {
   char *p,**p1;
   short i,r,n1;
	p1=legptr;
	if(fcom)
	  {
	   j=0;
	   fist();
	   fgets(buf,520,fcom);
	   fprintf(fsave,"menu line=%s",buf);
	   while(buf[j]!='\n')
		 j++;
	   buf[j]=0;
	   for(j=0;j<n;j++)
	     if(!strcmp(buf,legptr[j]))return(j+1);
	   return(0);
	  }
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
	if(inpnum(&r)!='\n')goto i2;
	if(r <0 || r > n)goto i2;
i3:
	legptr=p1;
	if(r)
	  fprintf(fsave,"( %s )\n",legptr[r-1]);
	else
	  fprintf(fsave,"! exit from menu\n");
	return(r);
  }

