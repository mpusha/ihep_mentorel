#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "mem_struc.h"

struct cata to_cat[num_db];
key_t Keys;
struct sembuf sops;
int semaph[num_db];
FILE *fpt,*fopen();

struct reldef rtab[max_rel],*to_rdf;
struct g_main gtab[max_rel];
struct DDBD_rwr_pac ou_pac;
struct comput COMP;	/* COMPUTERS tuple */
char rab[2000],buf_[2000],ss_name[30],*cel_rd();
struct ncat *p1;
struct cnam *p2;
struct relis *p3;
struct atc *patc;
struct atli *patli;
int block,cur_bl,*W1;
short ellen[]= { 
	0,
	sizeof(struct cnam),
	sizeof(struct cnam),
	sizeof(struct cnam),
	sizeof(struct cnam),
	sizeof(struct relis),
	sizeof(struct atc),
	sizeof(struct atc),
	sizeof(struct atc),
	};

unsigned short cdb[num_db];
unsigned short cur_db,rw_fl,rw_db,cur_type,n_tup,l_tup,n_plan,offs;
short nsubs,ncoms,nddbd,nhoms,now,nbss;
char subsys[]={"SUBSYSTEMS"};
char owner[]={"OWNERS"};
char comput[]={"COMPUTERS"};
char ddbd[]={"DDBD"};
char bases[]={"BASES"};
char hous[]={"HOUSES"};
extern short bptr;

short inidbl(base_num)
 short base_num;
  {
    short i=0;
	if(base_num >= num_db)
	  return(0x8001);
	if(base_num && cdb[base_num]) return(0);
	if(!cdb[0]) i=letini(0);
	if(!base_num) return(i);
	return(letini(base_num));
  }
letini(base_num)
 short base_num;
  {
    short j=0;
    int blck,i,offs;
     if(!base_num)
       {
        fpt=fopen("/usr/users/ssuda/sys/ssuda.srv","r");
        if(fpt==NULL)
         {
          printf("Can't open /usr/users/ssuda/sys/ssuda.srv\n");
          exit(0);
         }
        fgets(ss_name,sizeof(ss_name),fpt);
        fclose(fpt);
        for(i=0;i<sizeof(ss_name);i++)
          if(ss_name[i]=='\n')
            {
             ss_name[i]=0;
             break;
            }
        memset(rtab,0,sizeof(rtab));
        memset(gtab,0,sizeof(gtab));
        memset(semaph,0,sizeof(semaph));
        memset(cdb,0,sizeof(cdb));
        cdb[0]=fope("/usr/users/ssuda/sys/config.dbm");
        if(!cdb[0])
         {
          printf("Can't open /usr/users/ssuda/sys/config.dbm\n");
          exit(0);
         }
       }
     else
       {
        if(!(i=namn(bases,0,r_n)))
          return(0x8002);
        blck=sizeof(struct ncat)+(i-1)*sizeof(struct atc);
        redbl(cdb[0],to_cat[0].rel_dat_b+blck/sizeb,buf_,size2b);
        patc=(PATC)(buf_ +blck%sizeb);
        blck=to_cat[0].rel_dat_b+patc->alb;
        i=(base_num-1)*bases_tup_length+patc->alo;
        blck += i/sizeb;
        offs = i%sizeb;
        redbl(cdb[0],blck,buf_,size2b);
        cdb[base_num]=w_fope(buf_+offs);
        if((Keys=ftok(buf_+offs,'V')) == -1)
          perror("ftok");
        if((semaph[base_num]=semget(Keys,1,0666 | IPC_CREAT)) == -1)
          perror("semget");
        sops.sem_num=sops.sem_flg=0;
       }
     redbl(cdb[base_num],1,&to_cat[base_num],sizeof(struct cata));
     if(to_cat[base_num].one_dat)
       if(++to_cat[base_num].one_dat<255)
	 wribl(cdb[base_num],1,&to_cat[base_num],sizeof(struct cata));
       else
	 {
	  j=7;
	  wribl(cdb[base_num],1,&to_cat[base_num+j],sizeof(struct cata));
	 }
     if(!base_num)
       {
        if(ss_serv_processor!=my_type)
          {
           W1=(int *)(&to_cat[0]);
           for(j=0;j<19;j++)
             preob32(W1++,ss_serv_processor,my_type);
          }
        now=namn(owner,0,r_n);
        nsubs=namn(subsys,0,r_n);
        ncoms=namn(comput,0,r_n);
        nddbd=namn(ddbd,0,r_n);
        nhoms=namn(hous,0,r_n);
        nbss=namn(bases,0,r_n);
       }
     return(0);
  }

give_com(buf,num)	/* num=row number at COMPUTERS */
  struct comput *buf;
  unsigned short num;
    {
     int blck,i;
        blck=sizeof(struct ncat)+(ncoms-1)*sizeof(struct atc);
        redbl(cdb[0],to_cat[0].rel_dat_b+blck/sizeb,buf_,size2b);
        patc=(PATC)(buf_ +blck%sizeb);
        if(ss_serv_processor!=my_type)
          {
           preob32(&patc->alb,ss_serv_processor,my_type);
           preob32(&patc->alo,ss_serv_processor,my_type);
           preob32(&patc->ald,ss_serv_processor,my_type);
          }
        blck=to_cat[0].rel_dat_b+patc->alb;
        i=(num-1)*sizeof(struct comput)+patc->alo;
        redbl(cdb[0],blck+i/sizeb,buf_,size2b);
        strncpy((char*)buf,(char*)(buf_+i%sizeb),sizeof(struct comput));
        if(ss_serv_processor!=my_type)
          {
           preob16(&buf->nhost,ss_serv_processor,my_type);
           preob16(&buf->ncompu,ss_serv_processor,my_type);
          }
    }
fi_tup(n_db,name,n_tup,l_tup,block,offs)
 char *name;
 int block;
 unsigned short n_tup,l_tup,offs,n_db;
  {
   int curb;
   unsigned short nm_l=0,i,j,n_t=0;
    while(name[nm_l])
      ++nm_l;
    redbl(cdb[n_db],block,buf_,size2b);
    while(n_tup--)
      {
        ++n_t;
        for(i=0;i<nm_l;i++)
	 if(name[i]!=buf_[offs+i]) goto next;
        return(n_t);
next:
        curb=block+l_tup/sizeb;
        offs+= l_tup%sizeb;
        if(offs>=sizeb)
          {
            ++curb;
            offs -= sizeb;
          }
        if((offs+nm_l) > size2b) curb++;
        if(curb!=block)
          {
            block=curb;
	    redbl(cdb[n_db],block,buf_,size2b);
          }
      }
    return(0);
  }

idrel(id_r)
 unsigned short id_r;
  {
    if(id_r>=max_rel || !rtab[id_r].rel_num) return(0x8000);
    to_rdf= &rtab[id_r];
    cur_db=to_rdf->ndb;
    return(0);
  }

nnam(n_n,s_n)
 unsigned short s_n,*n_n;
  {
   unsigned short i;
   char j[2];
   int *ref_c;
    if(s_n>t_f) return(0x8000);
    p2=(P2)cel_rd(cur_db,*n_n,s_n);
    if(cur_db==0 && ss_serv_processor!=my_type)
      {
       preob16(&p2->nlon,ss_serv_processor,my_type);
       preob16(&p2->noffs,ss_serv_processor,my_type);
       preob32(&p2->nbloc,ss_serv_processor,my_type);
      }
    if(!p2->nref) return(0x8000);
    if( (*n_n=p2->nlon)<=2)return(p2->n2 - buf_);
    i=p2->noffs;
    j[0]=p2->n2[0]; j[1]=p2->n2[1];
    ref_c= (int*)&to_cat[cur_db];
    redbl(cdb[cur_db],p2->nbloc+ref_c[s_n<<1],buf_+2,size2b);
    *(buf_+i)=j[0]; *(buf_+i+1)=j[1];
    return(i);
  }

fi_at(at_n,at_of)
 unsigned short at_n,*at_of;
  {
   unsigned short i;
    if(p3->rtype&1)
      {
       *at_of= (--at_n)*(p3->nsub&255);
       return(p3->nsub&255);
      }
    block=(--at_n)*(long)sizeof(struct atli);
    i=to_rdf->attr_of+block%sizeb;
    block=to_rdf->attr_bl+block/sizeb;
    if(i>=sizeb)
      {
       ++block;
       i -= sizeb;
      }
    patli=(PATLI)(buf_+i);
    redbl(cdb[cur_db],block,buf_,size2b);
    *at_of=patli->aoff;
    return(patli->adli&255);
  }
namn(name,db_n,subf_n)
 char *name;
 unsigned short db_n,subf_n;
  {
   int *ref_c,ca_bl,curb;
   unsigned short nm_l=0,in,nnam=0,i;
    while(name[nm_l])
      ++nm_l;
    if(subf_n<r_n || subf_n>t_f)
      return(0);
    ref_c= (int*)&to_cat[db_n];
    ca_bl=ref_c[subf_n<<1];
    curb=ca_bl+1;
    redbl(cdb[db_n],ca_bl,buf_,size2b);
    p1=(P1)buf_;
    p2=(P2)(buf_+sizeof(struct ncat));
    if(db_n==0 && ss_serv_processor!=my_type)
      {
       preob16(&p2->nlon,ss_serv_processor,my_type);
       preob16(&p2->noffs,ss_serv_processor,my_type);
       preob32(&p2->nbloc,ss_serv_processor,my_type);
       preob16(&p1->nnum,ss_serv_processor,my_type);
       preob16(&p1->nclon,ss_serv_processor,my_type);
       preob32(&p1->nfree,ss_serv_processor,my_type);
       preob16(&p1->noffr,ss_serv_processor,my_type);
       preob16(&p1->nslon,ss_serv_processor,my_type);
      }
    in=p1->nnum;
    while(in--)
      {
       ++nnam;
       if(p2->nref && nm_l==p2->nlon && p2->n2[0]== *name && 
	  p2->n2[1]== *(name+1)
	  && cmpsd(cdb[db_n],name+2,nm_l-2,p2->nbloc+ca_bl,p2->noffs))
            return(nnam);
       ++p2;
       if(((char*)p2-buf_) >= sizeb)
         {
	  mov512(buf_);
	  p2=(P2)( (char*)p2-sizeb);
	  redbl(cdb[db_n],++curb,buf_+sizeb,sizeb);
         }
      }
    return(0);
  }

mov512(buf)
 char *buf;
  {
   unsigned short i;
    for(i=0;i<sizeb;i++)
      buf[i]=buf[i+sizeb];
  }

cmpsd(to_file,name,n_len,block,offs)
 unsigned short to_file,offs;
 short n_len;
 char *name;
 int block;
  {
    if(n_len <=0)return(1);
    rw_db=cur_bl=0;
    redbl(to_file,block,rab,size2b);
    while(n_len--)
      if(*name++ != rab[offs++]) return(0);
    return(1);
  }

char *cel_rd(n_db,n_num,n_subf)  /* find and read subfile catalog
				     element into buf_ */
 unsigned short n_db,n_num,n_subf;
  {
   int *ref_c;
   unsigned short offs;
    block=sizeof(struct ncat) +(long)(n_num-1)*ellen[n_subf];
    ref_c= (int*)&to_cat[n_db];
    offs = block%sizeb;
    block=ref_c[n_subf<<1]+block/sizeb;
    redbl(cdb[n_db],block,buf_,size2b);
    return(buf_+offs);
  }
set_rel(nre)
  short nre;
   {
       if(!nre)return(0);
       p3=(P3)cel_rd(0,nre,r_l);
       n_tup=p3->ntup;
       l_tup=p3->lotup;
       n_plan=p3->third;
       if(ss_serv_processor!=my_type)
         {
          preob16(&n_tup,ss_serv_processor,my_type);
          preob16(&l_tup,ss_serv_processor,my_type);
         }
       patc=(PATC)cel_rd(0,nre,r_d);
       if(ss_serv_processor!=my_type)
         {
          preob32(&patc->alb,ss_serv_processor,my_type);
          preob32(&patc->alo,ss_serv_processor,my_type);
          preob32(&patc->ald,ss_serv_processor,my_type);
         }
       return(1);
   }
re_tu(n_tab,n_tu,arra)
  char *arra;
  short n_tab,n_tu;
   {
    if(!set_rel(n_tab)) return(1);	/* error */
    if(n_tu <1 || n_tu > (n_tup*n_plan)) return(1);
    block = to_cat[0].rel_dat_b + patc->alb + (n_tu-1)*l_tup/sizeb;
    offs = patc->alo + (n_tu-1)*l_tup % sizeb;
    if(offs >= sizeb)
      {
       block += offs/sizeb;
       offs %= sizeb;
      }
    redbl(cdb[0],block,buf_,size2b);
    memcpy(arra,buf_+offs,l_tup);
    return(0);
   }

