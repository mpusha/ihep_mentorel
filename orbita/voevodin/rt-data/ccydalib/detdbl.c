#include "ccydalib.h"
short detdbl()
  {
    if(!cur_db&0x8000 && cdb[cur_db])
      close(cdb[cur_db]);
    cdb[cur_db&0x7fff]=0;
  }
