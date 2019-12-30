#include <stdio.h>
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

extern int bufk[catdli];
extern int er;
 char *endr;
 int begb,lasb,curb,lor,lorab,lowo;
 short flag,in,nin,zz;

 struct ncat *p1;
 struct cnam *p2;
 struct atc *patc;
 struct atli *patli;
 struct relis *p3;
/*
numbers <======> names
*/
namn(at,ts,buf,num)
 char *at,*buf;
 short ts,num;
  {
	if(num < r_n || num > t_f)return(0);
	nini(buf,num);
	return(nfind(at,ts,buf));
  }
/*----------------------------------*/
nnam(at,ts,buf,num,num1)
 char *at,*buf;
 short ts,num,num1;
  {
   int e;
	if(num < r_n || num > t_f)return(0);
	nini(buf,num);
	if(num1 < 1 || num1 > in)return(0);
	in=num1;
	ncal1(buf);
	if(p2->nref==0)return(0);
	redshr(curb+1,buf+sizeb,sizeb);
	at[0]=p2->n2[0];  at[1]=p2->n2[1];
	ts -= 2;
	if(ts <= 0 || (e=p2->nlon) <= 2)
	  {
	   at[e]='\0';
	   return(e);
	  }
	if(ts >= e-2)
	  ts=e-2;
	else
	  --ts;
	mfromd(at+2,ts,buf,begb+p2->nbloc,p2->noffs);
	at[ts+2]='\0';
	return(e);
  }
/*--------------------------------------*/
char *movb(c,d,f)		/* c-from,d-after last,f-beg.buf */
 char *c,*d,*f;
  {
   short i;
	if(flag=(i=d-c)&1)
	  ++f;
	while(i--)
	  *f++ = *c++;
	return(f);
  }
/*--------------------------------------*/
nfind(at,ts,buf)		/* p2,in,begb are setted */
 char *buf,*at;
 short ts;
  {
   int nnam=0;
   char *movb();
	while(in--)
	  {
	   ++nnam;
	   if(p2->nref != 0 && ts==p2->nlon && p2->n2[0]==at[0] &&
	   p2->n2[1]==at[1]
	   && cmps(at,ts,p2->nbloc+begb,p2->noffs))
			return(nnam);	/* text is found */
	   ++p2;
	   if(endr-(char *)p2 < sizeof(struct cnam))
		 {
		  redshr(++curb,endr=movb(p2,endr,buf),sizeb);
		  endr += sizeb;
		  p2=( P2)(buf+flag);
		 }
	  }
	return(0);			/* not found */
  }
/*-------------------------------------*/
stxor(num,buf)			/* xor state of rel. */
 char *buf;
 short num;
  {
	if((in=tor(num,buf))== -1)return;
	p3=( P3)(buf+in);
	p3->rstate ^= 1;
	wrishr(curb,buf,size2b);
  }
/*-------------------------------------*/
rlread(at,num,buf)		/* at=to where,num=rel.number */
 char *at,*buf;
 short num;
  {
	if((in=tor(num,buf))== -1)return(0);
	movch(at,buf+in,(short)sizeof(struct relis));
	return(sizeof(struct relis));
  }
/*-------------------------------------*/
tor(num,buf)
 char *buf;
 short num;
  {
	nini(buf,(short)r_l);
	if(num <= 0 || num > in)return(-1);
	return(toc(buf,(short)sizeof(struct relis),num));
  }
/*-------------------------------------*/
toc(buf,i,n)			/* i=size of struct,n=num.in catalog */
 char *buf;
 short i,n;
  {
   int lora;
	lora=sizeof(struct ncat)+(n-1)*i;
	curb=begb+lora/sizeb;
	redshr(curb,buf,size2b);
	return(lora%sizeb);
  }
/*--------------------------------------*/
cmps(at,ts,nbl,noff)	/* comp.string in mem. and disc */
 int nbl;
 short noff,ts;
 char *at;
  {
   char *to;
	redshr(nbl++,endr,sizeb);
	at += 2;
	to=endr+noff;
	if((ts -= 2) <= 0)return(1);
	while(ts--)
	  {
	   if(to-endr >= sizeb)
		 {
		  redshr(nbl++,endr,sizeb);
		  to=endr;
		 }
	   if(*to++ != *at++)return(0);	/* not equ. */
	  }
	return(1);						/* comp.O.K. */
  }
/*-----------------------------------*/
ncal1(buf)
 char *buf;
  {
	p2=( P2)(buf+toc(buf,(short)sizeof(struct cnam),in));
  }
/*-----------------------------------*/
movch(buf,buf1,dl)
 char *buf,*buf1;
 short dl;
  {
	while(dl--)
	  *buf++ = *buf1++;
  }
/*-----------------------------------*/
amovch(buf,buf1,dl)
 char *buf,*buf1;
 short dl;
  {
	while(dl--)
	  *--buf = *--buf1;
  }
/*-----------------------------------*/
nini(buf,num)		/* initialize */
 short num;
 char *buf;
  {
   short i;
	i=num<<1;
	begb=bufk[i];
	lasb=begb+bufk[i+1];
	redshr(curb=begb,buf,sizeb);
	endr=buf+sizeb;
	p1=( P1)buf;
	p2=( P2)(buf+sizeof(struct ncat));
	nin=in=p1->nnum;
  }
/*------------------------------------*/
mfromd(at,ts,buf,il,j)
 char *buf,*at;
 short ts,j;
 int il;
  {
	redshr(il++,buf,sizeb);
	if(sizeb-j > ts)
	  {
	   movch(at,buf+j,ts);
	   return;
	  }
	movch(at,buf+j,(short)sizeb-j);
	at += sizeb-j;
	ts -= sizeb-j;
	while(ts)
	  {
	   redshr(il++,buf,sizeb);
	   if(ts >= sizeb)
		 {
		  movch(at,buf,(short)sizeb);
		  at += sizeb;
		  ts -= sizeb;
		 }
	   else
		 {
		  movch(at,buf,ts);
		  ts=0;
		 }
	  }
  }
/*--------------------------------------*/
static short cusnum=1;	/* customer number for DB = landing.opp */
rek()
  {
	redshr(1,bufk,sizeof(bufk));
	zz=bufk[18];
	if(zz)
	  {
re1:
	   zz=bufk[18] += 1;
	   wrishr(1,bufk,sizeof(bufk));
	  }
	else
	  {
	   cstmer(0);
	   if(zz!=cusnum) goto re1;
	   zz=0;
	  }
  }
vucle()
  {

  }
