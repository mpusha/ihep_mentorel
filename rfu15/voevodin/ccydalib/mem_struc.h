#define Intelx86
#define ss_serv_processor 1 /* Intelx86 */

#ifdef DECstation
#define my_type 2
#endif
#ifdef Motorola
#define my_type 3
#endif
#ifdef Intelx86
#define my_type 1
#endif

#include "../../voevodin/ccyda/typdef.h"
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define r_n 1 /* rel.name */
#define a_n 2 /* atr.name */
#define t_n 3 /* tup.name */
#define t_f 4 /* text file */
#define r_l 5 /* rel.list */
#define a_l 6 /* atr.list */
#define r_d 7 /* datas */
#define t_l 8 /* tup.list */

#define true 1
#define false 0
#define catdli 19
#define sizeb 512
#define size2b 1024

#define bases_name_length 60
#define bases_tup_length 60
#define num_db 100
#define max_rel 100
#define max_rw 65535
#define max_gb 9100
#define max_FE 100
#define max_comp 300

struct cata {
	short beg_cat[4];
	int rel_nam_b;
	int rel_nam_n;
	int at_nam_b;
	int at_nam_n;
	int tup_nam_b;
 	int tup_nam_n;
	int text_b;
	int text_n;
 	int rel_tab_b;
	int rel_tab_n;
	int rel_at_b;
	int rel_at_n;
	int rel_dat_b;
	int rel_dat_n;
	int rel_tup_b;
	int rel_tup_n;
	int one_dat;
	  };

struct reldef {         /* absolute block numbers */
	unsigned nus;	/* user number */
	unsigned ndb;	/* DB number */
	int rel_num;   	/* =0 - free space */
	int dat_bl;
	int dat_off;
	int dat_len;
	int attr_bl;
	int attr_of;
	int attr_len;
   struct relis re_to_re;
	  };
struct g_main {
	short id_parent;
	short id_child;
	short id_micro;
   struct relis tab_inf;
   struct sockaddr_in parent;
   struct sockaddr_in child;
	unsigned char BC;
	unsigned char RT;
      };
struct DDBD_rwr_pac {
	char  n_proces;	/* processor num. of SSUDA server */
	char  n_que;	/* ref. to Que */
	char  pac_tip;
        char  pac_flag;
	short bd_num;
	char t_nam[20]; /* table name */
	char b_nam[60]; /* base name */
  struct g_main glb;
      };

struct ws_rwr {
        char n_proc;      /* processor type */
        char n_q;
	char pac_tip;     /* = 2 */
	char func;        /* 0-dtrdbl,1-dtwdbl, */
	short tbl_id;
	short buf_size;
	short coord[7];
      } ;
typedef struct ws_rwr *TORWR;

