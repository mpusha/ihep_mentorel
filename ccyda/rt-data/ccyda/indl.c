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
extern int exten(),bufk[catdli];
extern short zz;
extern char *endr;
extern int er,begb,lasb,curb,lor,lorab,lowo;
extern short flag,in,nin;
extern struct ncat *p1;
extern struct cnam *p2;
extern struct atc *patc;
extern struct atli *patli;
extern struct relis *p3;
/*
subroutines of include&delete
 --------------------------------------------*/
n_incl(at,ts,buf,num)	/*include text */
  char *buf,*at;	/*at-text,ts-num.of bytes*/
  short ts;
  int num;		/*num=1,2,3,4*/
   {
   short i,k;
	if(num < r_n || num > t_f)return(0);
	nini(buf,(short)num);
	if((in=nfind(at,ts,buf))!=0)
	  {
	   ncal1(buf);
	   p2->nref++;
	   wrishr(curb,buf,sizeb);
	   return(in);		/*text number*/
	  }
	redshr(curb=begb,buf,sizeb);
	i=p1->noffr;
	lorab=p1->nfree;
	if(ts > 2)
	  {
	   lowo=ts-2;
	   movtod(buf+sizeb,at+2,lowo,num);
	   wrishr(begb,buf,sizeb);
	  }
	else
	  lorab=i=0;
	redshr(curb+1,buf+sizeb,sizeb);
	p2=(P2)(buf+sizeof(struct ncat));
	for(in=nin;in!=0;in--)
	  {
	   if(p2->nref==0) goto nexit;
	   ++p2;
	   if((char *)p2-buf>=sizeb)
		 {
		  movch(buf,buf+sizeb,(short)sizeb);
		  p2=(P2)((char *)p2-sizeb);
		  redshr(++curb+1,buf+sizeb,sizeb);
		 }
	  }
	k=incend((short)sizeof(struct cnam),buf);
	p2=(P2)(buf+k);
	redshr(curb,buf,size2b);
	if(curb==lowo && k+sizeof(struct cnam) >= sizeb)
	  {
	   exca(buf,(short)num);
	   p2=(P2)(buf+sizeof(struct ncat));
	   for(in=nin;in>0;in--)
		 {
		  ++p2->nbloc;
		  ++p2;
		  if((char *)p2-buf >= sizeb)
			{
			 wrishr(curb,buf,sizeb);
			 movch(buf,buf+sizeb,(short)sizeb);
			 p2=(P2)((char *)p2-sizeb);
			 redshr(++curb+1,buf+sizeb,sizeb);
			}
		 }
	  }
nexit: 
	p2->nref=1;
	p2->n2[0]=at[0];  p2->n2[1]=at[1];
	p2->nlon=ts;
 	p2->nbloc=lorab;
	p2->noffs=i;
	wrishr(curb,buf,size2b);
	return(nin-in+1);
   }
/*-------------------------------------------------------*/
n_del(nt,buf,num)		/* delete text */
 char *buf;
 short nt,num;
  { 
   short unsigned i,j;
   int tem;
	if(num < r_n || num > t_f)return(0);
	nini(buf,num);
	lorab=begb+p1->nfree;
	if((in=nt) <= 0)return(0);
	ncal1(buf);
	if(p2->nref==0)return(0);
	lor=lorab;
	p2->nref--;
	wrishr(curb,buf,sizeb);
	if(p2->nref!=0)return(0);
	lorab=begb+p2->nbloc;		/* far block */
	i=p2->noffs;			/* far offset */
	j=p2->nlon-2;			/* far length */
	redshr(begb,buf,sizeb);
	if(j==0 || j&0x8000)return(in);
	modfr((int)j,buf);
	curb=lorab;
	tem= (long)i+(long)j;
	lowo=curb+tem/sizeb;
	fcompr(buf,i,(short)(tem%sizeb) );
	redshr(curb=begb,buf,size2b);
	p2=(P2)(buf+sizeof(struct ncat));
	for(nin=p1->nnum;nin > 0;nin--)
	  {
	   if(p2->nlon > 2 && p2->nref != 0 &&(lorab-begb < p2->nbloc ||
			lorab-begb == p2->nbloc && i <= p2->noffs ))
	     {
		  p2->nbloc -= j/sizeb;
		  p2->noffs -= j%sizeb;
		  if(p2->noffs < 0)
			{
			 p2->noffs += sizeb;
			 p2->nbloc--;
			}
		 }
	   ++p2;
	   if((char *)p2-buf >= sizeb)
	     {
		  wrishr(curb,buf,sizeb);
		  movch(buf,buf+sizeb,(short)sizeb);
		  redshr(++curb+1,buf+sizeb,sizeb);
		  p2=(P2)((char *)p2-sizeb);
	     }
	  }
	wrishr(curb,buf,sizeb);
	return(in);
  }
/*----------------------------------------------------*/
r_incl(at,buf)		/* at-record of rel.list,buf=2*sizeb */
 char *at,*buf;
  {
	nini(buf,(short)r_l);
	p3=(P3)p2;
	while(in--)
	  {
	   if(p3->ntup==0)
		 {
		  redshr(curb+1,buf+sizeb,sizeb);
		  movch(p3,at,(short)sizeof(struct relis));
		  wrishr(curb,buf,size2b);
		  return(nin-in);
		 }
	   ++p3;
	   if((char *)p3-buf >= sizeb)
		 {
		  redshr(++curb,buf,sizeb);
		  p3=(P3)((char *)p3-sizeb);
		 }
	  }
	redshr(begb,buf,sizeb);
	movtod(buf+sizeb,at,(int)sizeof(struct relis),(short)r_l);
	curb=p1->nfree;		/* save new values */
	in=p1->noffr;
	redshr(begb,buf,sizeb);
	p1->nfree=curb;		/* retrieve */
	p1->noffr=in;
	++p1->nnum;
	wrishr(begb,buf,sizeb);
	return(p1->nnum);	/* return number */
  }
/*-------------------------------------------------------------*/
r_del(nr,buf)			/* nr-rel.number */
 char *buf;
 short nr;
  {
   short k;
	if((k=tor(nr,buf))== -1)return(0);
	p3=(P3)(buf+k);
	p3->ntup=0;
	wrishr(curb,buf,sizeb);
	return(nr);
  }
/*-------------------------------------------------------*/
a_incl(at,ts,buf,num)	/* at-what write,num=6,7,8 */
 char *at,*buf;
 int num;
 int ts;
  {
   short i,k;
	if(num < a_l || num > t_l)return(0);
	nini(buf,(short)num);
	lorab=p1->nfree;
	i=p1->noffr;
	movtod(buf+sizeb,at,ts,(short)num);	/* at to disc */
	wrishr(curb,buf,sizeb);
	patc=(PATC)p2;
	while(in--)
	  {
	   if(patc->alb==0)
	     {
		  redshr(curb+1,buf+sizeb,sizeb);
		  in=nin-in;
		  goto a_bce;
		 }
	   ++patc;
	   if((char *)patc-buf >= sizeb)
		 {
		  redshr(++curb,buf,sizeb);
		  patc=(PATC)((char *)patc-sizeb);
		 }
	  }
	k=incend((short)sizeof(struct atc),buf);
	in=p1->nnum;
	patc=(PATC)(buf+k);
	redshr(curb,buf,size2b);
	if(curb==lowo && k+sizeof(struct atc) >= sizeb)
	  {
	   exca(buf,(short)num);
	   patc=(PATC)(buf+sizeof(struct ncat));
	   for(k=p1->nnum-1;k > 0;k--)
		 {
		  ++patc->alb;
		  ++patc;
		  if((char *)patc-buf >= sizeb)
			{
			 wrishr(curb,buf,sizeb);
			 movch(buf,buf+sizeb,(short)sizeb);
			 patc=(PATC)((char *)patc-sizeb);
			 redshr(++curb+1,buf+sizeb,sizeb);
			}
		 }
	  }
a_bce:
	patc->alb=lorab;
	patc->alo=i;
	patc->ald=ts;
	wrishr(curb,buf,size2b);
	return(in);
  }
/*----------------------------------------------------*/
a_del(i,buf,num)		/* i-rel.num.,num-6,7,8 */
 char *buf;
 short unsigned i,num;
  {
   short unsigned k,m;
	if(num < a_l || num > t_l)return(0);
	nini(buf,num);
	if(i <= 0 || i > p1->nnum)return(0);
	lor=begb+p1->nfree;			/* end copy */
	lorab=sizeof(struct ncat)+(long)(i-1)*sizeof(struct atc);
	curb=begb+lorab/sizeb;
	redshr(curb,buf,size2b);
	patc=(PATC)(buf+lorab%sizeb);
	if(patc->alb==0)return(0);
	lasb=begb+patc->alb;
	k=patc->alo;
	lorab=patc->ald;
	patc->alb=0;				/* clear cat. */
	wrishr(curb,buf,sizeb);
	redshr(begb,buf,sizeb);
 	curb=lasb;
	modfr(lorab,buf);			/* new ref.to free */
	lowo=curb+((long)k+lorab)/sizeb;
	m=((long)k+lorab)%sizeb;
	fcompr(buf,k,m);
	cadec(buf,k);
	return(i);
  }
/*------------------------------------------------------*/
plan_a(nr,n,buf,ad)			/*nr=rel.num.,n=num.of planes,ad=0-inc.,1-dec. */
 short unsigned nr,n,ad;
 char *buf;
  {
	if(!contad(nr,n,buf,ad))return(0);
	lorab=(long)p3->ntup * (long)p3->lotup * (long)n;	/* num.of bytes */
	if(ad==1 && n >= p3->third)return(0);
	if(ad)
	  p3->third -= n;
	else
	  p3->third += n;
	wrishr(curb,buf,size2b);
	nini(buf,(short)r_d);
	compex(nr,buf,ad,(short)r_d);
	return(1);
  }
/*-------------------------------------------------*/
tupl_a(nr,n,buf,ad,at)	/*n-tuple num.,at-ref.to names,ad=0-inc.,1-dec. */
 short unsigned nr,n,ad,*at;
 char *buf;
  {
   int j;
   short i;
	if(!contad(nr,n,buf,ad))return(0);
	if(p3->third!=1)return(0);		/* inc/dec one plane only */
	if(ad==1 && n >= p3->ntup)return(0);
	j=(long)p3->lotup * (long)n;				/* num.of data bytes */
	if(ad)
	  {
	   deltn(p3->ntup-n+1,n,buf,nr);
	   nini(buf,(short)r_l);
	   p3=(P3)(buf+toc(buf,(short)sizeof(struct relis),nr));
	   p3->ntup -= n;
	  }
	else
	  p3->ntup += n;
	lorab=(long)n<<1;		/* num.of bytes */
	wrishr(curb,buf,size2b);
	nini(buf,(short)t_l);
	i=compex(nr,buf,ad,(short)t_l);
	if(!ad)
	  {
	   redshr(lowo,buf,size2b);
	   movch(buf+i,at,n<<1);
	   wrishr(lowo,buf,size2b);
	  }
	nini(buf,(short)r_d);
	lorab=j;
	compex(nr,buf,ad,(short)r_d);
	return(1);
  }
/*-------------------------------------------------*/
deltn(k,m,buf,nr)
 short k,m,nr;
 char *buf;
  {
   short i;
	while(m--)
	  {
	   if((i=namstr(buf,buf+1200,k++,nr)))
		 {
		  i=namn(buf+1200,i,buf,(short)t_n);
		  n_del(i,buf,(short)t_n);
		 }
	  }
  }
/*-------------------------------------------------*/
contad(nr,n,buf,ad)
 short nr,n,ad;
 char *buf;
  {
	if(ad < 0 || ad > 1)return(0);
	if(n <= 0)return(0);
	nini(buf,(short)r_l);
	if(nr <= 0 || nr > nin)return(0);
	p3=(P3)(buf+toc(buf,(short)sizeof(struct relis),nr));
	return(1);
  }
/*--------------------------------------------------*/
/* compress / extend lorab bytes relation number nr subfile ns */

compex(nr,buf,ad,ns)		/* lorab=number of bytes */
 short nr,ad,ns;
 char *buf;
  {
   short i,j;
	lor=begb+p1->nfree;
	flag=p1->noffr;
	patc=(PATC)(buf+toc(buf,(short)sizeof(struct atc),nr));
	lowo=patc->ald;
	if(ad)
	  {
	   lasb=begb+patc->alb;
	   patc->ald -= lorab;
	  }
	else
	  patc->ald += lorab;
	wrishr(curb,buf,size2b);
	curb=begb+patc->alb+lowo/sizeb;
	if((i=patc->alo+lowo%sizeb) >= sizeb)
	  {
	   ++curb;
	   i -= sizeb;
	  }
	lowo=curb;			/* lowo,i=old end */
	curb=begb+patc->alb+patc->ald/sizeb;
	if((in=patc->alo+patc->ald%sizeb) >= sizeb)
	  {
	   ++curb;
	   in -= sizeb;		/* curb,in=new end */
	  }
 	j=patc->alo;
	redshr(begb,buf,sizeb);
	if(ad)				/* decrease record */
	  {
	   modfr(lorab,buf);
	   fcompr(buf,in,i);
	   cadec(buf,j);
	  }
	else				/* increase record */
	  {
	   amodfr(buf);
	   dohole(buf+sizeb,i,ns);
	  }
	return(i);
  }
/*-----------------------------------------------*/
cadec(buf,k)
 short k;
 char *buf;
  {
	redshr(curb=begb,buf,size2b);
	patc=(PATC)(buf+sizeof(struct ncat));
	for(in=p1->nnum;in>0;in--)
	  {
	   if(patc->alb != 0 && (lasb-begb < patc->alb ||
		 lasb-begb == patc->alb && k < patc->alo))
		 {
		  patc->alb -= lorab/sizeb;
		  patc->alo -= lorab%sizeb;
		  if(patc->alo < 0)
			{
			 patc->alo += sizeb;
			 patc->alb--;
			}
		 }
	   ++patc;
	   if((char *)patc-buf >= sizeb)
		 {
		  wrishr(curb,buf,sizeb);
		  movch(buf,buf+sizeb,(short)sizeb);
		  redshr(++curb+1,buf+sizeb,sizeb);
		  patc=(PATC)((char *)patc-sizeb);
		 }
	  }
	wrishr(curb,buf,sizeb);
  }
/*----------------------------------------------------*/
exca(buf,num)
 char *buf;
 short num;
  {
	lasb=exten(buf,num,(short)1);
	rek();
	redshr(curb=begb,buf,sizeb);
	curb += p1->nfree++;
	lorab++;
	while(curb >= begb+p1->nclon)
	  {
	   redshr(curb,buf+sizeb,sizeb);
	   wrishr(curb+1,buf+sizeb,sizeb);
	   --curb;
	  }
	curb=begb;
	redshr(begb+1,buf+sizeb,sizeb);
	p1->nclon++;
  }
/*----------------------------------------------------*/
incend(k,buf)
 short k,*buf;
  {
	redshr(begb,buf,sizeb);
	lor=sizeof(struct ncat)+(long)p1->nnum*(long)k;
	lowo=begb+p1->nclon-1;
	++p1->nnum;
	wrishr(begb,buf,sizeb);
	curb=begb+lor/sizeb;
	return(lor%sizeb);
  }
/*----------------------------------------------------*/
movtod(buf,at,ts,num)			/* move to disc */
 char *at,*buf;
 short num;
 int ts;
  {
   int xxx;
   short i;
	redshr(xxx=p1->nfree+begb,buf,sizeb);
	if(ts < sizeb-p1->noffr)
	  {
	   movch(buf+p1->noffr,at,(unsigned short)ts);
	   p1->noffr += ts;
	   wrishr(xxx=p1->nfree+begb,buf,sizeb);
	  }
	else
	  {
	   movch(buf+p1->noffr,at,(unsigned short)sizeb-p1->noffr);
	   if(num != r_d) at += sizeb-p1->noffr;
	   ts -= sizeb-p1->noffr;
	   wrishr(xxx=p1->nfree+begb,buf,sizeb);
	   i=(ts+511)/sizeb;
	   if(++p1->nfree+i+begb >= lasb)
		 lasb=exten(buf,num,i+1);
	   rek();
	   while(ts >= sizeb)
		 {
		  ts -= sizeb;
		  movch(buf,at,(short)sizeb);
		  if(num!=r_d) at += sizeb;
		  wrishr(xxx=p1->nfree+begb,buf,sizeb);
		  ++p1->nfree;
		 }
	   if(ts)
		 {
		  movch(buf,at,(unsigned short)ts);
		  p1->noffr=ts;
		  wrishr(xxx=p1->nfree+begb,buf,sizeb);
		  }
	   else
		 p1->noffr=0;
	  }
  }
/*------------------------------------------------*/
fcompr(buf,k,m)				/* compress (curb,k) (lowo,m) to lor */
 char *buf;					/* lor,lowo,curb are set */
 short k,m;
  {
	redshr(curb,buf,sizeb);
	redshr(lowo,buf+sizeb,sizeb);
	movch(buf+k,buf+sizeb+m,(short)sizeb-m);
	k += sizeb-m;
	if(zz > 255)return(0);
	if(k >= sizeb)
	  {
	   wrishr(curb++,buf,sizeb);
	   k -= sizeb;
	   movch(buf,buf+sizeb,k);
	  }
/*	while(++lowo <= lor) */
	while(lor >= ++lowo)
	  {
	   redshr(lowo,buf+sizeb,sizeb);
	   movch(buf+k,buf+sizeb,(short)sizeb);
	   wrishr(curb++,buf,sizeb);
	   movch(buf,buf+sizeb,k);
	  }
	wrishr(curb,buf,sizeb);
  }
/*----------------------------------------*/
modfr(j,buf)
 int j;
 char *buf;
  {
	if(j>0)
	  {
	   p1->nfree -= j/sizeb;
	   p1->noffr -= j%sizeb;
	   if(p1->noffr < 0)
	     {
		  p1->noffr += sizeb;
		  p1->nfree--;
	     }
	   wrishr(begb,buf,sizeb);
	  }
  }
/*------------------------------------------*/
amodfr(buf)
 char *buf;
  {
	if(lorab > 0)
	  {
	   p1->nfree += lorab/sizeb;
	   p1->noffr += lorab%sizeb;
	   if(p1->noffr >= sizeb)
		 {
		  ++p1->nfree;
		  p1->noffr -= sizeb;
		 }
	   wrishr(begb,buf,sizeb);
	  }
  }
/*-------------------------------------------*/
dohole(buf,m,ns)		/* ns=subfile number */
 short ns,m;			/* lor,flag=beg.of free */
 char *buf;			/* lowo,m=beg.of hole */
  {				/* lorab=num.of bytes in hole */
   short k;			/* buf-sizeb=block of begb */
   int lbg,len;
	lbg=lowo;
	len=lorab;
	if(((lasb-lor)*sizeb-flag) <= lorab )
	  {
	   lasb=exten(buf,ns,(short)(lorab/sizeb+2-(lasb-lor)));
	   rek();
	  }
	curb=begb+p1->nfree;
	k=p1->noffr;
	if(zz > 255)return(0);
	if(lor==lowo && flag==m)return(0);	/* last rel. */
        cpshr(lowo,m,lor,flag,lorab);
/*  cpshr do this now
	redshr(lor--,buf,sizeb);
	if(curb==lowo)
	   amovch(buf+sizeb+k,buf+flag,flag-m);
	else
	  {
	   amovch(buf+sizeb+k,buf+flag,k);
	   if(flag >= k)
		 {
		  wrishr(curb--,buf+sizeb,sizeb);
		  k=flag-k;
		  amovch(buf+size2b,buf+k,k);
		  k=sizeb-k;
		 }
	   else
		 k -= flag;
	   while(lor >= lowo)
		 {
		  redshr(lor--,buf,sizeb);
		  amovch(buf+sizeb+k,buf+sizeb,(short)sizeb);
		  wrishr(curb--,buf+sizeb,sizeb);
		  amovch(buf+size2b,buf+sizeb,(short)(sizeb-k));
		 }
	  }
	redshr(lor+1,buf,sizeb);	/* retrieve buf 
	amovch(buf+sizeb+m,buf+m,m);
	if(k!=sizeb)
	  wrishr(curb,buf+sizeb,sizeb);
*/
	patc=(PATC)((buf -= sizeb)+sizeof(struct ncat));
	redshr((curb=begb)+1,buf+sizeb,sizeb);
	for(in=p1->nnum;in>0;in--)
	  {
   if(patc->alb != 0 && (lowo-begb < patc->alb || lowo-begb==patc->alb &&
		m <= patc->alo))
		 {
		  patc->alb += lorab/sizeb;
		  patc->alo += lorab%sizeb;
		  if(patc->alo >= sizeb)
			{
			 ++patc->alb;
			 patc->alo -= sizeb;
			}
		 }
	   ++patc;
	   if((char *)patc-buf >= sizeb)
		 {
		  wrishr(curb,buf,sizeb);
		  movch(buf,buf+sizeb,(short)sizeb);
		  redshr(++curb+1,buf+sizeb,sizeb);
		  patc=(PATC)((char *)patc-sizeb);
		 }
	  }
	wrishr(curb,buf,sizeb);
/* fill hole by 0 
	if(m)
	  {
	   redshr(lbg,buf,sizeb);
	   k=sizeb-m;
	   if(k>len) k=len;
	   len -= k;
	   while(k--)
	      buf[m++]=0;
	   wrishr(lbg++,buf,sizeb);
	  }
	for(k=0;k<sizeb;k++)
	  buf[sizeb+k]=0;
	while(len)
	  if(len>=sizeb)
	    {
	     wrishr(lbg++,buf+sizeb,sizeb);
	     len -= sizeb;
	    }
	  else
	    {
	     redshr(lbg,buf,sizeb);
	     while(len)
	       buf[--len]=0;
	     wrishr(lbg++,buf,sizeb);
	    }
cpshr do this now */
  }

