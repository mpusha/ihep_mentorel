#include "ccydalib.h"
short setdbl(base_num)
 short base_num;
  {
    if((base_num&0x7fff) >= num_db)
      return(0x8001);
    cur_db=base_num;   /* b0=1 for global */
    return(0);
  }
