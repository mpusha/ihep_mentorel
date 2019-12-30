#include "ccydalib.h"
#include <stdio.h>
#include <syslog.h>
extern char ex_buf_[max_gb];
static struct FE_EC_reply {
	char func;
	char p_size[2];
	char off_r[2];
	char reply[2];
      } *to_rpl;
static int i;
static struct ws_rwr *to_hd;

typedef struct FE_EC_reply *TORPL;
gl_glos(ff,id_r)
 int ff;
 short id_r;
  {
   if(id_r < max_rel) return(0x8000);
   id_r -= max_rel;
   if(id_r >= max_rel) return(0x8000);
   cur_type=gtab[id_r].tab_inf.nexlis.nrr;
   to_hd=(TORWR)ex_buf_;
   to_hd->n_proc=my_type;
   to_hd->n_q=gtab[id_r].tab_inf.res1;  /* DB number */
   to_hd->func=ff;    /* 2 - 5 */
   to_hd->pac_tip=2;    /* packet type */
   if(i=sd_to_cmpt(ex_buf_,4,&gtab[id_r].parent))
     return(i);
cy_1:
   to_rpl=(TORPL)ex_buf_;
   if((i=rd_wt(ex_buf_,sizeof(ex_buf_),10)))
     return(i); /* time out */
   switch(to_rpl->func)
     {
      case 1:      /* reply */
      return(*(short *)to_rpl->reply);
      case 2:      /* not last data packet */
      case 3:      /* last data packet */
      return(0);   /* dtrdbl is done */
     }
   openlog("gl_glos:8070",LOG_CONS,LOG_USER);
   syslog(LOG_NOTICE,"to_rpl==> func=%d ",to_rpl->func);
   closelog();
   goto cy_1;
/*   return(0x8070); /* wrong function in reply */
  }

