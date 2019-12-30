#include <stdio.h>
#include <termio.h>
FILE *stdout1;
extern char buf[],work[];
static struct termio *poi;
typedef struct termio *POI;
gtpswd()
  {
   short i=0;
   char j;
	stdout1=stdout;
	poi=(POI)work;
	if(ioctl(0,TCGETA,poi)) printf("ERROR ioctl == TCGETA\n");
	poi->c_lflag ^= ECHO;
	if(ioctl(0,TCSETA,poi)) printf("ERROR ioctl == TCSETA\n");
	while((j=getchar())!='\n')
	  buf[i++]=j;
	buf[i]=0;
	poi->c_lflag ^= ECHO;
	if(ioctl(0,TCSETA,poi)) printf("ERROR ioctl == TCSETA\n");
	return(i);
  }

