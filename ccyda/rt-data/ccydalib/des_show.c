/*
	non standard use of next elements of struct relis:
		nsub ==> user,who opened the table
		nrr  ==> processor type of parent
		nrt  ==> processor type of child
		nra  ==> processor type of micro
		res1 ==> DB number
*/
#include "ccydalib.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>

void  show_desc(short j)
  {
   short i;
   i=j;
   if(i < max_rel || i < 0)
     goto ex_er;
   i -= max_rel;
   if(i>max_rel)
     {
ex_er:
      printf("show_desc: Index is wrong = %4x",j);
      return;
     }
   printf("%d => p=%d t=%d a=%d l=%d user=%d DB=%d\n         pr=%x ch=%x mc=%x BC=%d RT=%d\n\n",i,
     gtab[i].tab_inf.third,
     gtab[i].tab_inf.ntup,
     gtab[i].tab_inf.nat,
     gtab[i].tab_inf.lotup,
     gtab[i].tab_inf.nsub&255,
     gtab[i].tab_inf.res1,
     gtab[i].id_parent, 
     gtab[i].id_child, 
     gtab[i].id_micro, 
     gtab[i].BC, 
     gtab[i].RT); 
  }

