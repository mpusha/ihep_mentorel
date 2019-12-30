#include "../sh_ccydalib/sh_ccydalib.h"
#include "../../rt-data/ccydalib/ccydalib.h"
#include "CommTask.h"

static char T_names[]={"tabs_main.ecs"}, Xt_name[30];  // файл с именами MAIN таблиц ==> имя плоскость строка
static FILE *fpoi;
static struct itimerval newval;
static sigset_t maska;
static struct sigaction xa;
static short num_of_main, i, j, coord[7];
static char  *to_mem1, *to_sh_mem, buf[8500], b_name[25];
static struct my_ref_to_main *main_ref, *M_R;
static int k, p, t, flag_to_sh, svt;
static unsigned int Count_from_cntr=0;    // счётчик измерений в управляющей программе
static unsigned int Count_cntr=0;         // счётчик обновлений управляющих данных
static struct sh_start *to_sh_start;
static union sigval sig_dat;

static struct relop {
     short unsigned ntup;
     short unsigned nat;
     short unsigned lotup;
     short unsigned third;
     char rtype;
     char rstate;
    } r_opi;

static struct rlp {
   char r_name[20];
   unsigned short n_tup;    // число строк
   unsigned short n_attr;   // число столбцов
   unsigned short tup_len;  // длина строки
   unsigned short last_act; // последнее действие 0 - чтение, 0xcccc - запись связной, 0xaaaa - запись управляющей
    } r_m_op;

static int see_main_tabs();
char *get_sh_addr();
struct hostent *Xt_hp,*gethostbyname();
static struct sockaddr_in Xt_sin;

Xt_al_ini()
  {
   fpoi=fopen("/usr/users/ssuda/sys/ssuda.srv","r");
   if(fpoi==NULL)
     {
      printf("Can't open /usr/users/ssuda/sys/ssuda.srv\n");
      exit(0);
     }
   fgets(Xt_name,sizeof(Xt_name),fpoi);
   fclose(fpoi);
   for(i=0;i<sizeof(Xt_name);i++)
     if(Xt_name[i]=='\n')
       {
        Xt_name[i]=0;
        break;
       }
   if(!(Xt_hp=gethostbyname(Xt_name)))
     syslog(LOG_NOTICE,"gethostbyname(%s):%s\n",Xt_name,strerror(errno));
   else
     {
      if((svt=socket(AF_INET,SOCK_DGRAM,0)) < 0)
        syslog(LOG_NOTICE,":CommTask:socket:%s\n",strerror(errno));
      if(bind(svt,(struct sockaddr*)&Xt_sin,sizeof(Xt_sin)))
        syslog(LOG_NOTICE,"bind:%s\n",strerror(errno));
      memcpy(&Xt_sin.sin_addr,Xt_hp->h_addr,Xt_hp->h_length);
      Xt_sin.sin_family=AF_INET;
      Xt_sin.sin_port  = htons(5993); 
     }
  }
Xt_al_text(int s1,char *txt)
  {
  char Xt_bb[102];
   Xt_bb[0]=1;
   Xt_bb[1]=0;
   Xt_bb[101]=0;
   memcpy(Xt_bb+2,txt,99);
   if(Xt_hp)
     {
      if(sendto(s1,Xt_bb,sizeof(Xt_bb),0,(struct sockaddr*)&Xt_sin,sizeof(struct sockaddr_in)) <0)
        syslog(LOG_NOTICE,"sendto to alarmd:%s\n",strerror(errno));
     }
  }

static int time_wait(int ms)              // задержка в миллисекундах
  {
  int sig;
   newval.it_value.tv_sec=ms/1000;        // секунды
   newval.it_value.tv_usec=ms%1000*1000;  // микросекунды
   if(setitimer(ITIMER_REAL,&newval,0))
     {
      syslog(LOG_NOTICE,":CommTask:setitimer:%s\n",strerror(errno));
      perror("setitimer");
      return(1);
     }
   if(sigwait(&maska,&sig))
     {
      syslog(LOG_NOTICE,":CommTask:sigwait:%s\n",strerror(errno));
      perror("sigwait");
      return(1);
     }
   newval.it_value.tv_sec=0;  
   newval.it_value.tv_usec=0; 
   if(setitimer(ITIMER_REAL,&newval,0))  // стоп таймер
     {
      syslog(LOG_NOTICE,":CommTask:setitimer2:%s\n",strerror(errno));
      perror("setitimer2");
      return(1);
     }
   return(0);
  }
static void handler(int a)
  {
   switch (a)
     {
      case SIGALRM:
      break;
     }
  }
static short OpenTable()      // имя таблицы в b_name
  {
  int l;
  short i,j;
   for(l=0;l<MAX_TABS;l++)
    {
     if(!TabOp[l].t_nam[0])
       break;                 // кончились открытые
     if(!strcmp(b_name,TabOp[l].t_nam))
       return(TabOp[l].t_i);   // таблица уже открыта
    }
   if(l==MAX_TABS)
     return(0x8010);          // TabOp уже полна
   i=opnglb(b_name,"",&j);
   if(i>0)
    {
     TabOp[l].t_i=i;
     strcpy(TabOp[l].t_nam,b_name);
    }
   return(i);
  }
void put_to_m_t(int k)    // добавить в список идентификаторов таблиц измерений
  {
  int i;
   if(ind_m_t)   // meas_tabs не пуста
     {
      for(i=0;i<ind_m_t;i++)
       if(meas_tabs[i]==main_ref->r_to_p[k].sh_tab_id)
        return;      // таблица измерений уже в списке
      if(ind_m_t==MAX_MEAS_TABS)
        {
         syslog(LOG_NOTICE,":CommTask: Слишком много таблиц измерений!!!\n");
         printf(":CommTask: Слишком много таблиц измерений!!!\n");
         exit(0);
        }
     }
   if(main_ref->r_to_p[k].stat_flag)
     stat_meas_tabs[ind_m_t]=1;          // есть статистика измерений
   meas_tabs[ind_m_t]=main_ref->r_to_p[k].sh_tab_id;
   meas_data_tabs[ind_m_t]=main_ref->r_to_p[k].dat_tab_id;
   ++ind_m_t;
   return;
  }
int find_in_m_t()    // найти наличие в MAIN измеряемого параметра
  {
  int a,b;
   for(a=0;a<main_ref->num_of_par;a++)   // по таблицам паспортов
    for(b=0;b<ind_m_t;b++)               // по meas_tabs
     if(meas_tabs[b] == main_ref->r_to_p[a].sh_tab_id)
       return(1);
   return(0);                            // нет измерений
  } 
void set_pas_flags()
  {
  int a,b;
  short flag1, crd[7];
   flag1=pl_stat_m-3;                    // 2-30
   crd[4]=crd[5]=crd[6]=1;               // flag1
   for(a=0;a<main_ref->num_of_par;a++)   // по таблицам паспортов
    if(main_ref->r_to_p[a].stat_flag)    // параметр имеет статистику
     {
      if(main_ref->r_to_p[a].p_pl < 0)
       crd[0]=crd[1]= -main_ref->r_to_p[a].p_pl;
      else
       crd[0]=crd[1]=main_ref->r_to_p[a].p_pl;
      if(main_ref->r_to_p[a].p_tup < 0)
       crd[2]=crd[3]= -main_ref->r_to_p[a].p_tup;
      else
       crd[2]=crd[3]=main_ref->r_to_p[a].p_tup;
      if((i=dtwdbl(&flag1, sizeof(short), crd,main_ref->r_to_p[a].pass_tab_id))<0)  // записал flag1 PASSPORT
        {
         syslog(LOG_NOTICE,":CommTask:err.dtwdbl:flag1 Passport = %x\n", i);
         printf(":CommTask:err.dtwdbl:flag1 Passport = %x\n", i);
        }
     }
  }
short to_plane_1()     // копировать управляющие данные из таблицы в памяти в первую плоскость на диске
  {
  short crd[7],i,a=0;
   crd[0]=crd[1]=crd[2]=crd[6]=1;
   for(i=0;i<main_ref->num_of_par;i++)
    if(main_ref->r_to_p[i].p_pl > 0 && main_ref->r_to_p[i].p_tup > 0)  // управляющие данные
      { 
       crd[3]=main_ref->r_to_p[i].num_of_val;
       crd[4]=crd[5]=main_ref->r_to_p[i].p_tup;                        // копирование по столбцам
       if((a=sh_drtb(buf, sizeof(buf), crd+2, main_ref->r_to_p[i].sh_tab_id))<0) 
         {
          syslog(LOG_NOTICE,":CommTask:to_plane_1:err.sh_drtb = %hx\n", a);
          printf(":CommTask:to_plane_1:err.sh_drtb = %hx\n", a);
          break;
         }
       if((a=dtwdbl(buf, sizeof(buf), crd, main_ref->r_to_p[i].dat_tab_id))<0)
         {
          syslog(LOG_NOTICE,":CommTask:to_plane_1:err.dtwdbl Data = %hx\n", a);
          printf(":CommTask:to_plane_1:err.dtwdbl Data = %hx\n", a);
          break;
         }
      }
   return(a);
  }
void hot_to_mem()     // копировать горячую копию в память для main_ref
  {
  short crd[7],i,a=0;
   crd[0]=crd[1]=crd[2]=crd[6]=1;
   for(i=0;i<main_ref->num_of_par;i++)
    if(main_ref->r_to_p[i].p_pl > 0 && main_ref->r_to_p[i].p_tup > 0)  // управляющие данные
      { 
       crd[3]=main_ref->r_to_p[i].num_of_val;
       crd[4]=crd[5]=main_ref->r_to_p[i].p_tup;                        // копирование по столбцам
       if((a=dtrdbl(buf, sizeof(buf), crd, main_ref->r_to_p[i].dat_tab_id))<0)
         {
          syslog(LOG_NOTICE,":CommTask:hot_to_mem:err.dtrdbl Data = %hx\n", a);
          printf(":CommTask:hot_to_mem:err.dtrdbl Data = %hx\n", a);
          break;
         }
       if((a=sh_dwtb(buf, sizeof(buf), crd+2, main_ref->r_to_p[i].sh_tab_id))<0) 
         {
          syslog(LOG_NOTICE,":CommTask:hot_to_mem:err.sh_dwtb = %hx\n", a);
          printf(":CommTask:hot_to_mem:err.sh_dwtb = %hx\n", a);
          break;
         }
      }
  }
main()
  {
  int ic,ic0;
   sig_dat.sival_int=0;
   sh_ECSini(1);                    // созданы общая память и сигнал SIGUSR1
   newval.it_interval.tv_sec=0;
   newval.it_interval.tv_usec=0;
   newval.it_value.tv_sec=0;
   newval.it_value.tv_usec=0;
   if(setitimer(ITIMER_REAL,&newval,0))
     {
      syslog(LOG_NOTICE,":CommTask:setitimer:%s\n",strerror(errno));
      perror("setitimer");
      exit(0);
     }
   if(sigemptyset(&maska))          // SIGALRM используется с таймерами
     {
      syslog(LOG_NOTICE,":CommTask:sigemptyset:%s\n",strerror(errno));
      perror("sigemptyset");
      exit(0);
     }
   if(sigaddset(&maska,SIGALRM))
     {
      syslog(LOG_NOTICE,":CommTask:sigaddset:%s\n",strerror(errno));
      perror("sigaddset");
      exit(0);
     }
   xa.sa_handler=handler;
   xa.sa_flags=SA_RESTART;
   if(sigaction(SIGALRM,&xa,NULL))  // set handler
     {
      syslog(LOG_NOTICE,":CommTask:sigaction:%s\n",strerror(errno));
      perror("sigaction");
      exit(0);
     }
   Xt_al_ini();

   if(!(fpoi=fopen(T_names,"r")))         // файл tabs_main.ecs для подсчёта числа экранов
     {
      syslog(LOG_NOTICE,":CommTask:fopen:%s %s\n", T_names,strerror(errno));
      perror("fopen:");
      exit(0);
     }
   num_of_main=0;
   while(fgets(buf,sizeof(buf),fpoi))     // подсчитать число экранов
     if(buf[0]!=0 && buf[0]!='\n')
       ++num_of_main;
   fclose(fpoi);
   if(!(fpoi=fopen(T_names,"r")))         // файл tabs_main.ecs для чтения имён MAIN таблиц
     {
      syslog(LOG_NOTICE,":CommTask:fopen:%s\n",strerror(errno));
      perror("fopen:");
      exit(0);
     }
   to_mem1=malloc(num_of_main*sizeof(struct my_ref_to_main));
   if(!to_mem1)
     {
      syslog(LOG_NOTICE,":CommTask:malloc:%s\n",strerror(errno));
      perror("malloc:");
      exit(0);
     }
   memset(to_mem1,0,num_of_main*sizeof(struct my_ref_to_main));
   memset(stat_meas_tabs,0,sizeof(stat_meas_tabs));
   memset(TabOp,0,sizeof(TabOp));
   main_ref=(struct my_ref_to_main *)to_mem1;
   while(fgets(buf,sizeof(buf),fpoi))     // buf = имя таблицы MAIN
     {
      if(buf[0]==0 || buf[0]=='\n')
        break;        // из while
      memset(b_name,0,sizeof(b_name));
      sscanf(buf, "%s %d %d", b_name, &p, &t);
      if((main_ref->main_tab_id=OpenTable())<0)  // открыл таблицу MAIN
        {
         syslog(LOG_NOTICE,":CommTask:err.open MAIN '%s' = %x\n",b_name, main_ref->main_tab_id);
         printf(":CommTask:err.open '%s' = %x\n",b_name, main_ref->main_tab_id);
         exit(0);
        }
      if((i=rlpdbl(&r_opi,main_ref->main_tab_id))<0)         // заполнил структуру relis
        {
         syslog(LOG_NOTICE,":CommTask:err.rlpdbl '%s' = %x\n",b_name,i);
         printf(":CommTask:err.rlpdbl '%s' = %x\n",b_name,i);
         exit(0);
        }
      main_ref->m_pl=coord[0]=coord[1]=p;
      main_ref->m_tup=coord[2]=coord[3]=t;
      coord[4]=1;
      coord[5]=r_opi.nat;
      coord[6]=1;
      if((i=dtrdbl(&For_MAIN, sizeof(For_MAIN), coord,main_ref->main_tab_id))<0)  // прочитал строку MAIN
        {
         syslog(LOG_NOTICE,":CommTask:err.dtrdbl '%s' = %x\n",b_name, i);
         printf(":CommTask:err.dtrdbl '%s' = %x\n",b_name, i);
         exit(0);
        }
      main_ref->num_of_par=For_MAIN.par_num;
      for(k=0;k<For_MAIN.par_num;k++)            // таблицы паспортов одного экрана
        {
         memset(b_name,0,sizeof(b_name));
         strncpy(b_name,For_MAIN.params[k].PASS_tab,sizeof(For_MAIN.params[0].PASS_tab));
         if((main_ref->r_to_p[k].pass_tab_id=OpenTable())<0)  // открыл таблицу паспортов
           {
            syslog(LOG_NOTICE,":CommTask:err.open '%s' = %x\n",b_name, main_ref->r_to_p[k].pass_tab_id);
            printf(":CommTask:err.open PASSPORT '%s' = %x\n",b_name, main_ref->r_to_p[k].pass_tab_id);
            exit(0);
           }
         if((i=rlpdbl(&r_opi,main_ref->r_to_p[k].pass_tab_id))<0)         // заполнил структуру relis
           {
            syslog(LOG_NOTICE,":CommTask:err.rlpdbl '%s' = %x\n",b_name,i);
            printf(":CommTask:err.rlpdbl '%s' = %x\n",b_name,i);
            exit(0);
           }
         main_ref->r_to_p[k].p_pl  = For_MAIN.params[k].plane;   // может быть < 0
         main_ref->r_to_p[k].p_tup = For_MAIN.params[k].tuple;   // может быть < 0
         if(For_MAIN.params[k].plane > 0)
           coord[0]=coord[1]=For_MAIN.params[k].plane;
         else
           coord[0]=coord[1]= -For_MAIN.params[k].plane;
         if(For_MAIN.params[k].tuple > 0)
           coord[2]=coord[3]=For_MAIN.params[k].tuple;
         else
           coord[2]=coord[3]= -For_MAIN.params[k].tuple;
         coord[4]=1;
         coord[5]=r_opi.nat;
         if((i=dtrdbl(&For_Passport, sizeof(For_Passport), coord, main_ref->r_to_p[k].pass_tab_id))<0)  // прочитал PASSPORT
           {
            syslog(LOG_NOTICE,":CommTask:err.dtrdbl Passport '%s' = %x\n",b_name, i);
            printf(":CommTask:err.dtrdbl Passport '%s' = %x\n",b_name,i);
            exit(0);
           }
         main_ref->r_to_p[k].num_of_val=For_Passport.num_items;
         if((For_Passport.par_type&6)==6)      // измеряемый + статистика
           main_ref->r_to_p[k].stat_flag=1;    // у параметра есть статистика измерений
         memset(b_name,0,sizeof(b_name));
         strncpy(b_name,For_MAIN.params[k].DATA_tab,sizeof(For_MAIN.params[0].DATA_tab));
         if((main_ref->r_to_p[k].dat_tab_id=OpenTable())<0)  // открыл таблицу данных
           {
            syslog(LOG_NOTICE,":CommTask:err.open '%s' = %x\n",b_name, main_ref->r_to_p[k].dat_tab_id);
            printf(":CommTask:err.open DATA '%s' = %x\n",b_name, main_ref->r_to_p[k].dat_tab_id);
            exit(0);
           }
         if((main_ref->r_to_p[k].sh_tab_id=sh_optb(b_name))<0)        // открываю таблицу данных в памяти
           {
            printf("В памяти нет таблицы %s\n",b_name);
            main_ref->r_to_p[k].sh_tab_id=0;
           }
         if(For_MAIN.params[k].tuple < 0 && main_ref->r_to_p[k].sh_tab_id) // измеряемый параметр, запомнить в meas_tabs
           put_to_m_t(k);              
        }   // for
      ++main_ref;
     }                   //  Заполнена структура my_ref_to_main для num_of_main таблиц
   fclose(fpoi);
   to_sh_mem=get_sh_addr();
   to_sh_start = (struct sh_start *)to_sh_mem;
   Xt_al_text(svt,"У-70:Т-таймер и имитатор:СКО инициирована\n");
   sh_put_mess("Коммуникационная задача CommTask успешно стартовала");
/*
for(ic=0;ic<MAX_TABS;ic++)
 if(TabOp[ic].t_nam[0])
  printf("%s ==> %hd\n",TabOp[ic].t_nam,TabOp[ic].t_i);
for(ic=0;ic<ind_m_t;ic++)
 printf("Диск %hd == Память %hd Статистика %hhd\n",meas_data_tabs[ic],meas_tabs[ic],stat_meas_tabs[ic]);
main_ref=(struct my_ref_to_main *)to_mem1;
for(ic=0;ic<num_of_main;ic++)
 {
  printf("Main = %hd\n  Passports\n",main_ref->main_tab_id);
  for(ic0=0;ic0<main_ref->num_of_par;ic0++)
   printf("    Паспорт %hd == Диск %hd == Память %hd\n",
    main_ref->r_to_p[ic0].pass_tab_id,main_ref->r_to_p[ic0].dat_tab_id,main_ref->r_to_p[ic0].sh_tab_id);
  ++main_ref;
 }
*/
BigCyc:                  // большой цикл ожидания и проверки изменений наверху/внизу

   time_wait(300);       // 300 ms
   main_ref=(struct my_ref_to_main *)to_mem1;
   flag_to_sh=0;
   for(ic=0;ic<num_of_main;ic++)
     {
      if( !main_ref->main_state)    // если состояние ready
        see_main_tabs();            // смотреть новости сверху от программы U-70
      ++main_ref;
     }
   if(flag_to_sh)                   // была запись новых управляющих в таблицы в памяти
     {
      ++to_sh_start->S_put_sig;
      if(msync(&to_sh_start->S_put_sig, sizeof(int), MS_SYNC | MS_INVALIDATE))
        if(errno != EINVAL)
          perror("S_put_sig:msync: ");
//      if(sigqueue(to_sh_start->cntr_pid, SIGUSR1, sig_dat))  //  сигнал управляющей задаче
//        perror("sigqueue управляющей");
     }
   if(Count_from_cntr != to_sh_start->C_count)   // есть новости измерения
     {
      Count_from_cntr = to_sh_start->C_count;
      if(to_sh_start->err_meas < 0)              // есть ошибка в измерениях
        {
         main_ref=(struct my_ref_to_main *)to_mem1;
         coord[5]=coord[4]=1;             // flag0
         for(ic=0;ic<num_of_main;ic++)    // писать во все MAIN 
           {
            coord[0]=coord[1]=main_ref->m_pl;
            coord[2]=coord[3]=main_ref->m_tup;
            if((i=dtwdbl(&to_sh_start->err_meas, sizeof(short), coord,main_ref->main_tab_id))<0)  // записал flag0 MAIN
             {
              syslog(LOG_NOTICE,":CommTask:err.dtwdbl:main:flag0 = %x\n", i);
              printf(":CommTask:err.dtwdbl:main:flag0 = %x\n", i);
             }
            ++main_ref;
           }
        }
      else
        {
         main_ref=(struct my_ref_to_main *)to_mem1;
         for(ic=0,ic0=0;ic<ind_m_t;ic++)        // по всем таблицам измерений 
           {
            if(sh_lptb((char*)&r_m_op,meas_tabs[ic]))
              {
               syslog(LOG_NOTICE,":CommTask:err.sh_lptb: index=%d tab_id=%d\n", ic,meas_tabs[ic]);
               printf(":CommTask:err.sh_lptb: index=%d tab_id=%d\n", ic,meas_tabs[ic]);
              }
            else
              {
               coord[0]=coord[1]=coord[2]=coord[4]=1;
               coord[3]=r_m_op.n_tup;      // всю таблицу в памяти
               coord[5]=r_m_op.n_attr;
               p=sizeof(buf);
               if(p < r_m_op.n_tup*r_m_op.tup_len)
                {
                 syslog(LOG_NOTICE,":CommTask: слишком большой размер таблицы измерений =%d \n", r_m_op.n_tup*r_m_op.tup_len);
                 printf(":CommTask: слишком большой размер таблицы измерений =%d \n", r_m_op.n_tup*r_m_op.tup_len);
                }
               else
                 p=r_m_op.n_tup*r_m_op.tup_len;
               if(sh_drtb(buf, p, coord+2, meas_tabs[ic])<0)
                 {
                  syslog(LOG_NOTICE,":CommTask:err.sh_drtb: index=%d tab_id=%d\n", ic,meas_tabs[ic]);
                  printf(":CommTask:err.sh_drtb: index=%d tab_id=%d\n", ic,meas_tabs[ic]);
                 }
               if((i=dtwdbl(buf, p, coord,meas_data_tabs[ic]))<0)     // в первую плоскость на диске
                {
                 syslog(LOG_NOTICE,":CommTask:err.dtwdbl:измерения  index=%d tab_id=%d %x\n", ic, meas_data_tabs[ic], i);
                 printf(":CommTask:err.dtwdbl:измерения  index=%d tab_id=%d %x\n", ic, meas_data_tabs[ic], i);
                }
               if(stat_meas_tabs[ic])                                 // есть статистика измерений
                {
                 ic0=1;
                 coord[0]=coord[1]=pl_stat_m;
                 if((i=dtwdbl(buf, p, coord,meas_data_tabs[ic]))<0)   // в плоскости 5-33 на диске
                  {
                   syslog(LOG_NOTICE,":CommTask:err.dtwdbl:статистика измерений  index=%d tab_id=%d %x\n", ic, meas_data_tabs[ic], i);
                   printf(":CommTask:err.dtwdbl:статистика измерений  index=%d tab_id=%d %x\n", ic, meas_data_tabs[ic], i);
                  }
                }
              }
           }
         coord[5]=coord[4]=1;             // flag0
         for(ic=0;ic<num_of_main;ic++)    // по всем MAIN 
          {
           if(find_in_m_t())              // если есть измеряемые данные в этом экране
            {
             if(ic0)                      //  была запись статистики измерений
              set_pas_flags();
             coord[0]=coord[1]=main_ref->m_pl;
             coord[2]=coord[3]=main_ref->m_tup;
             if((i=dtrdbl(&For_MAIN.flag0, sizeof(short), coord,main_ref->main_tab_id))<0)  // прочитал flag0 MAIN
              {
               syslog(LOG_NOTICE,":CommTask:err.dtrdbl:flag0 = %x\n", i);
               printf(":CommTask:err.dtrdbl:flag0 = %x\n", i);
              }
             For_MAIN.flag0=(For_MAIN.flag0+1)&255;
             if((i=dtwdbl(&For_MAIN.flag0, sizeof(short), coord,main_ref->main_tab_id))<0)  // записал flag0 MAIN
              {
               syslog(LOG_NOTICE,":CommTask:err.dtwdbl:flag0 = %x\n", i);
               printf(":CommTask:err.dtwdbl:flag0 = %x\n", i);
              }
            }
           ++main_ref;
          }
         if(ic0)                    //  была запись статистики измерений
          {
           ++pl_stat_m;
           if(pl_stat_m==34)
             pl_stat_m=5;
          }
        }
     }
   if(Count_cntr != to_sh_start->C_put_count)   // есть новости управления от управляющей программы
     {
      Count_cntr = to_sh_start->C_put_count;
      main_ref=(struct my_ref_to_main *)to_mem1;
      coord[4]=2;
      coord[5]=3;                       // flag1 + flag2
      for(ic=0;ic<num_of_main;ic++)     // во все MAIN 
       {
        if(main_ref->main_state)        // только для ждущих ответа
         {
          coord[0]=coord[1]=main_ref->m_pl;
          coord[2]=coord[3]=main_ref->m_tup;
          if(to_sh_start->err_data < 0) // есть ошибка в управляющих/восстановить горячую копию в памяти
            {
             hot_to_mem();
             For_MAIN.flag1=to_sh_start->err_data;
            }
          else
            {
             if(!(i=to_plane_1()))  // управляющие данные из памяти записаны в первую плоскость
              {
               if((i=dtrdbl(&For_MAIN.flag1, sizeof(short), coord,main_ref->main_tab_id))<0)  // прочитал flag1 MAIN
                {
                 syslog(LOG_NOTICE,":CommTask:err.dtrdbl:flag1 = %x\n", i);
                 printf(":CommTask:err.dtrdbl:flag1 = %x\n", i);
                 For_MAIN.flag1=i;
                }
               else
                 For_MAIN.flag1=(For_MAIN.flag1+1)&255;
              }
             else
               For_MAIN.flag1=i;   // ошибка из to_plane_1
            }
          For_MAIN.flag2=0;
          if((i=dtwdbl(&For_MAIN.flag1, 2*sizeof(short), coord,main_ref->main_tab_id))<0)     // записал flag1&flag2 MAIN
           {
            syslog(LOG_NOTICE,":CommTask:err.dtwdbl:flag1 = %x\n", i);
            printf(":CommTask:err.dtwdbl:flag1 = %x\n", i);
           }
          main_ref->main_state=0;       // ready
         }
        ++main_ref;
       }
     }
   goto BigCyc;
  }
static int see_main_tabs()          // работать с MAIN таблицей, указанной в main_ref
  {
   if(!main_ref->main_tab_id)       // таблица не открыта
     return 0;
   coord[0]=coord[1]=main_ref->m_pl;
   coord[2]=coord[3]=main_ref->m_tup;
   coord[4]=1;
   coord[5]=3;                      // flag0, flag1, flag2
   coord[6]=1;
   if((i=dtrdbl(&For_MAIN, sizeof(For_MAIN), coord, main_ref->main_tab_id))<0)  // прочитал флаги MAIN
     {
      syslog(LOG_NOTICE,":CommTask:err.dtrdbl MAIN = %x\n", i);
      printf(":CommTask:err.dtrdbl MAIN = %x\n", i);
      return 0;
     }
   if((For_MAIN.flag2 & 0x81) == 0x81)     // есть новые управляющие данные
     {
      for(k=0;k<main_ref->num_of_par;k++)  // если таблицы паспортов и данных открыты
       if(main_ref->r_to_p[k].pass_tab_id && main_ref->r_to_p[k].dat_tab_id && main_ref->r_to_p[k].sh_tab_id)
        {
         if(main_ref->r_to_p[k].p_tup < 0)
           continue;                       // нередактируемые данные
         if(main_ref->r_to_p[k].p_pl < 0)
           continue;                       // эталонные данные
         coord[0]=coord[1]=main_ref->r_to_p[k].p_pl;
         coord[2]=coord[3]=main_ref->r_to_p[k].p_tup;
         coord[4]=1;
         coord[5]=2;                       // flag1, flag2
         if((i=dtrdbl(&For_Passport, sizeof(For_Passport), coord, main_ref->r_to_p[k].pass_tab_id))<0)  // прочитал флаги PASSPORT
           {
            syslog(LOG_NOTICE,":CommTask:err.dtrdbl Passport = %x\n", i);
            printf(":CommTask:err.dtrdbl Passport = %x\n", i);
er_exit:
            For_MAIN.flag2=0;      // выход из функции
            For_MAIN.flag1=i;
            coord[0]=coord[1]=main_ref->m_pl;
            coord[2]=coord[3]=main_ref->m_tup;
            coord[4]=2;   
            coord[5]=3;
            if((i=dtwdbl(&For_MAIN.flag1, sizeof(For_MAIN), coord, main_ref->main_tab_id))<0)  // записал флаги flag1, flag2 MAIN
             {
              syslog(LOG_NOTICE,":CommTask:err.dtwdbl MAIN flag1&flag2 = %x\n", i);
              printf(":CommTask:err.dtwdbl MAIN flag1&flag2 = %x\n", i);
             }
            return 0;
           }
         if(For_Passport.flag2 & 1)        // есть новые управляющие данные
           {
            coord[0]=coord[1]=2;           // данные из плоскости 2
            coord[2]=1;
            coord[3]=main_ref->r_to_p[k].num_of_val;
            coord[4]=coord[5]=main_ref->r_to_p[k].p_tup;  // номер столбца в таблице данных
            if((i=dtrdbl(buf, sizeof(buf), coord, main_ref->r_to_p[k].dat_tab_id))<0)  // прочитал столбец данных
             {
              syslog(LOG_NOTICE,":CommTask:err.dtrdbl Data = %x\n", i);
              printf(":CommTask:err.dtrdbl Data = %x\n", i);
              goto er_exit;
             }
            if((i=sh_dwtb(buf, sizeof(buf), coord+2, main_ref->r_to_p[k].sh_tab_id))<0) // записал в общую память
             {
              syslog(LOG_NOTICE,":CommTask:err.sh_dwtb = %x\n", i);
              printf(":CommTask:err.sh_dwtb = %x\n", i);
              goto er_exit;
             }
            main_ref->main_state=flag_to_sh=1;
            For_Passport.flag2=0;
            coord[0]=coord[1]=main_ref->r_to_p[k].p_pl;
            coord[2]=coord[3]=main_ref->r_to_p[k].p_tup;
            coord[4]=coord[5]=2;            // flag2
            if((i=dtwdbl(&For_Passport.flag2, sizeof(short), coord, main_ref->r_to_p[k].pass_tab_id))<0)  // обнулил flag2 PASSPORT
             {
              syslog(LOG_NOTICE,":CommTask:err.dtwdbl Passport = %x\n", i);
              printf(":CommTask:err.dtwdbl Passport = %x\n", i);
              goto er_exit;
             }
           }
        }
     }
   else
    if(For_MAIN.flag2 & 6)               // игнорирую Execute и копирование режимов
      {
       For_MAIN.flag2=0;
       For_MAIN.flag1=(For_MAIN.flag1+1)&255;
       coord[4]=2;
       if((i=dtwdbl(&For_MAIN.flag1, 2*sizeof(short), coord, main_ref->main_tab_id))<0)  // записал флаги flag1, flag2 MAIN
        {
         syslog(LOG_NOTICE,":CommTask:err.dtwdbl MAIN flag1&flag2 = %x\n", i);
         printf(":CommTask:err.dtwdbl MAIN flag1&flag2 = %x\n", i);
         return 0;
        }
      }
   return 1;    // выход без сбоев
  }

