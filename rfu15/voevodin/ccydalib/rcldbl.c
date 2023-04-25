#include "ccydalib.h"
short rcldbl(id_r)
 unsigned short id_r;
  {
   unsigned short i;
    if(id_r < max_rel)
      {
       if(i=idrel(id_r))return(i);
       to_rdf->rel_num=0;
       return(0);
      }
    else   /* global */
      {
       id_r -= max_rel;
       if(id_r >= max_rel)
         return(0x8000);
       memset(&gtab[id_r].id_parent,0,sizeof(struct g_main));
       return(0);
      }
  }
