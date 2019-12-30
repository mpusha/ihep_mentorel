#include<stdio.h>
FILE *fpoi1,*fpoi2;
char c,wor,buf1[240];
int i,j,k,l,m;

main()
  {
	printf("INPUT FILE NAME:");
	inkb(buf1);
	if(!(fpoi1=fopen(buf1,"rb")))
	  {
nofile:
	   printf("CAN'T OPEN %s\n",buf1);
	   exit();
	  }
	printf("OUTPUT FILE NAME:");
	inkb(buf1);
	if(!(fpoi2=fopen(buf1,"wb")))goto nofile;
	m=1;
	while((c=fgetc(fpoi1))!=EOF)
	  {
	   if(m&1)
	     {
	      wor=fgetc(fpoi1);
	      fputc((c<<4)|wor,fpoi2);
	     }
	   else
	      fputc(~c,fpoi2);
	   ++m;
	  }
	fclose(fpoi1);
	fclose(fpoi2);
	printf("\nEND OF TRANSFORMATION!!\n");
	exit();
  }
inkb(buf1)
  char *buf1;
   {
    int i,j,k;
	for(i=0;i<82;i++)
	  buf1[i]=0;
	i=0;
	while((buf1[i]=getchar())!='\n')
	  ++i;
	buf1[i]=0;
	return(i);
   }
