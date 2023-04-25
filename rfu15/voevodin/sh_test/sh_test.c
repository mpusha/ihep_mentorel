#include "../../voevodin/sh_ccydalib/sh_ccydalib.h"

static struct itimerval newval;
static sigset_t maska;
static struct sigaction xa;

// данные измеряемые
static double time_a[32];    // время измеренное
static char   stat_a[8];     // состояние 
static char   err_a[8];      // ошибки

// данные управляющие
static double leng_a[32], was_leng_a[32];    // длительность
static double rf_a[32], was_rf_a[32];        // амплитуда
static char   forc_a, was_forc_a;            // форсирование

//
static struct k_opis {
   char r_name[20];
   unsigned short n_tup;    // число строк
   unsigned short n_attr;   // число столбцов
   unsigned short tup_len;  // длина строки
   unsigned short last_act; // последнее действие 0 - чтение, 0xcccc - запись связной, 0xaaaa - запись управляющей
  } buf;
static short i, coord[4], was_err; // was_err - код ошибки внутри большого цикла
static char a_a[]="abcdefghijklmnopqrst";

static char *tb_names[]={
	"B_PD_ECS_A_RF_MEAS",   // 0
     	"B_PD_ECS_A_RF_CNTR",   // 1
      };
#define num_of_tabs sizeof(tb_names)/sizeof(tb_names[0])

static short tab_ids[num_of_tabs];
static unsigned int j, Nc, up_count;
static char *c_param[]={
         "Длина вектора",
         "Амплитуда",
         "Форсирование"
      };

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

void read_controls()  // чтение столбцов таблицы B_PD_ECS_A_RF_CNTR
  {
//   длительность
   coord[0]=1;  // первая строка
   coord[1]=32; // последняя строка
   coord[2]=1;  // первый столбец
   coord[3]=1;  // последний столбец
   i=sh_drtb((char *)leng_a ,sizeof(leng_a), coord, tab_ids[1]);  // B_PD_ECS_A_RF_CNTR
   if(i<0)
     {
      sh_put_data_er(i);
      printf("1.Ошибка %x чтения таблицы %s\n",i,tb_names[1]);
      exit(0);
     }
//   амплитуда
   coord[0]=1;  // первая строка
   coord[1]=32; // последняя строка
   coord[2]=2;  // первый столбец
   coord[3]=2;  // последний столбец
   i=sh_drtb((char *) rf_a,sizeof(rf_a), coord, tab_ids[1]);  // B_PD_ECS_A_RF_CNTR
   if(i<0)
     {
      sh_put_data_er(i);
      printf("2.Ошибка %x чтения таблицы %s\n",i,tb_names[1]);
      exit(0);
     }
//   форсирование
   coord[0]=1;  // первая строка
   coord[1]=1;  // последняя строка
   coord[2]=3;  // первый столбец
   coord[3]=3;  // последний столбец
   i=sh_drtb(&forc_a ,sizeof(forc_a), coord, tab_ids[1]);  // B_PD_ECS_A_RF_CNTR
   if(i<0)
     {
      sh_put_data_er(i);
      printf("1.Ошибка %x чтения таблицы %s\n",i,tb_names[1]);
      exit(0);
     }
  }

void writeTime(void)
{
  int i;
// измерения времени
  coord[0]=1;  // первая строка
  coord[1]=32; // последняя строка
  coord[2]=1;  // первый столбец
  coord[3]=1;  // последний столбец
  time_a[0]=0;
  for(i=1;i<32;i++) {
      time_a[i]=time_a[i-1]+leng_a[i];
  }
  i=sh_dwtb((char *)time_a, sizeof(time_a), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
  if(i<0) {
    sh_put_data_er(i);
    printf("Ошибка %x записи тока в таблицу %s\n",i,tb_names[0]);
    exit(0);
  }
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
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

//  тестовые измеренные данные инициализированы
//
   read_controls();
   memcpy(was_leng_a, leng_a, sizeof(leng_a));
   memcpy(was_rf_a, rf_a, sizeof(rf_a));
   was_forc_a=forc_a;


////  однократная инициализация измеряемых данных в тестовых целях

   
  writeTime();
   // измерения статуса
   coord[0]=1;  // первая строка
   coord[1]=1;  // последняя строка
   coord[2]=2;  // первый столбец
   coord[3]=2;  // последний столбец
   memcpy(stat_a,"overload",8);
   i=sh_dwtb(stat_a, sizeof(stat_a), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x записи статусов в таблицу %s\n",i,tb_names[0]);
      exit(0);
     }

   // измерения ошибки
   coord[0]=1;  // первая строка
   coord[1]=1;  // последняя строка
   coord[2]=3;  // первый столбец
   coord[3]=3;  // последний столбец
   memcpy(err_a,a_a+11,8);
   i=sh_dwtb(err_a, sizeof(err_a), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x записи состояний диагностики в таблицу %s\n",i,tb_names[0]);
      exit(0);
     }

   sh_put_meas_er(0);  // подтверждение обновления измерений в БД


//  открыты все таблицы, управляющие данные прочитаны
//  ЗДЕСЬ надо прописать аппаратуру последними данными, которые были сохранены в БД

   was_err=0;

   up_count=sh_get_count(); // циклический счётчик действий сверху
   Nc=0;
   sh_put_mess("Тестовая задача sh_test начинает работу");
   while(1)                 // большой постоянно действующий цикл !!!
    {
     was_err=0;
     time_wait(300);        // ~33 раза за 10 секунд
     j=sh_get_count();      // циклический счётчик действий сверху
     if(j!=up_count)        // есть новые данные сверху
      {
       up_count=j;
       read_controls();     // прочитал все управляющие данные
       for(j=0;j<32;j++)     // для 32-x значений
         {
          if(was_leng_a[j] != leng_a[j])
            {
             printf("%s элемент %d было %lg стало %lg\n", c_param[0], j+1, was_leng_a[j], leng_a[j]);
             was_leng_a[j]=leng_a[j];
             writeTime(); 
        
            }
          if(was_rf_a[j] != rf_a[j])
            {
             printf("%s элемент %d былa %lg сталa %lg\n", c_param[1], j+1, was_rf_a[j], rf_a[j]);
             was_rf_a[j]=rf_a[j];
            }
         }
       if(was_forc_a != forc_a)
         {
          printf("%s было %d стало %d\n", c_param[2], was_forc_a&255, forc_a&255);
          was_forc_a=forc_a;
         }
       if(!was_err)
        sh_put_data_er(0);  // подтверждение правильности введённых данных
      }

// здесь нужна проверка и сбор измеренных данных 


//     sh_put_meas_er(0);  // подтверждение обновления измерений в БД

    }
  }

