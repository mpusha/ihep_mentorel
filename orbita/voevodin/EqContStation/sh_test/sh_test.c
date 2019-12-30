#include "/usr/usera/voevodin/EqContStation/sh_ccydalib/sh_ccydalib.h"

static struct itimerval newval;
static sigset_t maska;
static struct sigaction xa;

// данные измеряемые
static char  er_da[16];     // err/state или дата/время
static char me_coun[1];     // циклический счётчик измерений
static char   axis[60];     // оси
static double posi[60];     // положение
static double harm[16];     // время измерения, гармоники и др.
static char   int_thr[16];

// данные управляющие
static double tim_[16], cur_tim_[16];      // время в мс
static char   cntr[16], cur_cntr[16];      // порог и 8 статусов

//
static struct k_opis {
   char r_name[20];
   unsigned short n_tup;    // число строк
   unsigned short n_attr;   // число столбцов
   unsigned short tup_len;  // длина строки
   unsigned short last_act; // последнее действие 0 - чтение, 0xcccc - запись связной, 0xaaaa - запись управляющей
  } buf;
static short i, coord[4], was_err; // was_err - код ошибки внутри большого цикла
static char *tb_names[]={
	"M_PD_ECS_ORB_CNTR", 
 	"M_PD_ECS_ORB_CNTR_T",
 	"M_PD_ECS_ORB_M_DS",
 	"M_PD_ECS_ORB_M_AXI",
        "M_PD_ECS_ORB_M_P_RC",
 	"M_PD_ECS_ORB_M_P_RP",
 	"M_PD_ECS_ORB_M_P_RM",
 	"M_PD_ECS_ORB_M_P_ZC",
 	"M_PD_ECS_ORB_M_P_ZP",
 	"M_PD_ECS_ORB_M_P_ZM",
 	"M_PD_ECS_ORB_M_H_RC",
 	"M_PD_ECS_ORB_M_H_RP",
 	"M_PD_ECS_ORB_M_H_RM",
 	"M_PD_ECS_ORB_M_H_ZC",
 	"M_PD_ECS_ORB_M_H_ZP",
 	"M_PD_ECS_ORB_M_H_ZM"
      };
#define num_of_tabs sizeof(tb_names)/sizeof(tb_names[0])

static short tab_ids[num_of_tabs];
static unsigned int j, k, Nc, up_count;

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

void read_controls()  // чтение таблиц M_PD_ECS_ORB_CNTR и M_PD_ECS_ORB_CNTR_T
  {
//   время в секундах
   coord[0]=1;  // первая строка
   coord[1]=1;  // последняя строка
   coord[2]=1;  // первый столбец
   coord[3]=9;  // последний столбец
   i=sh_drtb((char *)cur_cntr ,sizeof(cur_cntr), coord, tab_ids[0]);  // M_PD_ECS_ORB_CNTR
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x чтения таблицы M_PD_ECS_ORB_CNTR\n",i);
      exit(0);
     }
//   стартовый импульс
   coord[0]=1;  // первая строка
   coord[1]=16; // последняя строка
   coord[2]=1;  // первый столбец
   coord[3]=1;  // последний столбец
   i=sh_drtb((char *)cur_tim_ ,sizeof(cur_tim_), coord, tab_ids[1]);  // M_PD_ECS_ORM_CNTR_T
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x чтения таблицы M_PD_ECS_ORB_CNTR_T\n",i);
      exit(0);
     }
  }
void set_16_pos(int ind)    //  M_PD_ECS_ORB_M_P_xx
  {
  short rep;
  int i,l;
  double n_pos[60];
   coord[0]=1;              // первая строка
   coord[1]=60;             // последняя строка
   for(i=0;i<16;i++)        // для 16 измерений по 60 значений
     {
      for(l=0;l<60;l++)
        n_pos[l]=posi[l]+3*ind+2*i;
      coord[2]=i+1;        // первый столбец
      coord[3]=i+1;        // последний столбец
      rep=sh_dwtb((char *)n_pos, sizeof(n_pos), coord, tab_ids[ind]);  
      if(rep<0)
        {
         sh_put_data_er(rep);
         printf("Ошибка %x записи  в таблицу %s\n",rep,tb_names[ind]);
         exit(0); 
        }
     }
  }
void set_16_har(int ind)    //  M_PD_ECS_ORB_M_H_xx
  {
  short rep;
  int i,l;
   coord[0]=1;           // первая строка
   coord[1]=16;          // последняя строка
   for(i=0;i<11;i++)     // 11 параметров таблицы
     {
      coord[2]=i+1;        // первый столбец
      coord[3]=i+1;        // последний столбец
      switch(i)
        {
         case 0:         // t,ms
         for(l=0;l<16;l++)
           harm[l] = 1000 + 200*ind + 50*l + i;
         rep=sh_dwtb((char *)harm, sizeof(harm), coord, tab_ids[ind]);  
         break;

         case 10:        // Int threshold
         for(l=0;l<16;l++)
           if( (ind+l+i)%4)
             int_thr[l] = 0x81;
           else
             int_thr[l] = 0;
         rep=sh_dwtb((char *)int_thr, sizeof(int_thr), coord, tab_ids[ind]);  
         break;

         case 8:         // Dispersion
         case 9:         // RMS
         for(l=0;l<16;l++)
           harm[l] = 100 - 3*ind -2*l -i;
         rep=sh_dwtb((char *)harm, sizeof(harm), coord, tab_ids[ind]);  
         break;

         default:        // Sin, Cos, Radius
         for(l=0;l<16;l++)
           harm[l] = 100 - 6*ind -5*l -i;
         rep=sh_dwtb((char *)harm, sizeof(harm), coord, tab_ids[ind]);  
        }
      if(rep<0)
        {
         sh_put_data_er(rep);
         printf("Ошибка %x записи  в таблицу %s\n",rep,tb_names[ind]);
         exit(0); 
        }
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

//  однократная инициализация номеров осей
   coord[0]=1;  // первая строка
   coord[1]=60; // последняя строка
   coord[2]=1;  // первый столбец
   coord[3]=1;  // последний столбец
   for(i=0; i<60; i++)
     axis[i]=2+2*i;
   i=sh_dwtb(axis , sizeof(axis), coord, tab_ids[3]);  // M_PD_ECS_ORB_AXI
   if(i<0)
     {
      sh_put_data_er(i);
      printf("Ошибка %x записи Axes в таблицу M_PD_ECS_ORB_M_AXI\n",i);
      exit(0); 
     }
   sh_put_meas_er(0);  // подтверждение обновления измерений в БД

//
   read_controls();
   memcpy(tim_, cur_tim_, sizeof(cur_tim_));
   memcpy(cntr , cur_cntr, sizeof(cur_cntr));

//  открыты все таблицы, управляющие данные прочитаны
//  ЗДЕСЬ надо прописать аппаратуру последними данными, которые были сохранены в БД

   up_count=sh_get_count(); // циклический счётчик действий сверху
   Nc=0;
   sh_put_mess("Тестовая задача sh_test начинает работу");

   while(1)                 // большой постоянно действующий цикл !!!
    {
     ++Nc;
     if(Nc==40)             // обновление измерений и счётчика ~ один раз в 10 секунд
       {
        Nc=0;
        coord[0]=1;  // первая строка
        coord[1]=60; // последняя строка
// Absent Axes R
        coord[2]=2;  // первый столбец
        coord[3]=2;  // последний столбец
        for(i=0; i<60; i++)
          if(i&1)
             axis[i]=0x81;
          else
             axis[i]=0;
        i=sh_dwtb(axis , sizeof(axis), coord, tab_ids[3]);  // M_PD_ECS_ORB_AXI
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Absent Axes R в таблицу M_PD_ECS_ORB_M_AXI\n",i);
           exit(0); 
          }
// Absent Axes Z
        coord[2]=3;  // первый столбец
        coord[3]=3;  // последний столбец
        for(i=0; i<60; i++)
          if(i&1)
             axis[i]=0;
          else
             axis[i]=0x81;
        i=sh_dwtb(axis , sizeof(axis), coord, tab_ids[3]);  // M_PD_ECS_ORB_AXI
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Absent Axes Z в таблицу M_PD_ECS_ORB_M_AXI\n",i);
           exit(0); 
          }
// Err/StateC
        coord[0]=1;  // первая строка
        coord[1]=2;  // последняя строка
        coord[2]=1;  // первый столбец
        coord[3]=1;  // последний столбец
        memcpy(er_da, "CurErrorCurState...",16);
        i=sh_dwtb(er_da, sizeof(er_da), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Err/StateC в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }
// Date/TimeC
        coord[0]=1;  // первая строка
        coord[1]=2;  // последняя строка
        coord[2]=2;  // первый столбец
        coord[3]=2;  // последний столбец
        memcpy(er_da, "CurrDateCurrTime...",16);
        i=sh_dwtb(er_da, sizeof(er_da), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Date/TimeC в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }
// Err/StateP
        coord[0]=1;  // первая строка
        coord[1]=2;  // последняя строка
        coord[2]=3;  // первый столбец
        coord[3]=3;  // последний столбец
        memcpy(er_da, "PreErrorPreState...",16);
        i=sh_dwtb(er_da, sizeof(er_da), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Err/StateP в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }
// Date/TimeP
        coord[0]=1;  // первая строка
        coord[1]=2;  // последняя строка
        coord[2]=4;  // первый столбец
        coord[3]=4;  // последний столбец
        memcpy(er_da, "PrevDatePrevTime...",16);
        i=sh_dwtb(er_da, sizeof(er_da), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Date/TimeP в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }
// Err/StateM
        coord[0]=1;  // первая строка
        coord[1]=2;  // последняя строка
        coord[2]=5;  // первый столбец
        coord[3]=5;  // последний столбец
        memcpy(er_da, "MatErrorMatState...",16);
        i=sh_dwtb(er_da, sizeof(er_da), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Err/StateM в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }
// Date/TimeM
        coord[0]=1;  // первая строка
        coord[1]=2;  // последняя строка
        coord[2]=6;  // первый столбец
        coord[3]=6;  // последний столбец
        memcpy(er_da, "MathDateMathTime...",16);
        i=sh_dwtb(er_da, sizeof(er_da), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ошибка %x записи Date/TimeM в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }
// Positions 
        for(k=4; k<10; k++)     // идентификаторы 6 таблиц M_PD_ECS_ORB_M_P_xx
          {
           for(i=0; i<60; i++)  // исходные значения положения
             {
              posi[i]= -110+2*i;
             }
           set_16_pos(k);
          }
// Harmonics и др.
        for(k=10; k<16; k++)     // идентификаторы 6 таблиц M_PD_ECS_ORB_M_H_xx
          {
           set_16_har(k);
          }
//  циклический счётчик обновления измерений
        coord[0]=1;  // первая строка
        coord[1]=1;  // последняя строка
        coord[2]=7;  // первый столбец
        coord[3]=7;  // последний столбец
        ++me_coun[0];
        i=sh_dwtb(me_coun, sizeof(me_coun), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
        if(i<0)
          {
           sh_put_data_er(i);
           printf("Ока %x записи Counter в таблицу M_PD_ECS_ORB_M_DS\n",i);
           exit(0); 
          }

        sh_put_meas_er(0);  // подтверждение обновления измерений в БД
       }

     was_err=0;
     time_wait(250);        // ~4 раза в секунду
     j=sh_get_count();      // циклический счётчик действий сверху
     if(j!=up_count)        // есть новые данные сверху
      {
       up_count=j;
       read_controls();     // прочитал все управляющие данные в cur_***
       if(((double *)cntr)[0] != ((double *)cur_cntr)[0])
         {
          printf("Управление Threshold,10^11: было %lg стало %lg\n", ((double *)cntr)[0] , ((double *)cur_cntr)[0]);
          ((double *)cntr)[0] = ((double *)cur_cntr)[0];
         }
       if(cntr[8] != cur_cntr[8])
         {
          printf("Управление Start/Stop: было %d стало %d\n", cntr[8] , cur_cntr[8]);
          cntr[8] = cur_cntr[8];
         }
       if(cntr[9] != cur_cntr[9])
         {
          printf("Управление System Start: было %d стало %d\n", cntr[9] , cur_cntr[9]);
          cntr[9] = cur_cntr[9];
         }
       if(cntr[10] != cur_cntr[10])
         {
          printf("Управление Direction: было %d стало %d\n", cntr[10] , cur_cntr[10]);
          cntr[10] = cur_cntr[10];
         }
       if(cntr[11] != cur_cntr[11])
         {
          printf("Управление Measure Type: было %d стало %d\n", cntr[11] , cur_cntr[11]);
          cntr[11] = cur_cntr[11];
         }
       if(cntr[12] != cur_cntr[12])
         {
          printf("Управление Mode: было %d стало %d\n", cntr[12] , cur_cntr[12]);
          cntr[12] = cur_cntr[12];
         }
       if(cntr[13] != cur_cntr[13])
         {
          printf("Управление Math: было %d стало %d\n", cntr[13] , cur_cntr[13]);
          cntr[13] = cur_cntr[13];
         }
       if(cntr[14] != cur_cntr[14])
         {
          printf("Управление Work/Test: было %d стало %d\n", cntr[14] , cur_cntr[14]);
          cntr[14] = cur_cntr[14];
         }
       if(cntr[15] != cur_cntr[15])  // Это редактирование обязательно подтверждать !!!
         {
          printf("Управление Графики: было %d стало %d\n", cntr[15] , cur_cntr[15]);
          cntr[15] = cur_cntr[15];
         }

       for(j=0;j<16;j++)     // для 16 значений
         {
          if(cur_tim_[j] != tim_[j])
            {
             printf("Управление Time,ms: элемент %d было %lg стало %lg\n", j+1,tim_[j], cur_tim_[j]);
             tim_[j]=cur_tim_[j];
            }
         }

// здусь нужна проверка введённых данных 

       if(!was_err)
        sh_put_data_er(0);  // подтверждение правильности введённых данных
      }

    }

  }

