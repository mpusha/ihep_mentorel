#include<stdio.h>
FILE *fpoi1,*fpoi2,*fpoi3;
char c,*wor,buf1[520];
int i,j,k,l,m;

main()
  {

	printf("INPUT FILE NAME:");
	inkb(buf1);
	if(!(fpoi1=fopen(buf1,"r")))
	  {
nofile:
	   printf("CAN'T OPEN %s\n",buf1);
	   exit();
	  }
	buf1[0]=0;
	if(!(fpoi2=fopen("/usr/users/ssuda/sys/bmpv.cnf","wb")))goto nofile;
	i=0;
	m=1;
	while((buf1[i++]=c=fgetc(fpoi1))!=EOF)
	  {
	   zcd(c);
	   ++m;
	  }
	i=0;
	while(buf1[i]!='\n')
	  ++i;
	buf1[i]=0;
	if(!(fpoi3=fopen(buf1,"wb"))) goto nofile;
c1:
	printf("\nCCYDA:");
	if(inpnum(&k)!='\n')goto c1;
c2:
	printf("\nVPV:");
	if(inpnum(&l)!='\n')goto c2;
	fputc(k,fpoi3);
	fputc(l,fpoi3);
	fclose(fpoi1);
	fclose(fpoi2);
	fclose(fpoi3);
	printf("\nEND OF TRANSFORMATION!!\n");
	exit();
  }
zcd(cd)
  int cd;
   {
	if(m&1)
	  {
	   fputc((cd>>4)&15,fpoi2);
	   fputc(cd&15,fpoi2);
	  }
	else
	  {
	   cd= ~cd;
	   fputc(cd&255,fpoi2);
	  }
   }
inkb(buf1)
  char *buf1;
   {
    int i,j,k;
	for(i=0;i<82;i++)
	  buf1[i]=0;
	i=0;
	while((buf1[i++]=getchar())!='\n')
	   ;
	buf1[i-1]=0;
	return(i);
   }
inpnum(j)
 int *j;
  {
   char c;
	printf("enter number:");
	*j=0;
	scanf("%d",j);
	c=getchar();
	return(c);
  }
