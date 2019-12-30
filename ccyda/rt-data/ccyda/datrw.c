#include "common.h"

rwstr(buf,pr,nst,num,nrel,plos,inf)
 char *buf,*inf,pr;
 short unsigned nst,num,nrel,plos;
  {
   p3 pkatf;
   int bsb,bsf,lbl,bl,lb;
   short lbt,bt,ncr,lcr,lsf,sost;
   char *buf1;
	if(rlread(work,nrel,buf)==0)
	  {
	   error1= -4;
	   return(0);
	  }
	pkatf=(p3)work;
	sost=pkatf->rstate & 1;
	ncr=pkatf->ntup;
	lcr=pkatf->lotup;
	if(nst+num-1 > ncr || nst<1 || nst>ncr)
	  {
	   error1= -8;
	   return(0);
	  }
	lsf=num*lcr;
	razm(buf,(short)r_d,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	lbl=(long)bt+(long)lcr*(nst-1)+(long)lcr*(long)ncr*(plos-1);
	bsf=bsb+bl+lbl/length;
	lbt=lbl%length;
	if(lbl<length)lbt=lbl;
	redshr(bsf,buf,length);
	buf1=buf+lbt;
	bt=lsf;
	if(pr==0)
	  {
	   while(lsf--)
		 {
		  *inf++ = *buf1++;
		  if(buf1-buf >= length)
			{
			 redshr(++bsf,buf,length);
			 buf1 -= length;
			}
		 }
	   error1=0;
	   return(bt);
	  }
	else
	  {
	   if(sost==0)return(0);
	   while(lsf--)
		 {
		  *buf1++ = *inf++;
		  if(buf1-buf >= length)
			{
			 wrishr(bsf,buf,length);
			 redshr(++bsf,buf,length);
			 buf1 -= length;
			}
		 }
	   wrishr(bsf,buf,length);
	   error1=0;
	   return(bt);
	  }
  }

rwatr(buf,pr,nst,nrel,plos,inf)
 char *buf,*inf,pr;
 short nst,nrel,plos;
  {
   pnrel pkate;
   p3 pkatf;
   int bsb,bsf,lbl,bl,lb;
   short unsigned lbt,bt,ncr,lcr,natr,smst,lgst,lgstb,sost;
   char *buf1;
	if(rlread(work,nrel,buf)==0)
	  {
	   error1= -4;
	   return(0);
	  }
	pkatf=(p3)work;
	sost=pkatf->rstate & 1;
	ncr=pkatf->ntup;
	natr=pkatf->nat;
	lcr=pkatf->lotup;
	if(nst>natr || nst<1)
	  {
	   error1= -9;
	   return(0);
	  }
	numatr(buf,work,nst,nrel);
	if(error1)return(0);
	pkate=(pnrel)work;
	lgst=pkate->lgn & 255;
	smst=pkate->sm;
	razm(buf,(short)r_d,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	lbl=bt+(long)smst+(long)lcr*ncr*(long)(plos-1);
	bsf=bsb+bl+lbl/length;
	lbt=lbl%length;
	if(lbl<length)lbt=lbl;
	redshr(bsf,buf,length);
	buf1=buf+lbt;
	lgstb=lgst;
	bt=ncr*lgst;
	if(pr==0)
	  {
	   while(ncr--)
		 {
		  while(lgstb--)
			{
			 *inf++ = *buf1++;
			 if(buf1-buf >= length)
			   {
				redshr(++bsf,buf,length);
				buf1 -= length;
			   }
			}
		  lgstb=lgst;
		  lbl=buf1+lcr-buf;
		  if(lbl>=length)
			{
			 bsf += lbl/length;
			 buf1=lbl%length+buf;
			 if(lbl<length) buf1=lbl+buf;
			 redshr(bsf,buf,length);
			}
		 }
	   error1= 0;
	   return(bt);
	  }
	else
	  {
	   if(sost==0)return(0);
	   while(ncr--)
		 {
		  while(lgstb--)
			{
			 *buf1++ = *inf++;
			 if(buf1-buf >= length)
			   {
				wrishr(bsf,buf,length);
				redshr(++bsf,buf,length);
				buf1 -= length;
			   }
			}
		  lgstb=lgst;
		  lbl=buf1-buf+lcr;
		  if(lbl >= length)
			{
			 wrishr(bsf,buf,length);
			 bsf += lbl/length;
			 buf1=buf+lbl%length;
			 if(lbl<length)buf1=buf+lbl;
			 redshr(bsf,buf,length);
			}
		 }
	   wrishr(bsf,buf,length);
	   error1=0;
	   return(bt);
	  }
  }

rwelm(buf,pr,nst,iatr,nrel,plos,inf)
 char *buf,*inf,pr;
 short unsigned nst,iatr,nrel,plos;
  {
   pnrel pkate;
   p3 pkatf;
   int bsb,bsf,lbl,bl,lb;
   short lbt,bt,ncr,lcr,natr,smst,lgst,sost;
   char *buf1;
	if(rlread(work,nrel,buf)==0)
	  {
	   error1= -4;
	   return(0);
	  }
	pkatf=(p3)work;
	sost=pkatf->rstate & 1;
	ncr=pkatf->ntup;
	natr=pkatf->nat;
	lcr=pkatf->lotup;
	if(nst>ncr || nst<1)
	  {
	   error1= -8;
	   return(0);
	  }
	if(iatr>natr || iatr<1)
	  {
	   error1= -9;
	   return(0);
	  }
	numatr(buf,work,iatr,nrel);
	if(error1)return(0);
	pkate=(pnrel)work;
	lgst=pkate->lgn & 255;
	smst=pkate->sm;
	razm(buf,(short)r_d,nrel,&bl,&bt,&lb,&bsb);
	if(error1)return(0);
	lbl=bt+(long)smst+lcr*(long)(nst-1)+(long)lcr*(long)ncr*(plos-1);
	bsf=bsb+bl+lbl/length;
	lbt=lbl%length;
	if(lbl<length) lbt=lbl;
	redshr(bsf,buf,length);
	buf1=buf+lbt;
	bt=lgst;
	if(pr==0)
	  {
	   while(lgst--)
		 {
		  *inf++ = *buf1++;
		  if(buf1-buf>=length)
			{
			 redshr(++bsf,buf,length);
			 buf1 -= length;
			}
		 }
	   error1=0;
	   return(bt);
	  }
	else
	  {
	   if(sost==0)return(0);
	   while(lgst--)
		 {
		  *buf1++ = *inf++;
		  if(buf1-buf >= length)
			{
			 wrishr(bsf,buf,length);
			 redshr(++bsf,buf,length);
			 buf1 -= length;
			}
		 }
	   wrishr(bsf,buf,length);
	   error1=0;
	   return(bt);
	  }
  }
