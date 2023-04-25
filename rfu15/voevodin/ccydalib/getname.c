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
static int l;
static struct ws_rwr *to_hd;

typedef struct FE_EC_reply *TORPL;
short getname(addr,b_len,tex_n,s_n,id_r)
 char *addr;
 unsigned short b_len,tex_n,s_n,id_r;
  {
   short i,j,k;
    if(!b_len)
      return(0x8001);
    if(!(cur_db & 0x8000))
      {
       j=tex_n;
       if((i=nnam(&j,s_n))<0)return(0);
       if(j>b_len) j=b_len;
       for(k=0;k<j;k++)
         addr[k]=buf_[i+k];
       return(j);
      }
    else    /* global table was opened */
      {
       if(id_r < max_rel) return(0x8000);
       id_r -= max_rel;
       if(id_r >= max_rel) return(0x8000);
       cur_type=gtab[id_r].tab_inf.nexlis.nrr;
       to_hd=(TORWR)ex_buf_;
       to_hd->n_proc=my_type;
       to_hd->n_q=gtab[id_r].tab_inf.res1;  /* DB number */
       to_hd->func=6;    /* 6 - txrdbl */
       to_hd->pac_tip=2;    /* packet type */
       to_hd->tbl_id=(s_n<<8)|tex_n;    /* dictionary + text number */
       if(i=sd_to_cmpt(ex_buf_,6,&gtab[id_r].parent))
         return(i);
cy_1:
       to_rpl=(TORPL)ex_buf_;
       if((i=rd_wt(ex_buf_,sizeof(ex_buf_),10)))
         return(i); /* time out */
       switch(to_rpl->func)
         {
          case 1:         /* reply */
          return(*(short *)to_rpl->reply);
          case 2:         /* not last data packet */
          break;
          case 3:      /* last data packet */
          i= *(short *)to_rpl->p_size;
          if(i>b_len)
            i=b_len;
          memcpy(addr,to_rpl->reply,i);
          return(i);
         }
       openlog("getname:8070",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE,"to_rpl==> func=%d ",to_rpl->func);
       closelog();
       goto cy_1;
/*       return(0x8070); /* wrong function in reply */
      }
  }

