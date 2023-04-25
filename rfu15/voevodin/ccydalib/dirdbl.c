#include "ccydalib.h"
short dirdbl(tab_id)
 short tab_id;
  {
    if(!(cur_db & 0x8000))
      {
       redbl(cdb[cur_db],to_cat[cur_db].text_b,buf_,sizeb);
       return( *(short*)buf_);
      }
   else     /* global */
     return(gl_glos(3,tab_id));
  }
