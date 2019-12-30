#include "/usr/usera/voevodin/rt-data/ccydalib/mem_struc.h"
extern struct cata to_cat[num_db];
extern int semaph[num_db];
extern struct sembuf sops;
extern struct reldef rtab[max_rel],*to_rdf;
extern struct g_main gtab[max_rel];
extern char rab[size2b],buf_[size2b+2];
extern struct ncat *p1;
extern struct cnam *p2;
extern struct relis *p3;
extern struct atc *patc;
extern struct atli *patli;
extern struct comput COMP;
extern struct DDBD_rwr_pac ou_pac;
extern int block,cur_bl;
extern short ellen[];

extern unsigned short cdb[num_db];
extern short cur_db,rw_fl,rw_db,cur_type,n_tup,l_tup,n_plan,offs;
extern char *db_nam[],ss_name[];
extern short nsubs,ncoms,nddbd,nhoms,now,nbss;
extern char subsys[];
extern char owner[];
extern char comput[];
extern char ddbd[];
extern char hous[];
extern char bases[];

