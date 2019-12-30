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
static struct ws_rwr *to_hd;
typedef struct FE_EC_reply *TORPL;
struct relis *cel_rd();

short reddbl(stat,id_r)
 unsigned short stat,id_r;
  {
   unsigned short i;
    if(id_r < max_rel)
      {
       if(i=idrel(id_r)) return(i);
       if(!(!stat || stat==1)) return(0x8000);
       p3=cel_rd(cur_db,to_rdf->rel_num,r_l);
       p3->rstate=stat;
       wribl(cdb[cur_db],block,buf_,size2b);
       to_rdf->re_to_re.rstate=stat;
       return(0);
      }
    else    /* global */
      {
       id_r -= max_rel;
       if(id_r >= max_rel) return(0x8000);
       cur_type=gtab[id_r].tab_inf.nexlis.nrr;
       to_hd=(TORWR)ex_buf_;
       to_hd->n_proc=my_type;
       to_hd->n_q=gtab[id_r].tab_inf.res1;  /* DB number */
       to_hd->func=7;       /* 7 */
       to_hd->pac_tip=2;    /* packet type */
       to_hd->tbl_id=gtab[id_r].id_parent;
       to_hd->buf_size=stat;
       if(my_type != cur_type)
         preob16(&to_hd->buf_size,my_type,cur_type);
       if(i=sd_to_cmpt(ex_buf_,8,&gtab[id_r].parent))
         return(i);
cy_1:
       to_rpl=(TORPL)ex_buf_;
       if((i=rd_wt(ex_buf_,sizeof(ex_buf_),10)))
         return(i); /* time out */
       switch(to_rpl->func)
         {
          case 1:      /* reply */
          if(*(short *)to_rpl->reply==0)
            gtab[id_r].tab_inf.rstate=stat;
          return(*(short *)to_rpl->reply);
          case 2:      /* not last data packet */
          case 3:      /* last data packet */
          break;
         }
       openlog("reddbl:8070",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE,"to_rpl==> func=%d ",to_rpl->func);
       closelog();
       goto cy_1;
/*       return(0x8070); /* wrong function in reply */
      }
  }

