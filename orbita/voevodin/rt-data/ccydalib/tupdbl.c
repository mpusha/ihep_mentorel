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

short tupdbl(addr,b_len,tup_n,id_r)
 char *addr;
 unsigned short b_len,tup_n,id_r;
  {
   short i,j;
    if(!b_len)
      return(0x8001);
    for(i=0;i<b_len;i++)
      addr[i]=0;
    if(id_r < max_rel)
      {
       if(i=idrel(id_r)) return(i);
       patc=cel_rd(cur_db,to_rdf->rel_num,t_l);
       i=patc->alo+(--tup_n<<1)%sizeb;
       block=to_cat[cur_db].rel_tup_b+patc->alb+(tup_n<<1)/sizeb;
       if(i>=sizeb)
         {
          block++;
          i -= sizeb;
         }
       redbl(cdb[cur_db],block,buf_,sizeb);
       i= *(short*)(buf_+i);	/* name number */
       if((j=nnam(&i,t_n))<0) return(0x8000);
                     	/* i=name length */
			/* j=offset in the buf_ */
       if(b_len<i)i=b_len;
       memcpy(addr,buf_+j,i);
       return(i);
      }
    else    /* global */
      {
       id_r -= max_rel;
       if(id_r >= max_rel)
         return(0x8000);
       if(!tup_n || tup_n > gtab[id_r].tab_inf.ntup)
         return(0x8000);
       cur_type=gtab[id_r].tab_inf.nexlis.nrr;
       to_hd=(TORWR)ex_buf_;
       to_hd->n_proc=my_type;
       to_hd->n_q=gtab[id_r].tab_inf.res1;  /* DB number */
       to_hd->func=9;       /* 9 */
       to_hd->pac_tip=2;    /* packet type */
       to_hd->tbl_id=gtab[id_r].id_parent;
       to_hd->buf_size=b_len;
       to_hd->coord[0]=tup_n;
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
         return(i); /* error */
       switch(to_rpl->func)
         {
          case 1:      /* reply */
          return(*(short *)to_rpl->reply);
          case 2:      /* not last data packet */
          break;
          case 3:      /* last data packet */
          i= *(short *)to_rpl->p_size;
          if(i > b_len)
            i=b_len;
          memcpy(addr,to_rpl->reply,i);
          return(i);
         }
       openlog("tupdbl:8070",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE,"to_rpl==> func=%d ",to_rpl->func);
       closelog();
       goto cy_1;
/*       return((short)0x8070); /* wrong function in reply */
      }
  }

