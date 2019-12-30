#include <stdio.h>
#define r_n 1 /* rel.name */
#define a_n 2 /* atr.name */
#define t_n 3 /* tup.name */
#define t_f 4 /* text file */
#define r_l 5 /* rel.list */
#define a_l 6 /* atr.list */
#define r_d 7 /* datas */
#define t_l 8 /* tup.list */
#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 1550
int fcom=0,fsave=0;
char sysbas[2];
 int bufk[catdli];
 int er,cmdfl;
static int sumbl[8];
static short sumof[8];

extern short zz;

main()
  {
	printf("\nBASE NAME-");
	if(intext(sumbl,0)==0)
	  er=opnshr("base.dat");
	else
	  er=opnshr(sumbl);
	if(er)
	  {
	   printf("\nERR.OPEN=%x\n",er);
	   exit();
	  }
	rek();
	bufk[18]=0;
	wrishr(1,bufk,sizeof(bufk));
  }

