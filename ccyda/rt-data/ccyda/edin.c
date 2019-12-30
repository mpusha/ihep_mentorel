/* 
     Edit Information in system DB
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "typdef.h"
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
#define task_name 40
#define micro_name 60

 FILE *fcom,*fsave;
#define catdli 19
#define sizeb 512
#define size2b 1024
#define bufdli 2500
extern int exten();
int er,_a,_b,_c,bufk[catdli];
extern short error1;
extern int begb,lasb,curb,lor,lorab,lowo;
short base_number,computer;
extern int bptr,bptr0,bptrcf;
char buf[bufdli],work[bufdli],inpnum();

struct d_t {
     unsigned char tip;
     unsigned char prio;
     unsigned short interv;
     unsigned short to_tas;
     };
typedef struct d_t D_T;
short dtex(short);
int ficom(int);
void N_show(short);
void N_add(short,char*);
void N_del(short,char*);
int sel_com();
int sel_ec();
int in_ec_tsk(D_T*);
void pri_co();

struct sddbd pdd1,*pdd2;
struct timdat *ptd;
static struct atli *patli;
static struct atc *patc;
static struct relis p3,*p30;
char *cpt;
short adlina[]={1,2,4,4,8,0,0,2,4,6};

static char *ref1[]={
		"ADD",
		"SHOW",
		"DELETE"
		};
char *ref2[]={
        "New Ethernet Host",
        "New Equipment Controller"
        };
char *ref3[]={
        "Delete all tasks for EC",
        "Select tasks to delete"
        };
char *ref4[]={
        "Show all computers",
        "Show selected computer"
        };
static char *ref5[]={
		"COMPUTERS",
		"PROCESSORS",
		"HOUSES",
		"OPERATING SYSTEMS",
		"LIST OF TASKS FOR ECs",
		"LIST OF EC WITH TASKS",
		"LIST OF MICRO FILES",
		"COMPUTER NAMES",
		"DATA BASE NAMES"
		};
char sysbas[]={"/usr/users/ssuda/sys/config.dbm"};
static char nam1[]={"EDIN.OUT"};
static struct comput v_comp,v_c;
static struct tasks {
     char n_vpv[4];
     char n_file[40];
     } v_tasks,v_t;
static struct ec_tsks {
     short to_comp;
     struct d_t tsk[10];
     } v_ec_tsks,v_e;
static struct mc_tas {
     short to_vme;
     char bc;
     char rt;
     char mbc;
     char mN;
     char file[60];
     } v_mc_file,v_m;
short *totup=0,glnogl=0,mlen=0,numow=0,tofind,for_fcom;
short nbas,ntl,ncoms,nect,nhoms,now,nmfl,ncom_li,nos_li;
static char task_l[]={"TASKS"};
static char owner[]={"OWNERS"};
static char comput[]={"COMPUTERS"};
static char ec_t[]={"EC_TSKS"};
static char hous[]={"HOUSES"};
static char m_file[]={"MC_TASKS"};
static char compu_list[]={"COMPUTER_LIST"};
static char os_list[]={"OS_LIST"};
static char bases[]={"BASES"};
static int fcod=0;	/* your rights */
static int lo;
static short i,ii,j,n,m,q,r,s,t,k,fl_1;
main()
  {
	fsave=fopen(nam1,"w");
	bptr=bptr0=bptrcf= -1;
	bptrcf=open(sysbas,2);
        fcom=0;
	if(bptrcf != -1)
	  {
	   tosyba();
	   redshr(1,bufk,sizeof(bufk));
	   ntl=namn(task_l,(short)(sizeof(task_l)-1),buf,(short)r_n);
	   nect=namn(ec_t,(short)(sizeof(ec_t)-1),buf,(short)r_n);
	   ncoms=namn(comput,(short)(sizeof(comput)-1),buf,(short)r_n);
	   nhoms=namn(hous,(short)(sizeof(hous)-1),buf,(short)r_n);
	   now=namn(owner,(short)(sizeof(owner)-1),buf,(short)r_n);
	   ncom_li=namn(compu_list,sizeof(compu_list)-1,buf,r_n);
	   nos_li=namn(os_list,sizeof(os_list)-1,buf,r_n);
	   nmfl=namn(m_file,sizeof(m_file)-1,buf,r_n);
	   nbas=namn(bases,sizeof(bases)-1,buf,r_n);
	   if(now)
	     {
	      printf("\n PASSWORD-");
	      i=gtpswd();
	      if(i)
		{
		 if((numow=fiow(now)))
	 	 fcod= *(int *)(buf+1016);
		}
	     }
          }
        else
          {
           perror("Sys.base");
           exit(0);
          }
	computer=0;
begin:
	tofind=true;
	t=for_fcom=0;
	if((i=oumenu(3,ref1,1))==0)
	  exit(0);
	switch (i)
	  {
	   case 1:		/* add */
	   if(cost())break;		/* not permissible */
	   if((i=oumenu(7,ref5,1))==0)
             goto begin;
           switch (i)
             {
              case 1:              /* computer */
              memset(&v_comp,0,sizeof(v_comp));
              if((i=oumenu(2,ref2,1))==0)
                break;
              switch(i)
                {
                 case 1:  /* Ethernet Host */
                 printf("Enter new Ethernet Host Name:");
                 i=dtex(2);
                 if(!i)
                   goto begin;
                 fprintf(fsave,"New Ethernet Host name is added as %d\n",i);
                 v_comp.nhost=v_comp.ncompu=i;
                 break;
                 case 2:  /* Equipment Controller */
                 printf("Enter new Equipment Controller Name:");
                 i=dtex(2);
                 if(!i)
                   goto begin;
                 fprintf(fsave,"New Equipment Controller name is added as %d\n",i);
                 v_comp.ncompu=i;
                 printf("Select VME crate name from Computer list:\n");
	         if(!(i=sel_com()))
                   goto begin; 
                 v_comp.nhost=i;
BC_in:
                 printf("Enter BC number:");
                 if(inpnum(&i)!='\n')
                   {
BC_er:
                    printf("\n\nError BC number\n");
                    goto BC_in;
                   }
                 if(i<1 || i>8) 
                   goto BC_er;
                 v_comp.bus_c=i;
RT_in:
                 printf("Enter RT number:");
                 if(inpnum(&i)!='\n')
                   {
RT_er:
                    printf("\n\nError RT number\n");
                    goto RT_in;
                   }
                 if(i<1 || i>30) 
                   goto RT_er;
                 v_comp.rem_t=i;
                 break;
                }
PHOS:
              printf("\n\nSelect Processor\n");
              N_show(ncom_li);
              printf("   Enter number:");
              if(inpnum(&i)!='\n')
                goto PHOS;
              v_comp.type_pro=i;
              printf("Select House\n");
              N_show(nhoms);
              printf("   Enter number:");
              if(inpnum(&i)!='\n')
                goto PHOS;
              v_comp.nhom=i;
              printf("Select Operating System\n");
              N_show(nos_li);
              printf("   Enter number:");
              if(inpnum(&i)!='\n')
                goto PHOS;
              v_comp.nos=i;
              n=1;
              while(rwstr(buf,0,n,1,ncoms,1,&v_c))
                {
                 if(v_c.ncompu)
                   ++n;
                 else
                   {
                    if(!rwstr(buf,1,n,1,ncoms,1,&v_comp))
                      {
                       printf("\n\nError to write to COMPUTERS table tuple=%d !!!???\n",n);
                       fprintf(fsave,"Error to write to COMPUTERS table\n");
                       goto begin;
                      }
                    printf("\n\nNew computer is registed\n");
                    fprintf(fsave,"New computer is registed\n");
                    goto begin;
                   }
                }
              printf("\n\nTable COMPUTERS is full!!!???\n\n");
              break;
              case 2:              /* processor */
              N_add(ncom_li,"Processor Name");
              break;
              case 3:              /* house */
              N_add(nhoms,"House Name");
              break;
              case 4:              /* OS */
              N_add(nos_li,"Operating System Name");
              break;
              case 5:              /*list of EC tasks */
              memset(&v_tasks,0,sizeof(v_tasks));
              printf("Task Name under VPV (1-4 symbols) = ");
              i=intext(buf,0);
              if(i>4)
                {
                 printf("Too long VPV name\n");
                 break;
                }
              bcopy(buf,v_tasks.n_vpv,i);
              printf("Full File Name (1-40 symbols) = ");
              i=intext(buf,0);
              if(i>40)
                {
                 printf("Too long file name\n");
                 break;
                }
              bcopy(buf,v_tasks.n_file,i);
              n=1;
              while(rwstr(buf,0,n,1,ntl,1,&v_t))
                {
                 if(v_t.n_vpv[0])
                   ++n;
                 else
                   {
                    if(!rwstr(buf,1,n,1,ntl,1,&v_tasks))
                      {
                       printf("\n\nError to write to TASKS table tuple=%d !!!???\n",n);
                       fprintf(fsave,"Error to write to TASKS table\n");
                       goto begin;
                      }
                    printf("\n\nNew EC task is registed\n");
                    fprintf(fsave,"New EC task is registed\n");
                    goto begin;
                   }
                }
              printf("\n\nTable TASKS is full!!!???\n\n");
              break;
              case 6:              /* EC_tasks */
              memset(&v_ec_tsks,0,sizeof(v_ec_tsks));
              printf("Select Equipment Controller\n");
	      if(sel_ec()) 
                break;
              n=1;
              fl_1=0;
              while(rwstr(buf,0,n,1,nect,1,&v_e))
                if(v_e.to_comp==t)
                  {
                   memcpy(&v_ec_tsks,&v_e,sizeof(v_e));
                   for(j=0;j<10;j++)
                     if(v_ec_tsks.tsk[j].to_tas==0)
                       {
                        fl_1=1;
                        goto add_ec_t;
                       }
                   printf("There are 10 tasks for this EC already\n");
                   goto begin;
                  }
                else
                  ++n;
              v_ec_tsks.to_comp=t; /* row number of COMPUTERS table */
              j=0;
add_ec_t:
              for(i=j;i<10;i++)
                {
                 printf("\n Task number %d\n",i+1);
                 if(in_ec_tsk(&v_ec_tsks.tsk[i]))
                   break;
                }
              if(fl_1)
                goto ec_exist;
              n=1;
              while(rwstr(buf,0,n,1,nect,1,&v_e))
                {
                 if(v_e.to_comp)
                   ++n;
                 else
                   {
ec_exist:
                    if(!rwstr(buf,1,n,1,nect,1,&v_ec_tsks))
                      {
                       printf("\n\nError to write to EC_TSKS table tuple=%d !!!???\n",n);
                       fprintf(fsave,"Error to write to EC_TSKS table\n");
                       goto begin;
                      }
                    printf("\n\nNew EC_TSKS element is registed\n");
                    fprintf(fsave,"New EC_TSKS element is registed\n");
                    goto begin;
                   }
                }
              printf("\n\nTable EC_TSKS is full!!!???\n\n");
              break;
              case 7:              /* micro file */
              memset(&v_mc_file,0,sizeof(v_mc_file));
              printf("Select Equipment Controller\n");
	      if(sel_ec()) 
                break;
              v_mc_file.to_vme=v_comp.nhost;
              v_mc_file.bc=v_comp.bus_c;
              v_mc_file.rt=v_comp.rem_t;
              printf("Enter Micro Bus number (1-4): ");
              if(inpnum(&i)!='\n')
                goto begin;
              if(i<1 || i>4)
                {
                 printf("\n\nWrong Micro Bus number\n");
                 goto begin;
                }
              printf("Enter Micro Controller number (1-16): ");
              if(inpnum(&i)!='\n')
                goto begin;
              v_mc_file.mbc=i;
              if(i<1 || i>16)
                {
                 printf("\n\nWrong Micro Controller number\n");
                 goto begin;
                }
              v_mc_file.mN=i;
              printf("Full File Name (1-60 symbols) = ");
              i=intext(buf,0);
              if(i>60)
                {
                 printf("Too long file name\n");
                 goto begin;
                }
              bcopy(buf,v_mc_file.file,i);
              n=1;
              while(rwstr(buf,0,n,1,nmfl,1,&v_m))
                {
                 if(v_m.to_vme)
                   ++n;
                 else
                   {
                    if(!rwstr(buf,1,n,1,nmfl,1,&v_mc_file))
                      {
                       printf("\n\nError to write to MC_TASKS table tuple=%d !!!???\n",n);
                       fprintf(fsave,"Error to write to MC_TASKS table\n");
                       goto begin;
                      }
                    printf("\n\nNew MC_TASKS element is registed\n");
                    fprintf(fsave,"New MC_TASKS element is registed\n");
                    goto begin;
                   }
                }
              printf("\n\nTable MC_TASKS is full!!!???\n\n");
              break;
             }
	   break;
	   case 2:			/* show */
	   if((i=oumenu(9,ref5,1))==0)
             goto begin;
           switch (i)
             {
              case 1:              /* computer */
	      if((i=oumenu(2,ref4,1))==0)
                break;
              switch (i)
                {
                 case 1: /* all computers */
                 if(!rlread(&p3,ncoms,buf))
                   break;
                 for(i=t;t<=p3.ntup;t++)
                   {
                    rwstr(buf,0,t,1,ncoms,1,&v_comp);
                    if(v_comp.ncompu)
                      {
                       printf("%d. ",t);
                       fprintf(fsave,"%d. ",t);
                       pri_co();
                      }
                   }
                 break;
                 case 2:  /* selected computer */
	         if(!(i=sel_com())) 
                   break;
                 if(!(t=ficom(i)))
                   {
                    printf("\n\nNo such computer in COMPUTERS table\n");
                    break;
                   }
                 pri_co();
                 break;
                }
              break;
              case 2:              /* processor */
              fprintf(fsave,"SSUDA supports next types of processors:\n");
              N_show(ncom_li);
              break;
              case 3:              /* house */
              fprintf(fsave,"SSUDA knows next Buildings:\n");
              N_show(nhoms);
              break;
              case 4:              /* OS */
              fprintf(fsave,"SSUDA supports next types of Operating Systems:\n");
              N_show(nos_li);
              break;
              case 5:              /* tasks */
              fprintf(fsave,"There are next Tasks to load to ECs\n");
              ii=1;
              memset(buf,0,sizeof(buf));
              while(rwstr(buf,0,ii++,1,ntl,1,&v_tasks))
                if(v_tasks.n_vpv[0])
                  {
                   printf("%d. %c%c%c%c  ==>  %s\n",
                      ii-1,v_tasks.n_vpv[0],v_tasks.n_vpv[1],v_tasks.n_vpv[2],v_tasks.n_vpv[3],v_tasks.n_file);
                   fprintf(fsave,"%d. %c%c%c%c  ==>  %s\n",
                      ii-1,v_tasks.n_vpv[0],v_tasks.n_vpv[1],v_tasks.n_vpv[2],v_tasks.n_vpv[3],v_tasks.n_file);
                  }
              fprintf(fsave,"\n");
              break;
              case 6:              /* EC_tasks */
              fprintf(fsave,"There are next Equpment Controllers with their Tasks:\n");
              ii=1;
              memset(buf,0,sizeof(buf));
              while(rwstr(buf,0,ii++,1,nect,1,&v_ec_tsks))
               {
                if(v_ec_tsks.to_comp)
                  {
                   if(nnam(work+1000,100,buf,t_f,v_ec_tsks.to_comp)==0)
                     {
                      printf("\n\nName not found for %d\n",ii-1);
                      break;
                     }
                   printf("%d. Equipment Controller: %s ",ii-1,work+1000);
                   fprintf(fsave,"%d. Equipment Controller: %s ",ii-1,work+1000);
                   if(ficom(v_ec_tsks.to_comp))
                     if(nnam(work+1000,100,buf,t_f,v_comp.nhost)!=0)
                     {
                      printf("==> %s BC=%d RT=%d",work+1000,v_comp.bus_c,v_comp.rem_t);
                      fprintf(fsave,"==> %s BC=%d RT=%d run next tasks:",work+1000,v_comp.bus_c,v_comp.rem_t);
                     }
                   printf("\n");
                   fprintf(fsave,"\n");
                   for(s=0;s<10;s++)
                    {
                     if(v_ec_tsks.tsk[s].to_tas)
                       if(rwstr(buf,0,v_ec_tsks.tsk[s].to_tas,1,ntl,1,&v_tasks))
                         {
                          printf("     %d. %s",s+1,v_tasks.n_file);
                          fprintf(fsave,"     %d. %s",s+1,v_tasks.n_file);
                          v_tasks.n_file[0]=0;
                          printf(" --> %s\n",v_tasks.n_vpv);
                          fprintf(fsave," --> %s\n",v_tasks.n_vpv);
                          printf("        with type=%d priority=%d period=%d\n",
                            v_ec_tsks.tsk[s].tip,v_ec_tsks.tsk[s].prio,v_ec_tsks.tsk[s].interv);
                          fprintf(fsave,"        with type=%d priority=%d period=%d\n",
                            v_ec_tsks.tsk[s].tip,v_ec_tsks.tsk[s].prio,v_ec_tsks.tsk[s].interv);
                         }
                    }
                  }
               }
              fprintf(fsave,"\n");
              break;
              case 7:              /* micro file */
              fprintf(fsave,"There are next files to load to MicroControllers:\n");
              ii=1;
              memset(buf,0,sizeof(buf));
              while(rwstr(buf,0,ii++,1,nmfl,1,&v_mc_file))
                {
                 if(!v_mc_file.to_vme)
                   continue;
                 if(nnam(work+1000,100,buf,t_f,v_mc_file.to_vme)==0)
                   {
                    printf("\n\nVME name not found for %d\n",ii-1);
                    break;
                   }
                 printf("%d. VME: %s BC=%d RT=%d MicroBC=%d MicroN=%d\n",ii-1,work+1000,
                   v_mc_file.bc,v_mc_file.rt,v_mc_file.mbc,v_mc_file.mN);
                 printf("    File: %s\n",v_mc_file.file);
                 fprintf(fsave,"%d. VME: %s BC=%d RT=%d MicroBC=%d MicroN=%d\n",ii-1,work+1000,
                   v_mc_file.bc,v_mc_file.rt,v_mc_file.mbc,v_mc_file.mN);
                 fprintf(fsave,"    File: %s\n",v_mc_file.file);
                }
              break;
              case 8:              /* computer names */
              fprintf(fsave,"This is the list of Computer Names\n");
              dtex(1);
              break;
              case 9:              /* data bases */
              fprintf(fsave,"SSUDA supports next DBs:\n");
              N_show(nbas);
              break;
             }
	   break;
	   case 3:			/* delete */
	   if(cost())break;
	   if((i=oumenu(7,ref5,1))==0)
             goto begin;
           switch (i)
             {
              case 1:              /* computer */
              memset(&v_c,0,sizeof(v_comp));
	      if(!(i=sel_com())) 
                break;
              if(t=ficom(i))
                if(v_comp.bus_c) /* control EC tasks */
                  {
                   n=1;
                   while(rwstr(buf,0,n++,1,nect,1,&v_e))
                     if(v_e.to_comp==t)
                       {
                        printf("There are tasks to load to this EC !!!\n Do you want to delete this computer ??? (Y/N):");
                        ii=intext(buf,0);
                        if(ii!=1 || buf[0]!='Y')
                          goto begin;
                       }
                  }
              n_del(i,buf,t_f);
              if(!(t=ficom(i)))
                {
                 printf("\n\nNo such computer in COMPUTERS table\n");
                 break;
                }
              if(!rwstr(buf,1,t,1,ncoms,1,&v_c))
                {
                 printf("\n\nError to write to COMPUTERS table tuple=%d !!!???\n",t);
                 fprintf(fsave,"Error to write to COMPUTERS table\n");
                 break;
                }
              printf("\n\nComputer is deleted\n");
              fprintf(fsave,"Computer is deleted\n");
              break;
              case 2:              /* processor */
              N_del(ncom_li,"Processor Name To Delete");
              break;
              case 3:              /* house */
              N_del(nhoms,"House Name To Delete");
              break;
              case 4:              /* OS */
              N_del(nos_li,"Operating System Name To Delete");
              break;
              case 5:              /* tasks */
              memset(&v_tasks,0,sizeof(v_tasks));
              printf("Task Name under VPV (1-4 symbols) = ");
              i=intext(buf,0);
              if(i>4)
                {
                 printf("Too long VPV name\n");
                 break;
                }
              memset(&v_t,0,sizeof(v_t));
              bcopy(buf,v_t.n_vpv,i);
              n=1;
              while(rwstr(buf+100,0,n,1,ntl,1,buf))
                {
                 if(v_t.n_vpv[0] != buf[0] ||
                    v_t.n_vpv[1] != buf[1] ||
                    v_t.n_vpv[2] != buf[2] ||
                    v_t.n_vpv[3] != buf[3])
                   ++n;
                 else
                   {
                    if(!rwstr(buf,1,n,1,ntl,1,&v_tasks))
                      {
                       printf("\n\nError to write to TASKS table tuple=%d !!!???\n",n);
                       fprintf(fsave,"Error to write to TASKS table\n");
                       goto begin;
                      }
                    printf("\n\nEC task is deleted\n");
                    fprintf(fsave,"EC task is deleted\n");
                    goto begin;
                   }
                }
              printf("\n\nNo such task in the table TASKS !!!???\n\n");
              break;
              case 6:              /* EC_tasks */
              memset(&v_ec_tsks,0,sizeof(v_ec_tsks));
              if((ii=oumenu(2,ref3,1))==0)
                break;
              if(sel_ec())
                break;
              n=1;
              while(rwstr(buf,0,n,1,nect,1,&v_e))
               {
                if(v_e.to_comp==t)
                  {
                   if(ii==2) /* select tasks */
                     {
                      memcpy(&v_ec_tsks,&v_e,sizeof(v_e));
                      printf("Select Task to delete:\n");
                      for(s=0;s<10;s++)
                        {
                         if(v_ec_tsks.tsk[s].to_tas)
                          {
                           if(rwstr(buf,0,v_ec_tsks.tsk[s].to_tas,1,ntl,1,&v_tasks))
                             {
                              printf("   %d. %s\n",s+1,v_tasks.n_file);
                              printf("      with type=%d priority=%d period=%d\n",
                                v_ec_tsks.tsk[s].tip,v_ec_tsks.tsk[s].prio,v_ec_tsks.tsk[s].interv);
                             }
                          }
                        }
                      printf("Enter task number to delete: ");
                      if(inpnum(&i)!='\n')
                        goto begin;
                      if(i<1 || i>=10)
                        goto begin;
                      for(s=i-1;s<9;s++) /* move task descriptions */
                        {
                         v_ec_tsks.tsk[s].to_tas=v_ec_tsks.tsk[s+1].to_tas;
                         v_ec_tsks.tsk[s].tip=v_ec_tsks.tsk[s+1].tip;
                         v_ec_tsks.tsk[s].prio=v_ec_tsks.tsk[s+1].prio;
                         v_ec_tsks.tsk[s].interv=v_ec_tsks.tsk[s+1].interv;
                        }
                      memset(&v_ec_tsks.tsk[9],0,sizeof(struct d_t)); /* clear the last */
                     }
                   if(!rwstr(buf,1,n,1,nect,1,&v_ec_tsks))
                     {
                      printf("\n\nError to write to EC_TSKS table tuple=%d !!!???\n",n);
                      fprintf(fsave,"Error to write to EC_TSKS table\n");
                      goto begin;
                     }
                    printf("\n\nEC_TSKS element is deleted\n");
                    fprintf(fsave,"EC_TSKS element is deleted\n");
                    goto begin;
                  }
                else
                  ++n;
               }
              printf("\n\nNo tasks for this EC !!!???\n\n");
              break;
              case 7:              /* micro file */
              ii=1;
              memset(buf,0,sizeof(buf));
              printf("Select file to Delete:\n");
              while(rwstr(buf,0,ii,1,nmfl,1,&v_mc_file))
                {
                 if(v_mc_file.to_vme)
                   {
                    nnam(work+1000,100,buf,t_f,v_mc_file.to_vme);
                    printf("%d. VME: %s BC=%d RT=%d MicroBC=%d MicroN=%d\n",ii,work+1000,
                      v_mc_file.bc,v_mc_file.rt,v_mc_file.mbc,v_mc_file.mN);
                    printf("    File: %s\n",v_mc_file.file);
                   }
                 ii++;
                }
              memset(&v_mc_file,0,sizeof(v_mc_file));
              printf("Enter File number to delete: ");
              if(inpnum(&i)!='\n')
                break;
              if(i<1 || i>=ii)
                break;
              if(!rwstr(buf,1,i,1,nmfl,1,&v_mc_file))
                {
                 printf("\n\nError to write to MC_TASKS table tuple=%d !!!???\n",i);
                 fprintf(fsave,"Error to write to MC_TASKS table\n");
                 break;
                }
              printf("\n\nMC_TASKS element is deleted\n");
              fprintf(fsave,"MC_TASKS element is deleted\n");
              break;
             }
	   break;
	  }
	goto begin;
  }
void pri_co()
  {
   memset(work,0,sizeof(work));
   if(nnam(work+1000,100,buf,t_f,v_comp.ncompu)==0)
     {
      printf("\n\nName not found for %d\n",i);
      return;
     }
   rwstr(buf,0,v_comp.type_pro&255,1,ncom_li,1,work+1100);
   rwstr(buf,0,v_comp.nos&255,1,nos_li,1,work+1200);
   rwstr(buf,0,v_comp.nhom&255,1,nhoms,1,work+1300);
   printf("Host: %s with %s processor and %s is installed at %s\n",
     work+1000,work+1100,work+1200,work+1300);
   fprintf(fsave,"Host: %s with %s processor and %s is installed at %s\n",
     work+1000,work+1100,work+1200,work+1300);
   if(v_comp.bus_c)
     {
      nnam(work+1400,100,buf,t_f,v_comp.nhost);
      printf("      and is connected to %s through BC=%d RT=%d\n",
        work+1400,v_comp.bus_c,v_comp.rem_t);
      fprintf(fsave,"      and is connected to %s through BC=%d RT=%d\n",
        work+1400,v_comp.bus_c,v_comp.rem_t);
     }
   fprintf(fsave,"\n");
  }
int sel_ec()
  {
   printf("Select Equipment Controller\n");
   if(!(i=sel_com())) 
     return(1);
   if(!(t=ficom(i)))
     {
      printf("\n\nNo such computer in COMPUTERS table\n");
      return(1);
     }
   if(v_comp.bus_c==0)
     {
      printf("\n\nThis computer is not EC!!!???\n");
      return(1);
     }
   return(0);
  }
int in_ec_tsk(D_T *ref)
  {
   printf("Select Task to load to EC:\n");
   ii=1;
   memset(buf,0,sizeof(buf));
   while(rwstr(buf,0,ii++,1,ntl,1,&v_tasks))
     if(v_tasks.n_vpv[0])
       {
        printf("%d. %c%c%c%c  ==>  %s\n",
        ii-1,v_tasks.n_vpv[0],v_tasks.n_vpv[1],v_tasks.n_vpv[2],v_tasks.n_vpv[3],v_tasks.n_file);
       }
   if(inpnum(&i)!='\n')
     return(1);
   if(i==0)
     return(1);
   if(i>=ii)
     {
      printf("Number is too large\n");
      return(1);
     }
   ref->to_tas=i;
   printf("Enter type of the task (N+M)\n   N=128 - task starts immediately\n   M=3 - task starts periodically \n   N+M = 0 - task starts by external command\n : ");
   if(inpnum(&i)!='\n')
     return(1);
   ref->tip=i;
   i &= 7;
   if(i==3)
     {
      printf("Enter period of restarting in 100ms : ");
      if(inpnum(&i)!='\n')
        return(1);
      ref->interv=i;
     }
   else
     ref->interv=0;
   printf("Enter priority (0-22) : ");
   if(inpnum(&i)!='\n')
     return(1);
   if(i>=23)
     return(1);
   ref->prio=i;
   return(0);
  }
int ficom(int j)
  {
   short n=1;
   while(rwstr(buf,0,n++,1,ncoms,1,&v_comp))
     {
      if(v_comp.ncompu == j)
        return(n-1);
     }
   return(0); /* not found */
  }
int fiow(int j)
  {
   short n=1;
        while(rwstr(buf+100,0,n++,1,j,1,buf+1000))
          if(strcmp(buf,buf+1000)==0) goto ex;
        n=1;
ex:
        return(n-1);
  }
void N_show(short nrel)
  {
   ii=1;
   memset(buf,0,sizeof(buf));
   while(rwstr(buf,0,ii++,1,nrel,1,buf+1000))
     if(buf[1000])
       {
        printf("%d. %s\n",ii-1,buf+1000);
        fprintf(fsave,"%d. %s\n",ii-1,buf+1000);
       }
   fprintf(fsave,"\n");
  }
void N_del(short nrel,char *n_ou)
  {
  char for_N[20];
   ii=1;
   memset(for_N,0,sizeof(for_N));
   while(rwstr(buf,0,ii,1,nrel,1,for_N))
     {
      if(for_N[0])
        printf("%d. %s\n",ii,for_N);
      ++ii;
     }
   printf("Enter number of %s: ",n_ou);
   if(inpnum(&i)!='\n')
     {
      printf("%s is not deleted\n",n_ou);
      return;
     }
   memset(for_N,0,sizeof(for_N));
   memset(buf,0,sizeof(buf));
   rwstr(buf,0,i,1,nrel,1,for_N);
   if(rwstr(buf,1,i,1,nrel,1,buf+2000))
     printf("%s - %s was cleared\n",n_ou,for_N);
   else
     printf("Error on write to table\n");
  }
void N_add(short nrel,char *n_ou)
  {
   ii=1;
   memset(buf,0,sizeof(buf));
   while(rwstr(buf,0,ii,1,nrel,1,buf+1000))
     if(buf[1000]) /* tuple is busy */
       {
        ++ii;
        continue;  /* Next tuple */
       }
     else
       {
rep1:
        printf("Enter %s (less or equal to 16 symbols): ",n_ou);
        if((i=intext(buf,0))==0)
           return;
        if(i>16)
          {
           printf("Name is too long\n");
           goto rep1;
          }
        if(rwstr(buf+1000,1,ii,1,nrel,1,buf))
          printf("%s --> %s is written in tuple %d\n",n_ou,buf,ii);
        else
          printf("Error on DB write\n");
        return;
       }
   printf("Table is full\n");
  }
int sel_com()
  {
   lo=bufk[t_f<<1];
   redshr(lo,buf,sizeb);
   n= *(short *)buf;	/* number of texts */
   k=1;
echemen:
   j=0;
   while(n--)
     {
      if((i=nnam(buf,(short)sizeb,buf+sizeb,t_f,k))!=0)
        {
         if(i>=sizeb) i=sizeb-1;
         buf[i]='\0';
         printf("%d. %s\n",k,buf);
         if(++j == 20) break;
        }
      ++k;
     }
   ++k;
bcemen:
   if(k==1)
     return(0);
   printf("enter number or RETURN/Enter:");
   i=0;
   while((buf[i++]=getchar())!='\n')
     ;
   if(buf[0]=='\n')
     if(j!=20)
       {
        if(!n || n<0)
          return(0);
        goto bcemen;
       }
     else
       {
        if(!n || n<0)
          return(0);
        goto echemen;
       }
   k=0;
   sscanf(buf,"%d",&k);
   return(k);
  }
short dtex(short q)
  {
  short i,j;
  char c_nam[80];
   j=t_f;  /* TEXTS - computer names */
   lo=bufk[j<<1];
   redshr(lo,buf,sizeb);
   n= *(short *)buf;	/* number of texts */
   ii=q;
   switch(q)
     {
      case 1:	/* type texts */
      fprintf(fsave,"This is the List of Computer Names:\n");
      k=1;
      while(n--)
        {
         if((i=nnam(buf,(short)sizeb,buf+sizeb,j,k))!=0)
           {
            if(i>=sizeb) i=sizeb-1;
            buf[i]='\0';
            fprintf(fsave,"%d. %s\n",k,buf);
            printf("%d. %s\n",k,buf);
           }
         ++k;
        }
      fprintf(fsave,"\n\n");
      break;
      case 2:		/*include text(not rel.names)*/
      i=intext(c_nam,0);
      if(i)
        {
         if((namn(c_nam,i,buf,j))!=0)
           {
            printf("There is such computer Name\n");
            return(0);
           }
         i=n_incl(c_nam,i,buf,(int)j);
        }
      return(i);
     }
  }

binhex(c,bb)
 char c,*bb;
  {
   char d;
	d=(c>>4)&15;
	c &= 15;
	if(d<=9)
	  d += '0';
	else
	  d += 'A'-10;
	if(c<=9)
	  c += '0';
	else
	  c += 'A'-10;
	*bb++ =d;
	*bb=c;
  }
hexbin(dlb,bb,bsym)
 short dlb;
 char *bb,*bsym;
  {
   short i=dlb;
   char c;
	while(i--)
	  bb[i-1]=0;
	i=0;
	while((bsym[i]>='0' && bsym[i]<='9') ||
	  (bsym[i]>='A' && bsym[i]<='F') ||
	  (bsym[i]>='a' && bsym[i]<='f') )
	  ++i;
	--i;		/* last hexa digit */
	while(dlb--)
	  {
	   if(i<0)return;
	   if(bsym[i]>='0' && bsym[i]<='9')
	     c=bsym[i]-'0';
	   else
	     {
	      if(bsym[i]>='A' && bsym[i]<='F')
		c=bsym[i]-'A'+10;
	      else
	        c=bsym[i]-'a'+10;
	     }
	   --i;
	   if(i>=0)
	     {
	      if(bsym[i]>='0' && bsym[i]<='9')
	        c += (bsym[i]-'0')<<4;
	      else
	        {
	         if(bsym[i]>='A' && bsym[i]<='F')
	           c += (bsym[i]-'A'+10)<<4;
	         else
	           c += (bsym[i]-'a'+10)<<4;
	        }
	      --i;
	     }
	   bb[dlb]=c;
	  }
  }

cost()
  {
	if(now && (fcod&0x80)) return(0);
	if(!now) return(0);
	printf("YOU HAVE NOT RIGHTS\n");
	return(1);
  }

oumes(n)
 short n;
  {
   static char *tomes[]={
	"enter text=",              	/*  0 */
	"enter decimal=",           	/*  1 */
	"\n ? there is relation\n",  	/*  2 */
	"NUMBER OF ATTRIBUTES TUPLES PLANES",/*  3 */
	"insuffisient dynamic memory\n",/*  4 */
	"\n ? no such relation\n",  	/*  5 */
	"\n ? parameter input error\n",	/*  6 */
	"\n ? rel.inf. read error\n",	/*  7 */
	"\n ? no such tuple\n",     	/*  8 */
	"\n ? name including error\n",	/*  9 */
	"[size]",                   	/* 10 */
	"\n ? repeate mode error\n",	/* 11 */
	"\nno such attribute\n",    	/* 12 */
	"[RELATION]",             	/* 13 */
	"\ntuples",                 	/* 14 */
	"\nplanes",                  	/* 15 */
	"\n ? ERR.ON DELETE\n",      	/* 16 */
	"\n ? NO SUCH TEXT\n",      	/* 17 */
	"\n ? OPEN ERROR\n"         	/* 18 */
		};
  	  fprintf(fsave," %s",tomes[n]);
  	  if(!fcom) printf(" %s",tomes[n]);
  }
intext(buf,i)
 char *buf;
 short i;
  {
   short j,ii;
intx:
	j=0;
	if(i==0)
	  while((buf[j]=getchar())!='\n')
	    j++;
	else
	  {
	   while((buf[j]=getchar())!='\002')	/* ctrl/b */
	     ++j;
	   if(getchar()!='\n')goto intx;
	  }
tyx:
	buf[j]='\0';
	if(*buf=='+')
	   for(ii=0;ii<=j;ii++)
	     buf[ii]=buf[ii+1]^0xa1;
	return(j);
  }
char inpnum(j)
 short *j;
  {
   char c;
	*j=0;
	scanf("%d",&_a);   *j=_a;
	c=getchar();
	return(c);
  }
fist()
  {
   char c;
next:
	c=fgetc(fcom);
	ungetc(c,fcom);
	switch (c)
	  {
	   case '[':
	   fgets(buf,520,fcom);
	   while((c=fgetc(fcom))!=']')
	     {
	      ungetc(c,fcom);
	      fgets(buf,520,fcom);
	     }
	   fgets(buf,520,fcom);
	   break;
	   case '.':
	   case ';':
	   case '$':
	   fgets(buf,520,fcom);
	   break;
	   case '=':
	   fgetc(fcom);
	   return(c);
	   case '!':
	   default:
	   return(c);
	  }
	goto next;
  }
oumenu(n,legptr,k)
 char *legptr[];
 short n,k;
  {
   char *p,**p1;
   short i,r,n1;
	p1=legptr;
	if(fcom)
	  {
	   j=0;
	   fist();
	   fgets(buf,520,fcom);
	   while(buf[j]!='\n')
		 j++;
	   buf[j]=0;
	   for(j=0;j<n;j++)
	     if(!strcmp(buf,legptr[j]))return(j+1);
	   return(0);
	  }
	n1=n;
	printf("\n");
	for(i=1;n1>0;n1--)
	  {
	   p= *legptr;
	   if((*legptr)[0])
	     printf("%3d.%s\n",i,*legptr++);
	   ++i;
	  }
i2:
	r=0;
	if(inpnum(&r)!='\n')goto i2;
	if(r <0 || r > n)goto i2;
i3:
	legptr=p1;
	return(r);
  }

