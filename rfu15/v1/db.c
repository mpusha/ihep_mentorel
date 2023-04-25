#include "../../../ccyda/EqContStation/sh_ccydalib/sh_ccydalib.h"
#include "db.h"

int writeErr_DB(char data[]);
int writeStatus_DB(char data[]);
int writeTime_DB(void);
int writeRF_DB(void);
int read_controls(void);
int db_init(void);
int db_process(unsigned int *update);

double userMult;
static char *tb_names[]={
        "B_PD_ECS_A_RF_MEAS",   // 0
        "B_PD_ECS_A_RF_CNTR",   // 1
      };
#define num_of_tabs sizeof(tb_names)/sizeof(tb_names[0])

static struct itimerval newval;
static sigset_t maska;
static struct sigaction xa;

struct k_opis buf;
struct rfAmpSetData cur_par;


// данные управляющие
static double leng_a[ALLVECTORS], was_leng_a[ALLVECTORS];    ///< длительность
static double rf_a[ALLVECTORS], was_rf_a[ALLVECTORS];        ///< амплитуда
static char   forc_a, was_forc_a;            ///< форсирование

// данные измеряемые
static double time_a[ALLVECTORS];    ///< время измеренное
static double rfm_a[ALLVECTORS];    ///< RF измеренное
static char   stat_a[8];     ///< состояние
static char   err_a[8];      ///< ошибки

static short i, coord[4], was_err,first_run; // was_err - код ошибки внутри большого цикла
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
/*!
 * \brief read_controls
 * \return 0 - Ok, else error open DB
 */
int read_controls(void)  // чтение таблиц M_PD_ECS_ORB_CNTR и M_PD_ECS_ORB_CNTR_T
{
  int db_status=0;
//   длительность
  coord[0]=1;  // первая строка
  coord[1]=32; // последняя строка
  coord[2]=1;  // первый столбец
  coord[3]=1;  // последний столбец
  i=sh_drtb((char *)leng_a ,sizeof(leng_a), coord, tab_ids[1]);  // B_PD_ECS_A_RF_CNTR
  if(i<0) {
    sh_put_data_er(i);
    printf("1.Ошибка %x чтения таблицы %s\n",i,tb_names[1]);
    db_status|=2;
  }
//   амплитуда
  coord[0]=1;  // первая строка
  coord[1]=32; // последняя строка
  coord[2]=2;  // первый столбец
  coord[3]=2;  // последний столбец
  i=sh_drtb((char *) rf_a,sizeof(rf_a), coord, tab_ids[1]);  // B_PD_ECS_A_RF_CNTR
  if(i<0) {
    sh_put_data_er(i);
    printf("2.Ошибка %x чтения таблицы %s\n",i,tb_names[1]);
    db_status|=4;
  }
//   форсирование
  coord[0]=1;  // первая строка
  coord[1]=1;  // последняя строка
  coord[2]=3;  // первый столбец
  coord[3]=3;  // последний столбец
  i=sh_drtb(&forc_a ,sizeof(forc_a), coord, tab_ids[1]);  // B_PD_ECS_A_RF_CNTR
  if(i<0) {
    sh_put_data_er(i);
    printf("1.Ошибка %x чтения таблицы %s\n",i,tb_names[1]);
    db_status|=8;
  }
// read parameters from DB into structure
  if(!db_status) {
    for(int k=0;k<ALLVECTORS;k++) {
      cur_par.length[k]=leng_a[k];
      cur_par.rfAmp[k]=rf_a[k];
    }
    cur_par.time[0]=0;
    for(int i=1;i<ALLVECTORS;i++) {
      cur_par.time[i]=cur_par.time[i-1]+cur_par.length[i];
    }
    cur_par.forcing=forc_a;
    cur_par.overload=0;
    double mult=1;
    if(cur_par.forcing==1) mult=1.13;
    else if(cur_par.forcing==2) mult=1.25;
    else if(cur_par.forcing==3) mult=0.7;
    else if(cur_par.forcing==4) mult=userMult;
    for(int i=0;i<ALLVECTORS;i++){
      double tmp=cur_par.rfAmp[i]*mult;
      if(tmp>MAXAMP) { tmp=MAXAMP; cur_par.overload=1; }
      cur_par.realAmp[i]=tmp;
    }
  }
  return(db_status);
}

/*!
 * \brief writeStatus_DB write status into DB
 * \param[i] char* data
 * \return 0 if Ok
 */
int writeStatus_DB(char data[])
{
  short ret;
  char ch[8]="        ";
  for(int i=0;i<8;i++){
    if(data[i]) ch[i]=data[i]; else break;
  }
  coord[0]=1;  // первая строка
  coord[1]=1;  // последняя строка
  coord[2]=2;  // первый столбец
  coord[3]=2;  // последний столбец
  ret=sh_dwtb(ch, sizeof(ch), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
  if(ret<0) {
    sh_put_data_er(ret);
    printf("Ошибка %x записи статусов в таблицу %s\n",ret,tb_names[0]);
    return(1);
  }
  return(0);
}

/*!
 * \brief writeErr_DB  write error into DB
 * \param[in] char* data
 * \return 0 if Ok
 */
int writeErr_DB(char data[])
{
  short ret;
  char ch[8]="        ";
  for(int i=0;i<8;i++) {
    if(data[i]) ch[i]=data[i]; else break;
  }
  coord[0]=1;  // первая строка
  coord[1]=1;  // последняя строка
  coord[2]=3;  // первый столбец
  coord[3]=3;  // последний столбец
  ret=sh_dwtb(ch, sizeof(ch), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
  if(ret<0) {
    sh_put_data_er(ret);
    printf("Ошибка %x записи состояния ошибок в таблицу %s\n",ret,tb_names[0]);
    return(1);
  }
  return(0);
}

/*!
 * \brief writeTime_DB вычисляет время начала векторов и записывает его в БД
 * \return 0 if Ok
 */
int writeTime_DB(void)
{
  int ret;
// измерения времени
  coord[0]=1;  // первая строка
  coord[1]=32; // последняя строка
  coord[2]=1;  // первый столбец
  coord[3]=1;  // последний столбец

  for(int i=0;i<ALLVECTORS;i++) {
      time_a[i]=cur_par.time[i];
  }
  ret=sh_dwtb((char *)time_a, sizeof(time_a), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
  if(ret<0) {
    sh_put_data_er(i);
    printf("Ошибка %x записи времени в таблицу %s\n",i,tb_names[0]);
    return(1);
  }
  //sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return 0;
}

/*!
 * \brief writeRF_DB
 * \return 0 if Ok
 */
int writeRF_DB(void)
{
  int ret;
// измерения ВЧ
  coord[0]=1;  // первая строка
  coord[1]=32; // последняя строка
  coord[2]=4;  // первый столбец
  coord[3]=4;  // последний столбец

  for(int i=0;i<ALLVECTORS;i++) {
      rfm_a[i]=cur_par.realAmp[i];
  }
  ret=sh_dwtb((char *)rfm_a, sizeof(rfm_a), coord, tab_ids[0]);  // B_PD_ECS_A_RF_MEAS
  if(ret<0) {
    sh_put_data_er(i);
    printf("Ошибка %x записи установленной амплитуды в таблицу %s\n",i,tb_names[0]);
    return(1);
  }
  //sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return 0;
}

/*!
 * \brief db_init
 * \return 0 if Ok
 */
int db_init(void)
{
  int db_status=0;
  newval.it_interval.tv_sec=0;
  newval.it_interval.tv_usec=0;
  newval.it_value.tv_sec=0;
  newval.it_value.tv_usec=0;
  if(setitimer(ITIMER_REAL,&newval,0))
  {
    syslog(LOG_NOTICE,":test:setitimer:%s\n",strerror(errno));
    perror("setitimer");
    return(1);
  }
  if(sigemptyset(&maska))          // SIGALRM используется с таймерами
  {
    syslog(LOG_NOTICE,":test:sigemptyset:%s\n",strerror(errno));
    perror("sigemptyset");
    return(1);
  }
  if(sigaddset(&maska,SIGALRM))
  {
    syslog(LOG_NOTICE,":test:sigaddset:%s\n",strerror(errno));
    perror("sigaddset");
    return(1);
  }
  xa.sa_handler=handler;
  xa.sa_flags=SA_RESTART;
  if(sigaction(SIGALRM,&xa,NULL))  // set handler
  {
    syslog(LOG_NOTICE,":test:sigaction:%s\n",strerror(errno));
    perror("sigaction");
    return(1);
  }
  i=API_ECSini();
  if(i)  // инициализация связи со связной программой
  {
    printf("Ошибка при вызове API_ECSini = %d\n", i);
    i=API_ECSini();
    if(i)
    {
      syslog(LOG_NOTICE,":test:Вторая ошибка при вызове API_ECSini = %d\n", i);
      printf("Вторая ошибка при вызове API_ECSini = %d\n", i);
      return(1);
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
      syslog(LOG_NOTICE,"Ошибка %x открытия таблицы %s\n",i,tb_names[j]);
      printf("Ошибка %x открытия таблицы %s\n",i,tb_names[j]);
      return(1);
    }
    tab_ids[j]=i;
  }

  db_status=read_controls() ;
  if(db_status) {
    sh_put_mess("Ошибка базы данных.");
    writeErr_DB((char*)"DB error");
    writeStatus_DB((char*)"Locale");
    sh_put_meas_er(0);  // подтверждение обновления измерений в БД
    return db_status;
  }

  memcpy(was_leng_a, leng_a, sizeof(leng_a));
  memcpy(was_rf_a, rf_a, sizeof(rf_a));
  was_forc_a=forc_a;
  writeErr_DB((char*)"Unknown ");
  writeTime_DB();
  if(cur_par.overload){
    writeStatus_DB("Overload");
    printf("RF overload.\n");
  }
  else {
    writeStatus_DB("Normal");
  }
  writeRF_DB();
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД

  //  открыты все таблицы, управляющие данные прочитаны --------------------------------------------------
  //  ЗДЕСЬ надо прописать аппаратуру последними данными, которые были сохранены в БД

  was_err=0;
  Nc=0;
  first_run=1;

  up_count=sh_get_count(); // циклический счётчик действий сверху
  sh_put_data_er(0); //error not found
  sh_put_mess("Задача srvRfU15 начинает работу");
  return(0);
}

/*!
 * \brief db_process
 * processing DB ~1/10 s
 * \param[out] update - битовая карта измененных параметров
 * 0 - длительность, 1 - ВЧ амплитуда, 2 - форсировка.
 *
 * \return 0 - не было изменений данных, 1 - данные изменялись нужно записать в аппаратуру
 */
int db_process(unsigned int *update)
{
  int was_change=first_run;
  *update=0;

  if(first_run){
    *update=0xffffff; //update all parameters
  }
  first_run=0;

  was_err=0;
  j=sh_get_count();      // циклический счётчик действий сверху
  if(j!=up_count) {       // есть новые данные сверху
    printf("New data up_count=%d count=%d\n",up_count,j);
    up_count=j;
    int db_errcode;
    if((db_errcode=read_controls())) { // прочитал все управляющие данные в cur_***
      printf("Ошибка %x чтения таблицы M_PD_ECS_TIMIT\n",db_errcode);
      return(0);    // Ошибка БД - это криминал don't write data into HW
    }
    for(j=0;j<ALLVECTORS;j++) {     // для 32 значений
      if(was_leng_a[j] != leng_a[j]) {
        printf("%s элемент %d было %lg стало %lg\n", c_param[0], j+1, was_leng_a[j], leng_a[j]);
        was_leng_a[j]=leng_a[j];
        *update|=0x0001;
        was_change=1;
      }
      if(was_rf_a[j] != rf_a[j]) {
        printf("%s элемент %d былa %lg сталa %lg\n", c_param[1], j+1, was_rf_a[j], rf_a[j]);
        was_rf_a[j]=rf_a[j];
        cur_par.rfAmp[j]=rf_a[j];
        *update|=0x0002;
        was_change=1;
      }
    }
    if(was_forc_a != forc_a) {
      printf("%s было %d стало %d\n", c_param[2], was_forc_a&255, forc_a&255);
      was_forc_a=forc_a;
      *update|=0x0004;
      was_change=1;
    }
// здесь нужна проверка введённых данных
    if(was_change) {
      if(*update&0x1) {
        if(cur_par.time[ALLVECTORS-1]>MAXTIME) {
          was_err=1;
          writeErr_DB("Err data");
          writeStatus_DB("Time >50");
          printf("Input time set up error.\n");
          was_change=0; // can't write hw
          *update=0;
        }
        writeTime_DB();
      }
      if(*update&0x6){
        if(cur_par.overload){
          writeStatus_DB("Overload");
          printf("RF overload.\n");
        }
        else {
          writeStatus_DB("Normal  ");
        }
        writeRF_DB();
      }
      sh_put_meas_er(0);  // подтверждение обновления измерений в БД
    }
    if(!was_err)
      sh_put_data_er(0);  // подтверждение правильности введённых данных
  }
  return(was_change);
}
