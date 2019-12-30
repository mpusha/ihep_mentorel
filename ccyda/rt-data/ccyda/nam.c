#include "common.h"

namstr(buf,n_mn,nstr,nrel)
 char *buf,*n_mn;
 short nstr,nrel;
  {
   int bsb,bsf,bl,lb;
   short iname,bt;
   char *buf1;
	razm(buf,(short)r_t,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	if((int)(--nstr)*2 > lb)
	  {
	   error1= -2;
	   return(0);
	  }
	bt += (int)nstr*2;
	bsf=bsb+bl+bt/length;
	if(bt>=length)bt %= length;
	redshr(bsf,buf,length);
	buf1=buf+bt;
	iname= *(short *)buf1;	/* name number */
	bt=nnam(n_mn,lennam,buf,(short)t_t,iname);
	if(!bt)
	  {
	   error1= -3;
	   return(0);
	  }
	error1=0;
	return(bt);
  }

numstr(buf,n_mn,ln,nrel)
 char *buf,*n_mn;
 short ln,nrel;
  {
   p3 pkatf;
   int bsb,bsf,bl,lb;
   short iname,bt,nsti,ncr;
   char *buf1;
	if(rlread(work,nrel,buf)==0)
	  {
	   error1= -4;
	   return(0);
	  }
	pkatf=(p3)work;
	ncr=pkatf->ntup;
	iname=namn(n_mn,ln,buf,(short)t_t);
	if(iname==0)
	  {
	   error1= -5;
	   return(0);
	  }
	razm(buf,(short)r_t,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	bsf=bsb+bl;
	redshr(bsf,buf,length);
	redshr(++bsf,buf+length,length);
	buf1=buf+bt;
	nsti=1;
	while(iname!= *(short *)buf1++)
	  {
	   ++nsti;
	   if(buf1-buf>=length)
		 {
		  movbuf(buf+length,buf,(short)length);
		  buf1 -= length;
		  ++bsf;
		  redshr(bsf,buf+length,length);
		 }
	   --ncr;
	   if(ncr <= 0)
		 {
		  error1= -6;
		  return(0);
		 }
	  }
	error1=0;
	return(nsti);
  }

numatr(buf,n_mn,natr,nrel)
 char *buf,*n_mn;
 short natr,nrel;
  {
   int bsb,bsf,bl,lb,pkatd,lbl;
   char *buf1;
   short lbt,bt;
	razm(buf,(short)r_a,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	pkatd=bt+sizeof(struct relatr)*(long)(--natr);
	lbl=pkatd/length;
	lbt=pkatd%length;
	if(pkatd<length)lbt=pkatd;
	bsf=bsb+lbl+bl;
	redshr(bsf++,buf,length);
	if(lbt+sizeof(struct relatr)>=length)
	  redshr(bsf,buf+length,length);
	buf1=buf+lbt;
	movbuf(buf1,n_mn,(short)sizeof(struct relatr));
	error1=0;
	return(sizeof(struct relatr));
  }

nifatr(buf,n_mn,namatr,ln,nrel)
 char *buf,*n_mn,*namatr;
 short ln,nrel;
  {
   pnrel pkate;
   int bsb,bsf,bl,lb;
   short bt,nst,iname;
   char *buf1;
	iname=namn(namatr,ln,buf,(short)a_t);
	if(iname==0)
	  {
	   error1= -5;
	   return(0);
	  }
	razm(buf,(short)r_a,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	bsf=bsb+bl;
	redshr(bsf,buf,length);
	redshr(++bsf,buf+length,length);
	buf1=buf+bt;
	pkate=(pnrel)buf1;
	nst=1;
	while(iname!=pkate->in)
	  {
	   ++nst;
	   ++pkate;
	   if((char *)pkate-buf >= length)
		 {
		  movbuf(buf+length,buf,(short)length);
		  pkate -= length;
		  redshr(++bsf,buf+length,length);
		 }
	   lb -= sizeof(struct relatr);
	   if(lb<=0)
	 	 {
		  error1= -7;
		  return(0);
		 }
	  }
	buf1=(char *)pkate;
	movbuf(buf1,n_mn,(short)sizeof(struct relatr));
	error1=0;
	return(nst);
  }

subfag(bsf,lsf,n)
 int *bsf,*lsf;
 short n;
  {
   pnamekt pkat;
	if(n<0 || n>r_t)
	  {
	   error1= -1;
	   return;
	  }
	pkat=(pnamekt)bufk;
	pkat += n;
	*bsf=pkat->begkat;
	*lsf=pkat->lenkat;
	error1=0;
  }

movbuf(bufout,bufin,lbl)
 char *bufout,*bufin;
 short lbl;
  {
	while(lbl--)
	  *bufin++ = *bufout++;
  }

razm(buf,df,nrel,bl,bt,lb,bsb)
 char *buf;
 short unsigned nrel;
 short df,*bt;
 int *bl,*lb,*bsb;
  {
   pkt1 pkatc;
   int bsf,lsf,lbl,pkatd;
   short lbt;
   char *buf1,buf2[sizeof(struct katbs)];
 	subfag(&bsf,&lsf,df);
	if(error1)return;
	*bsb=bsf;
	--nrel;
	pkatd=sizeof(struct strkat)+(int)nrel*sizeof(struct katbs);
	lbl=pkatd/length;
	lbt=pkatd%length;
	bsf += lbl;
	redshr(bsf,buf,length);
	buf1=buf+lbt;
	pkatc=(pkt1)buf1;
	if(lbt+sizeof(struct katbs) > length)
	  {
	   movbuf(buf1,buf2,(short)(length-lbt));
	   redshr(++bsf,buf,length);
	   movbuf(buf,buf2+length-lbt,(short)(lbt+sizeof(struct katbs)-length));
	   pkatc=(pkt1)buf2;
	  }
	*bl=pkatc->blk;
	*bt=pkatc->byte;
	*lb=pkatc->dim;
  }
