#define length 512
#define lengkt 76
#define lennam 100
#define subfil 8
#define b_k 1l
#define r_n 1
#define a_t 2
#define t_t 3
#define t_f 4
#define r_l 5
#define r_a 6
#define r_d 7
#define r_t 8
extern char buf[],work[],inf[],name[],bufk[];
extern char filenm[];
extern short er,error1;

typedef struct katalog
  {
	int begkat;
	int lenkat;
  }
	namekt,*pnamekt;
typedef struct strkat
  {
	short unsigned numrel;
	short unsigned lon;
	int freblk;
	short frebyt;
	short unsigned katlon;
  }
	nmkt1,*pnkt1;
typedef struct katbs
  {
	int blk;
	int byte;
	int dim;
  }
	kt1,*pkt1;
typedef struct relatr
  {
	char lgn;
	char type;
	short unsigned sm;
	short unsigned in;
  }
	*pnrel;
typedef struct cnam
  {
	short unsigned nref;
	char n2[2];
	short unsigned nlon;
	short noffs;
	int nbloc;
  } *p2;
struct rnet
  {
	short nco;
	short nmp;
  } ;
struct relref
  {
	short nrr;
	short nrt;
	short nra;
  } ;
typedef struct relis
  {
	short unsigned ntup;
	short unsigned nat;
	short unsigned lotup;
	short unsigned plus;
	char rtype;
	char rstate;
	struct rnet refn;
	char nsub;
	char nowner;
	struct relref nexlis;
  } *p3;
