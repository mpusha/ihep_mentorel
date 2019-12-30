#include "../sh_ccydalib/sh_ccydalib.h"

static struct itimerval newval;
static sigset_t maska;
static struct sigaction xa;

static char *Ch_names[]={        // имена каналов - используются первые 8 байт !
	"Chan-01",
	"Chan-02",
	"Chan-03",
	"Chan-04",
	"Chan-05",
	"Chan-06",
	"Chan-07",
	"Chan-08",
	"Chan-09",
	"Chan-10",
	"Chan-11",
	"Chan-12",
	"Chan-13",
	"Chan-14",
	"Chan-15",
	"Chan-16",
	"Chan-17",
	"Chan-18",
	"Chan-19",
	"Chan-20",
	"Chan-21",
	"Chan-22",
	"Chan-23",
	"Chan-24"
      };
// столбцы таблицы M_PD_ECS_TTIM_CHAN, строк 24  - массив структур не идёт - размер структуры кратен 8
   double cur_t[24], new_t[24];
   char cur_OO[24], new_OO[24];
   char cur_Strt[24], new_Strt[24];
static struct Imita {            // одна структура строки таблицы M_PD_ECS_TIMIT, строка 1
   double NC;
   double B1;
   double B2;
   double KC1;
   double KC2;
   double Tcycle;
   char Mode;
   char Regim;
  } cur_par, new_par;
static char Cur_stat[4];         // строка таблицы M_PD_ECS_TTIM_STAT состоит из 1 элемента, строк 4
static struct k_opis {
   char r_name[20];
   unsigned short n_tup;    // число строк
   unsigned short n_attr;   // число столбцов
   unsigned short tup_len;  // длина строки
   unsigned short last_act; // последнее действие 0 - чтение, 0xcccc - запись связной, 0xaaaa - запись управляющей
  } buf;
static short i, coord[4], my_cy, was_err; // my_cy - якобы регистр состояния, was_err - код ошибки внутри большого цикла
static char *tb_names[]={
	"M_PD_ECS_TTIM_COMM",   // имена каналов по 8 байт
	"M_PD_ECS_TTIM_CHAN",   // 3 параметра управления по 24 значения (1 double, 2 char)
	"M_PD_ECS_TIMIT",       // 8 параметров управления по 1 значению (6 double, 2 char
	"M_PD_ECS_TTIM_STAT"    // значения 4-х бит в char регистра состояния
                                // добавление к коду 0x80 ==> текст выводится красным цветом
      };
#define num_of_tabs sizeof(tb_names)/sizeof(tb_names[0])

static short tab_ids[num_of_tabs];
static unsigned int j, Nc, up_count;

static int time_wait(int ms)              // задержка в миллисекундах
  {
  int sig;
   newval.it_value.tv_sec=ms/1000;        // секунды
   newval.it_value.tv_usec=ms%1000*1000;  // микросекунды
   if(setitimer(ITIMER_REAL,&newval,0))
     {
      perror("setitimer");
      return(1);
     }
   if(sigwait(&maska,&sig))
     {
      perror("sigwait");
      return(1);
     }
   newval.it_value.tv_sec=0;  
   newval.it_value.tv_usec=0; 
   if(setitimer(ITIMER_REAL,&newval,0))  // стоп таймер
     {
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
void set_state()   // по 4-м битам заполняет таблицу M_PD_ECS_TTIM_STAT
  {
  int c;
  short k;
   if(my_cy & 8)
     Cur_stat[1]=0x85;        // Imitation
   else
     Cur_stat[1]=6;           // Работа
   if(my_cy & 4)
     Cur_stat[2]=0x84;        // Imitator error
   else
     Cur_stat[2]=2;           // -
   if(my_cy & 2)
     Cur_stat[0]=0x81;        // Ручной
   else
     Cur_stat[0]=0;           // Автом.
   if(my_cy & 1)
     Cur_stat[3]=0x83;        // Timer error
   else
     Cur_stat[3]=2;           // -

   coord[0]=coord[2]=coord[3]=1;
   coord[1]=4;
   i=sh_dwtb(Cur_stat ,sizeof(Cur_stat), coord, tab_ids[3]);  // M_PD_ECS_TTIM_STAT
   if(i<0)
     {
      printf("Ошибка %x записи в таблицу M_PD_ECS_TTIM_STAT\n",i);
      was_err=i;
     }
  }

main()
  {
   newval.it_interval.tv_sec=0;
   newval.it_interval.tv_usec=0;
   newval.it_value.tv_sec=0;
   newval.it_value.tv_usec=0;
   if(setitimer(ITIMER_REAL,&newval,0))
     {
      syslog(LOG_NOTICE,":test:setitimer:%s\n",strerror(errno));
      perror("setitimer");
      exit(0);
     }
   if(sigemptyset(&maska))          // SIGALRM используется с таймерами
     {
      syslog(LOG_NOTICE,":test:sigemptyset:%s\n",strerror(errno));
      perror("sigemptyset");
      exit(0);
     }
   if(sigaddset(&maska,SIGALRM))
     {
      syslog(LOG_NOTICE,":test:sigaddset:%s\n",strerror(errno));
      perror("sigaddset");
      exit(0);
     }
   xa.sa_handler=handler;
   xa.sa_flags=SA_RESTART;
   if(sigaction(SIGALRM,&xa,NULL))  // set handler
     {
      syslog(LOG_NOTICE,":test:sigaction:%s\n",strerror(errno));
      perror("sigaction");
      exit(0);
     }

   if(i=API_ECSini())  // инициализация связи со связной программой
     {
      printf("Ошибка при вызове API_ECSini = %d\n", i);
      if(i=API_ECSini())
       {
        syslog(LOG_NOTICE,":test:Вторая ошибка при вызове API_ECSini = %d\n", i);
        printf("Вторая ошибка при вызове API_ECSini = %d\n", i);
        exit(0);
       }
     }
   for(j=0;j<num_of_tabs;j++)  // открыть все таблицы
     {
      i=sh_optb(tb_names[j]);
      if(i<0)
        {
         tab_ids[j]=0;
         sh_put_meas_er(i);   // код ошибки
         sh_put_data_er(i);
         printf("Ошибка %x открытия таблицы %s\n",i,tb_names[j]);
         continue;
        }
      tab_ids[j]=i;
     }

   coord[0]=coord[2]=coord[3]=1;     // first tuple/attr
   coord[1]=24;                      // last tuple
   i=sh_drtb((char *)&cur_t ,sizeof(cur_t), coord, tab_ids[1]);       // M_PD_ECS_TTIM_CHAN  attr 1
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x чтения таблицы M_PD_ECS_TTIM_CHAN\n",i);
     }
   coord[2]=coord[3]=2;     // attr 2
   i=sh_drtb((char *)&cur_OO ,sizeof(cur_OO), coord, tab_ids[1]);     // M_PD_ECS_TTIM_CHAN  attr 2
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x чтения таблицы M_PD_ECS_TTIM_CHAN\n",i);
     }
   coord[2]=coord[3]=3;     // attr 3
   i=sh_drtb((char *)&cur_Strt ,sizeof(cur_Strt), coord, tab_ids[1]); // M_PD_ECS_TTIM_CHAN  attr 3
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x чтения таблицы M_PD_ECS_TTIM_CHAN\n",i);
     }

   coord[1]=coord[2]=1;
   coord[3]=8;
   i=sh_drtb((char *)&cur_par ,sizeof(cur_par), coord, tab_ids[2]);  // M_PD_ECS_TIMIT
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x чтения таблицы M_PD_ECS_TIMIT\n",i);
     }

//  открыты все таблицы, управляющие данные прочитаны
//  ЗДЕСЬ надо прописать аппаратуру последними данными, которые были сохранены в БД
   my_cy=was_err=0;         // первичный статус
   set_state();
   if(!was_err) 
     sh_put_meas_er(0);     // сообщаю о нормальном состоянии системы
   else
     sh_put_meas_er(was_err);
   up_count=sh_get_count(); // циклический счётчик действий сверху
   Nc=0;
   sh_put_mess("Тестовая задача sh_test начинает работу");

   coord[0]=coord[1]=coord[2]=coord[3]=1;
   for(j=0; j<24; j++)  // записать имена каналов в таблицу
     {
      i=sh_dwtb(Ch_names[j] ,8, coord, tab_ids[0]);  // M_PD_ECS_TTIM_COMM
      if(i<0)
        {
         sh_put_data_er(i);
         printf("Ошибка %x записи в таблицу M_PD_ECS_TTIM_COMM \n",i);
         break; 
        }
      else
        sh_put_meas_er(0);  
      coord[1] += 1;
      coord[0]=coord[1];
     }

   while(1)                 // большой постоянно действующий цикл !!!
    {
     was_err=0;
     time_wait(300);        // ~33 раза за 10 секунд
     j=sh_get_count();      // циклический счётчик действий сверху
     if(j!=up_count)        // есть новые данные сверху
      {
       up_count=j;
// 1
       coord[0]=coord[2]=1;
       coord[1]=1;
       coord[3]=8;
       i=sh_drtb((char *)&new_par ,sizeof(new_par), coord, tab_ids[2]);  // M_PD_ECS_TIMIT
       if(i<0)
        {
         was_err=i;
         sh_put_data_er(i);
         printf("Ошибка %x чтения таблицы M_PD_ECS_TIMIT\n",i);
         continue;    // Ошибка БД - это криминал
        }

       if(cur_par.NC != new_par.NC)
        {
         printf("Параметр StartNC был %lg стал %lg\n",cur_par.NC,new_par.NC);
         cur_par.NC=new_par.NC;
        }
       if(cur_par.B1 != new_par.B1)
        {
         printf("Параметр StartB1 был %lg стал %lg\n",cur_par.B1,new_par.B1);
         cur_par.B1=new_par.B1;
        }
       if(cur_par.B2 != new_par.B2)
        {
         printf("Параметр StartB2 был %lg стал %lg\n",cur_par.B2,new_par.B2);
         cur_par.B2=new_par.B2;
        }
       if(cur_par.KC1 != new_par.KC1)
        {
         printf("Параметр StartKC1 был %lg стал %lg\n",cur_par.KC1,new_par.KC1);
         cur_par.KC1=new_par.KC1;
        }
       if(cur_par.KC2 != new_par.KC2)
        {
         printf("Параметр StartKC2 был %lg стал %lg\n",cur_par.KC2,new_par.KC2);
         cur_par.KC2=new_par.KC2;
        }
       if(cur_par.Tcycle != new_par.Tcycle)
        {
         printf("Параметр Tcycle был %lg стал %lg\n",cur_par.Tcycle,new_par.Tcycle);
         cur_par.Tcycle=new_par.Tcycle;
        }
       if(cur_par.Mode != new_par.Mode)
        {
         printf("Параметр Mode был %d стал %d\n",cur_par.Mode&255,new_par.Mode&255);
         cur_par.Mode=new_par.Mode;
        }
       if(cur_par.Regim != new_par.Regim)
        {
         printf("Параметр Regime был %d стал %d\n",cur_par.Regim&255,new_par.Regim&255);
         cur_par.Regim=new_par.Regim;
        }
// 2
       coord[0]=coord[2]=coord[3]=1;     // first tuple/attr
       coord[1]=24;                      // last tuple
       i=sh_drtb((char *)&new_t ,sizeof(new_t), coord, tab_ids[1]);       // M_PD_ECS_TTIM_CHAN  attr 1
       if(i<0)
        {
         was_err=i;
         sh_put_data_er(i);
         printf("Ошибка %x чтения таблицы M_PD_ECS_TTIM_CHAN\n",i);
         continue;    // Ошибка БД - это криминал
        }
       coord[2]=coord[3]=2;              // attr 2
       i=sh_drtb((char *)&new_OO ,sizeof(new_OO), coord, tab_ids[1]);     // M_PD_ECS_TTIM_CHAN  attr 2
       if(i<0)
        {
         was_err=i;
         sh_put_data_er(i);
         printf("Ошибка %x чтения таблицы M_PD_ECS_TTIM_CHAN\n",i);
         continue;    // Ошибка БД - это криминал
        }
       coord[2]=coord[3]=3;              // attr 3
       i=sh_drtb((char *)&new_Strt ,sizeof(new_Strt), coord, tab_ids[1]); // M_PD_ECS_TTIM_CHAN  attr 3
       if(i<0)
        {
         was_err=i;
         sh_put_data_er(i);
         printf("Ошибка %x чтения таблицы M_PD_ECS_TTIM_CHAN\n",i);
         continue;    // Ошибка БД - это криминал
        }

       for(j=0;j<24;j++)      // для 24 значений
        {
         if(cur_t[j] != new_t[j])
          {
           printf("Параметр Time[%d] был %lg стал %lg\n",j,cur_t[j],new_t[j]);
           cur_t[j]=new_t[j];
          }
         if(cur_OO[j] != new_OO[j])
          {
           printf("Параметр OnOff[%d] был %d стал %d\n",j,cur_OO[j]&255,new_OO[j]&255);
           cur_OO[j]=new_OO[j];
          }
         if(cur_Strt[j] != new_Strt[j])
          {
           printf("Параметр StartFrom[%d] был %d стал %d\n",j,cur_Strt[j]&255,new_Strt[j]&255);
           cur_Strt[j]=new_Strt[j];
          }
        }
// здусь нужна проверка введённых данных 

       if(!was_err)
        sh_put_data_er(0);  // подтверждение правильности введённых данных
      }

// здесь измерения в таблицу 
     ++Nc;
     if(Nc==10)          // меняю состояние 
      {
       Nc=0;
       ++my_cy;
       my_cy &= 15;
       set_state();
       if(!was_err) 
         sh_put_meas_er(0);     // сообщаю о нормальном состоянии системы
       else
         sh_put_meas_er(was_err);
      }
    }
  }

