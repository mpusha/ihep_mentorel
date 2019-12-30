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
#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 1550
#include "typdef.h"
 int bufk[catdli];
 int er,cmdfl;
FILE *fcom=0,*fsave=0;

static struct ncat *p1,*p01;
static struct cnam p2;
static struct atc patc6,patc7,*patc,patc8;
static  struct atli *patli;
static struct relis p3;

static char *buf1,*toc;
static char *ref0[]={""};

static char *ref1[]={
		"R_N",
		"A_N",
		"T_N",
		"T_F",
		"R_L",
		"A_L",
		"R_D",
		"T_L"
	   };
static short int ref2[]={
		sizeof(struct cnam),
		sizeof(struct cnam),
		sizeof(struct cnam),
		sizeof(struct cnam),
		sizeof(struct relis),
		sizeof(struct atc),
		sizeof(struct atc),
		sizeof(struct atc)
	   };
static char *ref3[]={
		"IN THE BEGINNING",
		" "
		};
static char *ref4[]={
		"IN THE END",
		" "
		};
static short int j;
static char firec[sizeof(struct ncat)<<3],bb[100];
static int sumbl[8];
static short int sumof[8];
extern short int bptr;
extern short int zz;
char sysbas[]={"config.dbm"};
main()
  {
   short int i,k;
   int block;
	printf("\nBASE NAME-");
	if(intext(bb,(short)0)==0)
         {
	  er=opnshr("base.dat")!=0;
         }
	else
         {
	  er=opnshr(bb);
         }
	if(er)
	  {
	   printf("\nERR.OPEN=%x\n",er);
	   exit(0);
	  }
	rek();
	for(i=0;i<8;i++)
	  sumbl[i]=sumof[i]=0;

/*  test central catalog  */

	for(i=2;i<=16;i+=2)
	  {
	   j=(i==16)?0:i+2;
	   if(bufk[i]+bufk[i+1]!=bufk[j])
	     {
		  printf("\nER.SUBFILE %s\n",ref1[(i>>1)-1]);
		  exit(0);
		 }
	  }
	toc=buf1=malloc(size2b*5);	/* 10 blocks */
	if(!toc)printf("\n NO MEMORY\n");
	if(zz)
	  {
	   sumbl[0]=0;
	   while(bufk[1]--)
	     redshr(1,buf1,sizeb);
	   printf("\n END OF TEST!! \n");
	   exit(0);
	  }

/* test first record of subfile */

	j=0;
	for(i=2;i<=16;i+=2)
	  {
	   p1=( P1)toc;
	   redshr(bufk[i],toc,sizeb);
	   k=(toc-buf1)/sizeb;
	   if((int)p1->nclon > p1->nfree)
	     printf("\nER.NCAT %s ==> CATALOG LENGTH & FREE\n",ref1[k]);
	   if(i!=10)
		 if((p1->nnum*p1->nslon+sizeb-1)/sizeb > p1->nclon ||
			 p1->nclon >= bufk[i+1] || p1->nfree >= bufk[i+1] ||
			 p1->nfree < 0 || p1->noffr >= sizeb)
			printf("\nER.NCAT %s\n",ref1[k]);
	   if(p1->nslon != ref2[j])
		 printf("\nER.NUM.OF BYTES IN CAT.ELEM. %s=%d\n",ref1[k],p1->nslon);
	   movch(firec+j*sizeof(struct ncat),toc,sizeof(struct ncat));
	   ref2[j]=sizeof(struct ncat);
	   toc += sizeb;
	   ++j;
	  }
	p1=( P1)buf1;
	k=p1->nnum;
	p1=( P1)(buf1+(sizeb<<2));
	for(i=4;i<8;i++)
	  {
	   if(p1->nnum!=k)
	     printf("\nER.NUM.OF RELATIONS R_N=%d %s=%d\n",k,ref1[i],p1->nnum);
	   bufk[(i<<1)+3]=bufk[(i<<1)+2];
	   p1=( P1)((char *)p1+sizeb);
	  }
	j=1;	/* current rel.number */

/* test for all relations */

	while(k--)
	  {
	   p1 = ( P1)firec;
	   nexcat();	/* fill structs p2,p3,patc6-patc8 */
	   if((p2.nref==0 &&(p3.ntup!=0 || patc6.alb!=0 || patc7.alb!=0 ||
		   patc8.alb!=0)) || (p2.nref!=0 &&(p3.ntup==0 || patc6.alb==0 ||
		   patc7.alb==0 || patc8.alb==0)))
	printf("\nEMPTY ELEMENT(%d):\n   R_N=%d R_L=%d A_L=%ld R_D=%ld T_L=%ld\n"
			,j,p2.nref,p3.ntup,patc6.alb,patc7.alb,patc8.alb);
	   if(p2.nref==0) continue;

/* test block&offset for text subfiles */

	   tblof(0);

/* test reference,length,catalog of subfiles 6,7,8 */

	   ablof(&patc6,5,(int)p3.nat*(int)sizeof(struct atli));
	   ablof(&patc7,6,(int)p3.ntup*p3.lotup*(int)p3.third);
	   ablof(&patc8,7,(int)p3.ntup<<1);

/* test catalog for text subfiles */

	   for(i=0;i<4;i++)
		 {
		  if(p1->nnum*sizeof(struct cnam) > p1->nclon*sizeb)
			printf("\nCATALOG %s\n",ref1[i]);
		  ++p1;
		 }
	   if(p2.nlon-2 > 0)
		 summa(0,(int)(p2.nlon-2));

/* test length of tuple and sum of attr.lengths */

	   i=0;
	   toc=buf1+sizeb;
	   block=bufk[13]+patc6.alb;
	   patli=( PATLI)(toc+patc6.alo);
	   redshr(block,toc,size2b);
	   while(p3.nat--)
	     {
		  i += ((short)patli++->adli)&255;
		  if((char *)patli-toc >= sizeb)
			{
			 patli=( PATLI)((char *)patli-sizeb);
			 movch(toc,toc+sizeb,sizeb);
			 redshr(++block+1,toc+sizeb,sizeb);
			}
		 }
	   if(i!=p3.lotup)
	     printf("\nATTR.SUM(%d)=%d %d\n",j,p3.lotup,i);
	   ++j;			/* to next relation */
	  }

/* test sum of records and begin of free for subfiles 0,6,7,8 */

	for(i=0;i<8;i++)
	  {
	   if(i!=0 && i<5) continue;
	   p1=( P1)(firec+i*sizeof(struct ncat));
	   if(p1->nfree!=p1->nclon+sumbl[i] || p1->noffr!=sumof[i])
		 printf("\nRECORD SUM %s=%ld -> %d %ld => %d\n",
		   ref1[i],p1->nfree,p1->noffr,sumbl[i],sumof[i]);
	  }
	close(bptr);
	printf("\n END OF TEST!!! \n");
  }
nexcat()
  {
	rrecor(&p2,sizeof(struct cnam),1);
	rrecor(&p3,sizeof(struct relis),5);

	rrecor(&patc6,sizeof(struct atc),6);
	contig(&patc6,6,bufk[13]);

	rrecor(&patc7,sizeof(struct atc),7);
	contig(&patc7,7,bufk[15]);

	rrecor(&patc8,sizeof(struct atc),8);
	contig(&patc8,8,bufk[17]);
  }
tblof(n)
 int n;
  {
	if(p2.nbloc<0 || p2.nbloc>p1->nfree || p2.noffs>=sizeb)
	  printf("\nREFERENCE %s(%d)=%ld -> %d\n",ref1[n],j,p2.nbloc,p2.noffs);
  }
summa(n,dl)
 int n;
 int dl;
  {
	sumbl[n] += dl/sizeb;
	sumof[n] += dl%sizeb;
	if(sumof[n]>=sizeb)
	  {
	   sumof[n] -= sizeb;
	   sumbl[n]++;
	  }
  }
rrecor(at,ts,n)
				/* at-struct address,ts-length of struct,n-subfile number */
 char *at;
 int ts,n;
  {
   short int i;
   char *jk;
	jk=buf1+sizeb*(n-1);
	i=ref2[n-1];		/* i=offset */
	while(ts--)
	  {
	   *at++ = jk[i++];
	   if(i>=sizeb)
		 {
		  i=0;
		  redshr(++bufk[n<<1],jk,sizeb);
		 }
	  }
	ref2[n-1]=i;		/* new offset */
  }
ablof(pnt,n,m)
 struct atc *pnt;
 int m,n;
  {
	p1=( P1)(firec+n*sizeof(struct ncat));
	if(pnt->alb>p1->nfree || pnt->alo>=sizeb  || pnt->alb<0 || pnt->alo<0)
	  printf("\nREFERENCE %s(%d)=%ld -> %d\n",ref1[n],j,pnt->alb,pnt->alo);
	if(pnt->ald!=m)
	  printf("\nRECORD LENGTH %s(%d)=%ld %ld\n",ref1[n],j,m,pnt->ald);
	if(p1->nnum*sizeof(struct atc) > p1->nclon*sizeb)
	  printf("\nCATALOG %s\n",ref1[n]);
	p1=( P1)firec;
	summa(n,pnt->ald);
  }
contig(poin,n,bl)
 struct atc *poin;
 int n;
 int bl;
  {
   short int i,k,l,m,ox;
   char *jk;
   int blen,rlen;
	if(poin->alb==0)return;
	k=l=0;
	jk=buf1+(size2b<<2);	/* after 8 blocks */
	patc=( PATC)(jk+sizeof(struct ncat));
	p01=( P1)(firec+(n-1)*sizeof(struct ncat));
	i=p01->nnum;
	redshr(bl,jk,size2b);
	bl += 2;
	if(poin->alb==p01->nclon && poin->alo==0)k=1;	/* first in subfile */
	blen=poin->ald+poin->alo;
	m=blen%sizeb;	/* data end offset */
	blen=blen/sizeb+poin->alb;	/* data end block */
	if(blen==p01->nfree && m==p01->noffr)l=1;	/* last in subfile */
	while(i--)
	  {
	   if(k>0 && l>0)return;
	   if(k==0)
		 {
		  rlen=patc->ald+patc->alo;
		  ox=rlen%sizeb;
		  rlen=rlen/sizeb+patc->alb;
		  if(poin->alb==rlen && poin->alo==ox)k=1;
		 }
	   if(l==0)
		 if(blen==patc->alb && m==patc->alo)l=1;
	   ++patc;
	   if((char *)patc > (jk+sizeb))
		 {
		  patc =( PATC)((char *)patc - sizeb);
		  movch(jk,jk+sizeb,sizeb);
		  redshr(bl++,jk+sizeb,sizeb);
		 }
	  }
	if(k==0 || l==0)
	  printf("\nR_N=%d SUBFILE=%s HOLE OR OVERLAP %s %s\n",j,ref1[n-1],
		ref3[k],ref4[l]);
   }
