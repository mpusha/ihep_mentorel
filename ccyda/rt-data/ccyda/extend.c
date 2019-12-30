#include "common.h"

int size()
  {
   short i;
   int s;
   pnamekt pkat;
	pkat=(pnamekt)bufk;
	s=pkat->lenkat;
	for(i=1;i<=subfil;i++)
	  {
	   ++pkat;
	   s += pkat->lenkat;
	  }
	++s;
	return(s);
  }
  

int exten(buf,numsub,numblk)
 char *buf;
 short numsub,numblk;
  {
   short s;
   int s0,s1,sr;
   pnamekt pkat;
	redshr(b_k,bufk,lengkt);
	s =numsub;
	 ++s;
	pkat=(pnamekt)bufk;
	pkat->begkat += numblk;
	if(numblk<pkat->lenkat) pkat->lenkat -= numblk;
	else
	  {
	   s0=size()+numblk;
	   wrishr(s0,buf,length);
	  }
	while(--s)
	  ++pkat;
	s0=pkat->begkat+pkat->lenkat;
	pkat->lenkat += numblk;
	sr=pkat->begkat+pkat->lenkat;
	s=subfil-numsub; 
 	++s;
	while(--s)
	  {
	   ++pkat;
	   pkat->begkat += numblk;
	  }
	s1=pkat->begkat+pkat->lenkat-numblk;
	wrishr(b_k,bufk,lengkt);
	while(s1>=s0)
	  {
	   redshr(s1,buf,length);
	   wrishr(s1+numblk,buf,length);
	   --s1;
	  }
	return(sr);
  }
