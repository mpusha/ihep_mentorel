#include "ccydalib.h"
#include <stdio.h>
struct relis *cel_rd();

short rlpdbl(addr,id_r)
 char *addr;
 unsigned short id_r;
  {
   unsigned short i;
   char *br;
    if(id_r < max_rel)
      {
       if(i=idrel(id_r)) return(i);
       br= (char*)&to_rdf->re_to_re;
       for(id_r=0;id_r<10;id_r++)
         addr[id_r]=br[id_r];
       return(0);
      }
    else    /* global */
      {
       id_r -= max_rel;
       if(id_r >= max_rel) return(0x8000);
       if(!gtab[id_r].id_parent) return(0x8000);
       br= (char*)&gtab[id_r].tab_inf;
       for(id_r=0;id_r<10;id_r++)
         addr[id_r]=br[id_r];
       return(0);
      }
  }

