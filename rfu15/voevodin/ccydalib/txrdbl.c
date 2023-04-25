#include "ccydalib.h"
short txrdbl(addr,b_len,tex_n,tab_id)
 char *addr;
 unsigned short b_len,tex_n,tab_id;
  {
    if(!(cur_db & 0x8000))
      return(getname(addr,b_len,tex_n,t_f));
    else    /* global table was opened */
      return(getname(addr,b_len,tex_n,t_f,tab_id));
  }
