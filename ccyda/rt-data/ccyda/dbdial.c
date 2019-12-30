#include<stdio.h>
/* subroutines of dialogue */
extern FILE *fcom,*fsave;
oumes(n)
 short n;
  {
   static char *tomes[]={
	"enter text=",              	/*  0 */
	"enter decimal=",           	/*  1 */
	"\n ? there is relation\n",  	/*  2 */
	"\nNUMBER OF TUPLES ATTRIBUTES PLANES",/*  3 */
	"insuffisient dynamic memory\n",/*  4 */
	"\n ? no such relation\n",  	/*  5 */
	"\n ? parameter input error\n",	/*  6 */
	"\n ? rel.inf. read error\n",	/*  7 */
	"\n ? no such tuple\n",     	/*  8 */
	"\n ? name including error\n",	/*  9 */
	"[size]",                   	/* 10 */
	"\n ? repeate mode error\n",	/* 11 */
	"\nno such attribute\n",    	/* 12 */
	"\n[RELATION]",             	/* 13 */
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
   short j;
intx:
	oumes(0);
	j=0;
	if(fcom)
	  {
	   fgets(buf,100,fcom);
           while(buf[j])
             {
              if(buf[j]=='\n')
                buf[j]=0;
              ++j;
             }
           j=0;
	   while(buf[j]!='\n')
		 j++;
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
	if(fsave)
	  fprintf(fsave,"%s\n",buf);
	return(j);
  }
char inpnum(j)
 short *j;
  {
   char c;
   int jj;
	oumes(1);
	*j=0;
	if(fcom)
	  {
	   fscanf(fcom,"%d",&jj);
           *j=jj;
	   c=fgetc(fcom);
	   printf("%d%c",*j,c);
	   return(c);
	  }
	scanf("%d",&jj);
        *j=jj;
	c=getchar();
	if(fsave)
	  fprintf(fsave,"%d%c",*j,c);
	return(c);
  }
oumenu(n,legptr,k)
 char *legptr[];
 short n,k;
  {
   char *p;
   short i,j,m,r,n1,q;
	if(k > 10 || k < 1)return(0);
	q=j=0;
	r=1;
	n1=n;
	putchar('\n');
	for(i=1;n1>0;n1--)
	  {
	   m=j*(80/k)-q;
	   while(m--)
		 {
		  putchar(' ');
		  ++q;
		 }
	   p= *legptr;
	   if((*legptr)[0])
	     printf("%3d.%s",i,*legptr++);
	   q += 4;
	   while(*p++!='\0')
		++q;
	   if(++j==k && n1!=1)
		 {
		  q=j=0;
		  putchar('\n');
		  if(++r==16)
			{
			 r=78;
			 putchar('\n');
			 while(r--)
			  putchar('*');
			 putchar('\n');
i1:
			 r=0;
			 if(inpnum(&r)!='\n')goto i1;
			 if(r < 0 || r > n)goto i1;
			 if(r!=0)return(r);
			 r=1;
			}
		 }
	   ++i;
	  }
i2:
	r=0;
	putchar('\n');
	if(inpnum(&r)!='\n')goto i2;
	if(r <0 || r > n)goto i2;
	return(r);
  }
