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
struct atc *cel_rd();

short atrdbl(addr,b_len,att_n,id_r)
 char *addr;
 unsigned short b_len,att_n,id_r;
  {
   unsigned short i;
    if(b_len < 4)
      return(0x8001);
    if(id_r < max_rel)
      {
       if(i=idrel(id_r))
         return(i);
       block=(att_n-1)*sizeof(struct atli);
       i=to_rdf->attr_of+block%sizeb;
       block=to_rdf->attr_bl+block/sizeb;
       if(i>=sizeb)
         {
          block++;
          i -= sizeb;
         }
       redbl(cdb[cur_db],block,buf_,size2b);
       patli=(PATLI)(buf_+i);
       for(i=0;i<4;i++)
         addr[i]= ((char*)patli)[i];
       if(b_len==4) return(0);
       return(getname(addr+4,b_len-4,patli->annu,a_n));
      }
    else    /* global */
      {
       id_r -= max_rel;
       if(id_r >= max_rel)
         return(0x8000);
       if(!att_n || att_n > gtab[id_r].tab_inf.nat)
         return(0x8000);
       cur_type=gtab[id_r].tab_inf.nexlis.nrr;
       to_hd=(TORWR)ex_buf_;
       to_hd->n_proc=my_type;
       to_hd->n_q=gtab[id_r].tab_inf.res1;  /* DB number */
       to_hd->func=10;       /* 10 */
       to_hd->pac_tip=2;     /* packet type */
       to_hd->tbl_id=gtab[id_r].id_parent;
       to_hd->buf_size=b_len;
       to_hd->coord[0]=att_n;
       if(my_type != cur_type)
         {
          preob16(&to_hd->buf_size,my_type,cur_type);
          preob16(&to_hd->coord[0],my_type,cur_type);
         }
       if(i=sd_to_cmpt(ex_buf_,10,&gtab[id_r].parent))
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
          break;
          case 3:      /* last data packet */
          i= *(short *)to_rpl->p_size;
          if(i>b_len)
            i=b_len;
          memcpy(addr,to_rpl->reply,i);
          return(i-4);
         }
       openlog("atrdbl:8070",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE,"to_rpl==> func=%d ",to_rpl->func);
       closelog();
       goto cy_1;
/*       return(0x8070); /* wrong function in reply */
      }
  }

