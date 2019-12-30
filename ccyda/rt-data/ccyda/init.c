#include <stdio.h>
#include "libro.h"
extern int size(),exten(),inikat();
extern int inisub();
extern FILE *bptr;
char sysbas[]={"/usr/users/ssuda/sys/config dbm"};
main()
  {
   int l,block;
   int ns=7,nb=2;
	printf("\nENTER NUMBER OF BLOCKS:");
	scanf("%ld",&block);
	creshr(filenm);
	l=inikat(buf,block);
	exten(buf,ns,nb);
	l=size();
	printf("\nLENGTH =%ld\n",l);
	exit(0);
  }
