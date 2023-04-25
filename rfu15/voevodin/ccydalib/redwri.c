#include "ccydalib.h"
#include <stdio.h>
#include <syslog.h>
#include "u_pack.h"
#define max_gb_bl 8550
#define out_p 218

static int lj;
static unsigned short i,j,k,l,work[4];
int mx_offset1,mx_offset2,mx_ncopied;
short rw_dbl();
char ex_buf_[max_gb];
static struct FE_EC_head {
	char net_ad_len;
	char sockad[10];
	char func;	/* 0-dtrdbl,1-dtwdbl,b0+b1-line number */
	unsigned short p_size;
	char flag;
	char fl_proc;	/* processor number */
	short tbl_id;
	short buf_size;
	short coord[7];
      } *to_h;
static struct FE_EC_reply {
	char func;
	char p_size[2];
	char off_r[2];
	char reply[2];
      } *to_rpl;
static struct ws_rwr *to_hd; 

typedef struct FE_EC_head *TOHEAD;
typedef struct FE_EC_reply *TORPL;

short dtrdbl(addr,b_len,coord,id_r)
 char *addr;
 unsigned short b_len,id_r,*coord;
  {
    rw_fl=0;	/* read data */
    return(rw_dbl(addr,b_len,coord,id_r));
  }

short dtwdbl(addr,b_len,coord,id_r)
 char *addr;
 unsigned short b_len,id_r,*coord;
  {
    rw_fl=1;	/* write data */
    return(rw_dbl(addr,b_len,coord,id_r));
  }

short rw_dbl(addr,b_len,coord,id_r)
 char *addr;
 unsigned short b_len,id_r,*coord;
   {
   int b_len1=0,b_len2=0;
    if(!b_len)
      return(0x8000); 
    if(!coord[0] || !coord[1] || !coord[2] ||
       !coord[3] || !coord[4] || !coord[5])
         return(0x8000);
    if(id_r >= max_rel)
      {                 /* global table */
       id_r -= max_rel;
       if(id_r >= max_rel)
         return(0x8000);
       if(!gtab[id_r].id_parent || coord[6]>3 || !coord[6])
           return(0x8000);
       if(coord[0]>coord[1] || coord[2]>coord[3] || coord[4]>coord[5])
         return(0x8000);
       if(coord[1] > gtab[id_r].tab_inf.third || coord[3] > gtab[id_r].tab_inf.ntup || coord[5] > gtab[id_r].tab_inf.nat)
         return(0x8000);
       if(rw_fl && gtab[id_r].tab_inf.nowner)
         if(gtab[id_r].tab_inf.nsub != 1 && gtab[id_r].tab_inf.nsub !=
            gtab[id_r].tab_inf.nowner)
              return(0x8050);
       if(rw_fl && (gtab[id_r].tab_inf.rstate&1) == 0)
         return(0x8050);
       if(b_len >= max_gb_bl)
         return(0x80a0);
       switch (coord[6])
         {
          case 1:	/* parent */
          cur_type=gtab[id_r].tab_inf.nexlis.nrr;
          if(i=set_ws_rwr(addr,b_len,id_r,coord,gtab[id_r].id_parent)) 
            return(i);
          if(i=sd_to_cmpt(ex_buf_,j,&gtab[id_r].parent))
            return(i);
          break;

          case 2:	/* child */
	  if(!gtab[id_r].id_child)
            return(0x80f0);  /* not open in child */
          cur_type=gtab[id_r].tab_inf.nexlis.nrt;
          if(i=set_ws_rwr(addr,b_len,id_r,coord,gtab[id_r].id_child)) 
            return(i);
          gtab[id_r].child.sin_port=htons(5995); /* set port */
          i=sd_to_cmpt(ex_buf_,j,&gtab[id_r].child);
          gtab[id_r].child.sin_port=htons(5996); /* retrieve port */
          if(i)
            return(i);
          break;

          case 3:	/* micro */
	  if(!gtab[id_r].id_micro)
            return(0x80f0);  /* not open in micro */
          cur_type=gtab[id_r].tab_inf.nexlis.nra;
          to_h=(TOHEAD)(ex_buf_+7);
          ex_buf_[0]=gtab[id_r].BC;
          ex_buf_[1]=gtab[id_r].RT;
          ex_buf_[2]=8;               /* packet type 8 */
          memcpy(ex_buf_+3,"#RWR",4);
          to_h->fl_proc=my_type;
          to_h->func=0x80 | rw_fl; /* line 2 */
          to_h->tbl_id=gtab[id_r].id_micro;
          memcpy(to_h->coord,coord,sizeof(to_h->coord));
          to_h->buf_size=b_len;
          if(!rw_fl)  /* read table */
            to_h->p_size=sizeof(struct FE_EC_head);
          else  /* write to table */
            {
             to_h->p_size=b_len+sizeof(struct FE_EC_head);
             if(to_h->p_size > sizeof(ex_buf_)-7)
               to_h->p_size=sizeof(ex_buf_)-7;
             memcpy(ex_buf_+sizeof(struct FE_EC_head)+7,addr,
               to_h->p_size-sizeof(struct FE_EC_head));
            }
          j=to_h->p_size+7;
          if(my_type != cur_type)
            {
             preob16(&to_h->p_size,my_type,cur_type);
             preob16(&to_h->buf_size,my_type,cur_type);
             pr_coor(to_h->coord,my_type,cur_type);
            }
          if(i=sd_to_cmpt(ex_buf_,j,&gtab[id_r].child))
            return(i);
         }
       to_rpl=(TORPL)ex_buf_;
next_pckt:
       if(l=rd_wt(ex_buf_,sizeof(ex_buf_),20))
         return(l);
       switch(to_rpl->func)
         {
          case 1:      /* reply */
          return(*(short *)to_rpl->reply);
          case 2:      /* not last data packet on data read */
          if(rw_fl)
            {
             goto er_wr; /* wait reply on data write */
            }
          if((*(short *)to_rpl->off_r+ *(short *)to_rpl->p_size)>b_len)
            goto er_wr;
          memcpy(addr+ *(short *)to_rpl->off_r,
            to_rpl->reply,*(short *)to_rpl->p_size);
          goto next_pckt;
          case 3:      /* last data packet on data read*/
          if(rw_fl)
            {
             goto er_wr; /* wait reply on data write */
            }
          if((*(short *)to_rpl->off_r+ *(short *)to_rpl->p_size)>b_len)
            goto er_wr;
          memcpy(addr+ *(short *)to_rpl->off_r,
            to_rpl->reply,*(short *)to_rpl->p_size);
          return(0);   /* dtrdbl is done */
         }
er_wr:
       openlog("redwri:8070",LOG_CONS,LOG_USER);
       syslog(LOG_NOTICE,"len=%d+%d = %d rw_fl=%d to_rpl==> func=%d ",
         *(short *)to_rpl->off_r,*(short *)to_rpl->p_size,
           b_len,rw_fl,to_rpl->func);
       closelog();
       goto next_pckt;
/*       return(0x8070); /* wrong function in reply */
      }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*   HERE IS LOCAL TABLE ACCESS */
/*______________________________*/

    if(i=idrel(id_r)) return(i);
    p3= &to_rdf->re_to_re;
    if(rw_fl && p3->nowner)
      if(to_rdf->nus != 1 || to_rdf->nus != p3->nowner)
        return(0x8050);
    if(rw_fl && !(p3->rstate&1))
      return(0x8050);
    if(coord[0]>coord[1] || coord[2]>coord[3] || coord[4]>coord[5] ||
       coord[1]>p3->third || coord[3]>p3->ntup || coord[5]>p3->nat)
        return(0x8000);
    mx_offset1=mx_offset2=work[0]=work[1]=work[2]=work[3]=0;
    if(coord[4]==1 && coord[5]==p3->nat)
      if(coord[2]==1 && coord[3]==p3->ntup)
        {
         lj=p3->ntup*p3->lotup*(coord[1]-coord[0]+1);
         if(lj>max_rw)
           return(0x80a0);
         k=work[3]=lj;	/* data length */
        }
      else
        {
         lj=p3->lotup*(coord[3]-coord[2]+1);
         if(lj>max_rw)return(0x80a0);
	 work[2]=lj;      	/* data length inside plane */
         work[3]=coord[0];	/* current plane */
        }
    else
      {
       work[1]=fi_at(coord[4],work);	/* work[0]=attr.offset
                                      	   work[1]=attr.length */
       if(coord[4]!=coord[5])
         {
          i=fi_at(coord[5],&j);
	  work[1]=i+j-work[0];
         }                    	/* work[1]=data length inside tuple */
       work[2]=coord[2];   	/* current tuple */
       work[3]=coord[0];    	/* current plane */
      }
    return(rw_rw_dbl(addr,b_len,coord,id_r));
  }
set_ws_rwr(addr,b_len,id_r,coord,t_id)
 char *addr;
 unsigned short b_len,id_r,*coord;
 short t_id;
  {
   to_hd=(TORWR)ex_buf_;
   to_hd->n_proc=my_type;
   to_hd->n_q=gtab[id_r].tab_inf.res1;  /* DB number */
   if(!to_hd->n_q)
     {
      openlog("set_ws_rwr:8060",LOG_CONS,LOG_USER);
      syslog(LOG_NOTICE,"t_id=%d id_r=%d BD=%d ",t_id,id_r,
        gtab[id_r].tab_inf.res1);
      closelog();
      return(0x8060);
     }
   to_hd->func=rw_fl;    /* 0 or 1 */
   to_hd->pac_tip=2;    /* packet type */
   to_hd->tbl_id=t_id;
   memcpy(to_hd->coord,coord,sizeof(to_h->coord));
   to_hd->buf_size=b_len;
   if(!rw_fl)            /* read table */
     j=sizeof(struct ws_rwr);
   else                  /* write table */
     {
      j=b_len+sizeof(struct ws_rwr);
      if(j > sizeof(ex_buf_))
        j=sizeof(ex_buf_);
      memcpy(ex_buf_+sizeof(struct ws_rwr),addr,
        j-sizeof(struct ws_rwr));
     }
   if(my_type != cur_type)
     {
      preob16(&to_hd->buf_size,my_type,cur_type);
      pr_coor(to_hd->coord,my_type,cur_type);
     }
   return(0);
  }
pr_coor(crd,fr,to)
short crd[7],fr,to;
  {
   preob16(&crd[0],fr,to);
   preob16(&crd[1],fr,to);
   preob16(&crd[2],fr,to);
   preob16(&crd[3],fr,to);
   preob16(&crd[4],fr,to);
   preob16(&crd[5],fr,to);
   preob16(&crd[6],fr,to);
  }
rw_rw_dbl(addr,b_len,coord,id_r)
 char *addr;
 unsigned short b_len,id_r,*coord;
  {
    if(!b_len)
      return(0x8001);
    mx_ncopied=0;
    if(mx_offset2)
      {
       if(!mx_offset1)
         {
          k=mx_offset2&0x300;
          mx_offset1=mx_offset2=0;
          if(k == 0x100) goto by_attr;
          goto by_tup;
         }
       if( (mx_offset2 & 0xff) != rw_fl) return(0x8001);
       k=mx_offset1;
       mx_offset1=mx_offset2=0;
       goto entry_2;
      }
    if(work[0] || work[1])
      {
by_attr:
       k=work[1];
       block=(work[3]-1)*p3->ntup*p3->lotup+
             (work[2]-1)*p3->lotup+
              work[0];
      }
    else
     {
      if(work[2])
        {
by_tup:
         k=work[2];
         block=(work[3]-1)*p3->ntup*p3->lotup+
               (coord[2]-1)*p3->lotup;
        }
      else   	/* by planes */
        block=(coord[0]-1)*p3->ntup*p3->lotup;
     }
    block += to_rdf->dat_off; 	/* block= data offset */
    i=block%sizeb;
    block=to_rdf->dat_bl+block/sizeb;
    if(cur_db!=rw_db || block!=cur_bl)
      if((block-1)==cur_bl)
        i += sizeb;
      else
        {
         redbl(cdb[cur_db],block,rab,size2b);
	 rw_db=cur_db;
         cur_bl=block;
        }
entry_2:
    if(k>b_len)
      {
       mx_offset2= 0x300+rw_fl;  /* for last interval */
       mx_offset1=k-b_len;
       k=b_len;
      }
    b_len -= k;
oth_bl:
    if((i+k)<=size2b)
      j=0;
    else
      {
       j=i+k-size2b;
       k=size2b-i;
      }
    if(!rw_fl)
      {
       mx_ncopied += k;
       while(k--)
	 *addr++ = rab[i++];
      }
    else
      {
       while(k--)
         rab[i++]= *addr++;
       wribl(cdb[cur_db],cur_bl,rab,size2b);
      }
    if(j)
      {
       cur_bl += 2;
       redbl(cdb[cur_db],cur_bl,rab,size2b);
       block=cur_bl;
       i=0;
       k=j;
       goto oth_bl;
      }
    if(work[0] || work[1])
      if(++work[2]<=coord[3])
        goto bat;
      else
        if(++work[3]>coord[1])
          return(0);
        else
          {
           work[2]=coord[2];
           goto bat;
          }
    else
      if(work[2])
        if(++work[3]>coord[1])
          return(0);
        else
          goto btu;
      else
        return(0);
bat:
    if(b_len) goto by_attr;
    mx_offset2= 0x100;
    goto he_bce;
btu:
    if(b_len) goto by_tup;
    mx_offset2= 0x200;
he_bce:
    mx_offset2 += rw_fl;
    return(0);
  }
short mxrdbl(addr,b_len,coord,id_r)
 char *addr;
 unsigned short b_len,id_r,*coord;
  {
    rw_fl=0;	/* read data */
    return(rw_rw_dbl(addr,b_len,coord,id_r));
  }

short mxwdbl(addr,b_len,coord,id_r)
 char *addr;
 unsigned short b_len,id_r,*coord;
  {
    rw_fl=1;	/* write data */
    return(rw_rw_dbl(addr,b_len));
  }

