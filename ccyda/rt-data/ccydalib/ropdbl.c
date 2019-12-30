#include "ccydalib.h"
extern char *cel_rd();
extern short n_tup,l_tup;
short ropdbl(rel_n,use_n)
 char *rel_n,*use_n;
  {
   unsigned short j,i=0;
    for(j=0;j<max_rel;j++)
      if(rtab[j].rel_num==0)goto ec_fr;
er_op1:
    return(0x8010);
ec_fr:
    if(*use_n)
      {
       if(!set_rel(now))return(0x8000);
       if(!(i=fi_tup(0,use_n,n_tup,l_tup,to_cat[0].rel_dat_b+patc->alb,
          patc->alo))) return(0x8020);
      }
    rtab[j].nus=i;	/* owner number */
    rtab[j].ndb=cur_db;
    if(!cur_db && i!=1) return(0x8060);
    if(!(i=namn(rel_n,cur_db,r_n))) return(0x8040);
    rtab[j].rel_num=i;
    patc=(PATC)cel_rd(cur_db,i,r_d);
    rtab[j].dat_bl=to_cat[cur_db].rel_dat_b+patc->alb;
    rtab[j].dat_off=patc->alo;
    rtab[j].dat_len=patc->ald;
    patc=(PATC)cel_rd(cur_db,i,a_l);
    rtab[j].attr_bl=to_cat[cur_db].rel_at_b+patc->alb;
    rtab[j].attr_of=patc->alo;
    rtab[j].attr_len=patc->ald;
    use_n=cel_rd(cur_db,i,r_l);
    p3= &rtab[j].re_to_re;
    rel_n= (char *)p3;
    for(n_tup=0;n_tup<sizeof(struct relis);n_tup++)
      rel_n[n_tup]=use_n[n_tup];   /* copy struct relis */
    if(p3->rtype&1)
      {
       redbl(cdb[cur_db],rtab[j].attr_bl,buf_,sizeb);
       patli=(PATLI)(buf_ + rtab[j].attr_of);
       p3->nsub=patli->adli;  /* attr.length for matrix */
      }
    return(j);
  }

