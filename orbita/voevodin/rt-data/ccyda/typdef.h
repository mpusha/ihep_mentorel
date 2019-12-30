struct sddbd {
	char lena;
	char ncrc;
	char rena[20];
	short dbti;	/* base number at the BASES*/
 	short n_parent; /* to COMPUTERS */
	short n_child;
	short n_micro;
		} ; 
struct timdat {
	char dat[8];
	char tim[8];
	int secds;
		} ;
struct ncat {
	short nnum;
	short nclon;
	int nfree;
	short noffr;
	short nslon;
		} ;
struct cnam {
	short unsigned nref;
	char n2[2];
	short nlon;
	short noffs;
	int nbloc;
		} ;
struct atc  {
	int alb;
	int alo;
	int ald;
		} ;
struct atli {
	char adli;
	char atip;
	short aoff;
	short annu;
		} ;
struct relref {
	short nrr;
	short nrt;
	short nra;
		} ;
struct relis {
	short unsigned ntup;
	short unsigned nat;
	short unsigned lotup;
	short unsigned third;
	char rtype;
	char rstate;
	short res1;
	short res2;
	char nsub;
	char nowner;
	struct relref nexlis;
		} ;
struct comput {
        unsigned char bus_c; /* BC number */
	unsigned char rem_t; /* RT number */
	short nhost;	/* number at dictionary */
	char type_pro;	/* number at COMPUTER_LIST */
	char nhom;	/* number at HOUSES */
	short ncompu;	/* number at dictionary */
	char nos;	/* number at OS_LIST */
		} ;
struct base {
	char full_name[60];
	short n_bd;	/* number at BASES */
		};
typedef struct sddbd *PDD;
typedef struct timdat *PTD;
typedef struct ncat *P1;
typedef struct cnam *P2;
typedef struct atc *PATC;
typedef struct atli *PATLI;
typedef struct relis *P3;
typedef struct comput *TOCO;

