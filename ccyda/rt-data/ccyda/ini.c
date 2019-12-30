#include "common.h"
extern int size();

int inikat(buf,block)
 int block;
 char *buf;
  {
   short i;
   int end=2l;
   pnamekt pkat;
	pkat=(pnamekt)bufk;
	for(i=1;i<=subfil;i++)
	  {
	   ++pkat;
	   pkat->begkat=end;
	   pkat->lenkat=block;
	   end += block;
	  }
	pkat=(pnamekt)bufk;
	pkat->begkat=end;
	pkat->lenkat=block;
	*(short *)(bufk+72)=0;
	wrishr(b_k,bufk,lengkt);
	end=size();
	wrishr(end,buf,length);
	inisub(buf);
	return(end);
  }

inisub(buf)
 char *buf;
  {
   short i;
   pnamekt pkat;
   pnkt1 pkat1;
   int blk;
	pkat=(pnamekt)bufk;
	for(i=1;i<=t_f;i++)
	  {
	   ++pkat;
	   blk=pkat->begkat;
	   pkat1=(pnkt1)buf;
	   pkat1->numrel=0;
	   pkat1->lon=1;
	   pkat1->freblk=1l;
	   pkat1->frebyt=0;
	   pkat1->katlon=sizeof(struct cnam);
	   wrishr(blk,buf,length);
	  }
	++pkat;
	blk=pkat->begkat;
	pkat1=(pnkt1)buf;
	pkat1->numrel=0;
	pkat1->lon=0;
	pkat1->freblk=0l;
	pkat1->frebyt=sizeof(struct strkat);
	pkat1->katlon=sizeof(struct relis);
	wrishr(blk,buf,length);
	for(i=1;i<=t_t;i++)
	  {
	   ++pkat;
	   blk=pkat->begkat;
	   pkat1=(pnkt1)buf;
	   pkat1->numrel=0;
	   pkat1->lon=1;
	   pkat1->freblk=1l;
	   pkat1->frebyt=0;
	   pkat1->katlon=sizeof(struct katbs);
	   wrishr(blk,buf,length);
	  }
  }
