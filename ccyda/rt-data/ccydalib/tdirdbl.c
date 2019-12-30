#include "ccydalib.h"
short tdirdbl(tab_id)
 short tab_id;
  {
    if(!(cur_db & 0x8000))
      {
       redbl(cdb[cur_db],to_cat[cur_db].tup_nam_b,buf_,sizeb);
       return( *(short*)buf_);
      }
   else     /* global */
     return(gl_glos(5,tab_id));
  }
