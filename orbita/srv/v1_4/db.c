#include "../../../ccyda/EqContStation/sh_ccydalib/sh_ccydalib.h"
#include "db.h"

int writeErrState_DB(int param, char data[], int size);
int writeCntUD_DB(int *cnt);
int writeAbsAxes_DB(char axisR[], char axisZ[], int sizeR, int sizeZ);
int writeOrbData_DB(int ind, double data[][timePoints]);
int writeOrbParamData_DB(int ind, int param, double data[], int size);
int writeOrbParamTH_DB(int ind,int param,char data[],int size);
int read_controls();
int db_init(void);
int db_process(unsigned int *update, int *realStart, int *realStop);
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

static struct itimerval newval;
static sigset_t maska;
static struct sigaction xa;

struct k_opis buf;
struct orbSetData cur_par;


// данные управляющие
static double tim_[16], cur_tim_[16];      // время в мс
static char   cntr[16], cur_cntr[16];      // порог и 8 статусов

// данные измеряемые
static char me_coun[1];     // циклический счётчик измерений
static char   axis[allAxis];// оси

static short i, coord[4], was_err,first_run; // was_err - код ошибки внутри большого цикла
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

int read_controls()  // чтение таблиц M_PD_ECS_ORB_CNTR и M_PD_ECS_ORB_CNTR_T
{
  int db_status=0;
//   8 parameters and threshold
  coord[0]=1;  // первая строка
  coord[1]=1;  // последняя строка
  coord[2]=1;  // первый столбец
  coord[3]=9;  // последний столбец
  i=sh_drtb((char *)cur_cntr ,sizeof(cur_cntr), coord, tab_ids[0]);  // M_PD_ECS_ORB_CNTR
  if(i<0)
  {
    sh_put_data_er(i);
    printf("1.Ошибка %x чтения таблицы M_PD_ECS_ORB_CNTR",i);
    db_status|=2;
  }
//   время в мс
   coord[0]=1;  // первая строка
   coord[1]=16; // последняя строка
   coord[2]=1;  // первый столбец
   coord[3]=1;  // последний столбец
  i=sh_drtb((char *)cur_tim_ ,sizeof(cur_tim_), coord, tab_ids[1]);  // M_PD_ECS_ORM_CNTR_T
  if(i<0)
  {
    sh_put_data_er(i);
    printf("2.Ошибка %x чтения таблицы M_PD_ECS_ORB_CNTR_T",i);
    db_status|=4;
  }
// read parameters from DB into structure
  if(!db_status){
    for(int k=0;k<16;k++) cur_par.ti[k]=cur_tim_[k];
    cur_par.tresholdI=((double *)cur_cntr)[0];
    cur_par.stStop=cur_cntr[8];
    cur_par.launch=cur_cntr[9];
    cur_par.dir=cur_cntr[10];
    cur_par.single=cur_cntr[11];
    cur_par.mode=cur_cntr[12];
    cur_par.math=cur_cntr[13];
    cur_par.work=cur_cntr[14];
    cur_par.graph=cur_cntr[15];
  }
  return(db_status);
}

/*
  write errors and status into DB
  param 6 параметров таблицы
  1 - Current Status/Error
  2 - Current Data/Time
  3 - Previous Status/Error
  4 - Previous Data/Time
  5 - Math Status/Error
  6 - Math Data/Time

  data char[16]
*/
int writeErrState_DB(int param, char data[],int size)
{
  short rep;
  if(!size) return(0);
  coord[0]=1;  // первая строка
  coord[1]=2;  // последняя строка
  coord[2]=param;    // первый столбец
  coord[3]=param;    // последний столбец

  rep=sh_dwtb(data, size, coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
  if(rep<0) {
    sh_put_data_er(rep);
    printf("Ошибка %x записи Err/StateP в таблицу M_PD_ECS_ORB_M_DS\n",i);
    return(1);
  }
  return(0);
}

// cycle counter update of measure
int writeCntUD_DB(int *cnt)
{
  int rep;
  coord[0]=1;  // первая строка
  coord[1]=1;  // последняя строка
  coord[2]=7;  // первый столбец
  coord[3]=7;  // последний столбец
  ++me_coun[0];
  rep=sh_dwtb(me_coun, sizeof(me_coun), coord, tab_ids[2]);  // M_PD_ECS_ORB_DS
  if(rep<0) {
    sh_put_data_er(i);
    printf("Ошибка %x записи Counter в таблицу M_PD_ECS_ORB_M_DS\n",i);
    *cnt=0;
    return(1);
  }
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  *cnt=me_coun[0];
  return(0);
}

// write absent axes into DB axes value 0-normal 0x81-absent
int writeAbsAxes_DB(char axisR[],char axisZ[],int sizeR,int sizeZ)
{
  int rep;
  coord[0]=1;  // первая строка
  coord[1]=60; // последняя строка
// Absent Axes R
  coord[2]=2;  // первый столбец
  coord[3]=2;  // последний столбец
  for(int k=0; k<allAxis; k++)
    if(axisR[k]&1)
      axis[k]=0x81;
    else
      axis[k]=0;
  rep=sh_dwtb(axisR , sizeR, coord, tab_ids[3]);  // M_PD_ECS_ORB_AXI
  if(rep<0) {
    sh_put_data_er(rep);
    printf("Ошибка %x записи Absent Axes R в таблицу M_PD_ECS_ORB_M_AXI\n",i);
    return(1);
  }
// Absent Axes Z
  coord[2]=3;  // первый столбец
  coord[3]=3;  // последний столбец
  for(int k=0; k<allAxis; k++)
    if(axisZ[k]&1)
      axis[k]=0x81;
    else
      axis[k]=0;
  rep=sh_dwtb(axisZ , sizeZ, coord, tab_ids[3]);  // M_PD_ECS_ORB_AXI
  if(rep<0) {
    sh_put_data_er(rep);
    printf("Ошибка %x записи Absent Axes Z в таблицу M_PD_ECS_ORB_M_AXI\n",i);
    return(1);
  }
  return(0);
}

// write position orbit data into DB ind - index of table
/*
        "M_PD_ECS_ORB_M_P_RC", 4
        "M_PD_ECS_ORB_M_P_RP", 5
        "M_PD_ECS_ORB_M_P_RM", 6
        "M_PD_ECS_ORB_M_P_ZC", 7
        "M_PD_ECS_ORB_M_P_ZP", 8
        "M_PD_ECS_ORB_M_P_ZM", 9
*/
int writeOrbData_DB(int ind,double data[][timePoints])
{
  int rep;
  double n_pos[allAxis];

  coord[0]=1;              // первая строка
  coord[1]=60;             // последняя строка

  for(int i=0;i<timePoints;i++) {    // для 16 измерений по 60 значений
    coord[2]=i+1;        // первый столбец
    coord[3]=i+1;        // последний столбец
    for (int j=0;j<allAxis;j++) n_pos[j]=data[j][i];
    rep=sh_dwtb((char *)n_pos, sizeof(n_pos), coord, tab_ids[ind]);
      if(rep<0) {
        sh_put_data_er(rep);
        printf("Ошибка %x записи  в таблицу %s\n",rep,tb_names[ind]);
        return(1);
      }
  }
  return(0);
}

/*  write parameters of measure orbit into DB
 *
 *  ind - index of table
        "M_PD_ECS_ORB_M_H_RC", 10
        "M_PD_ECS_ORB_M_H_RP", 11
        "M_PD_ECS_ORB_M_H_RM", 12
        "M_PD_ECS_ORB_M_H_ZC", 13
        "M_PD_ECS_ORB_M_H_ZP", 14
        "M_PD_ECS_ORB_M_H_ZM"  15
 param 11 параметров таблицы
 1 - RTime - real time of meas
 2 - Sin9, 3 -Cos9, 4 - Sin10, 5 - Cos10, 6 - Sin11, 7 - Cos11, 8 - Radius
 9 - Dispertion, 10 - RMS, 11 - threshold
 data[16] write data on measure time
*/
int writeOrbParamData_DB(int ind,int param,double data[],int size)
{
  int rep;
  coord[0]=1;           // первая строка
  coord[1]=16;          // последняя строка
  coord[2]=param;       // первый столбец
  coord[3]=param;       // последний столбец
  rep=sh_dwtb((char *)data, size, coord, tab_ids[ind]);
  if(rep<0) {
    sh_put_data_er(rep);
    printf("Ошибка %x записи  в таблицу %s\n",rep,tb_names[ind]);
    return(1);
  }
  return(0);
}

// write threshold
int writeOrbParamTH_DB(int ind,int param,char data[],int size)
{
  int rep;
  coord[0]=1;           // первая строка
  coord[1]=16;          // последняя строка
  coord[2]=param;       // первый столбец
  coord[3]=param;       // последний столбец
  rep=sh_dwtb((char *)data, size, coord, tab_ids[ind]);
  if(rep<0) {
    sh_put_data_er(rep);
    printf("Ошибка %x записи  в таблицу %s\n",rep,tb_names[ind]);
    return(1);
  }
  return(0);
}

//  однократная инициализация номеров осей
int staticInit(char init)
{
  if(!init) return(0);
  //  однократная инициализация номеров осей
  coord[0]=1;  // первая строка
  coord[1]=60; // последняя строка
  coord[2]=1;  // первый столбец
  coord[3]=1;  // последний столбец
  for(i=0; i<60; i++)
    axis[i]=2+2*i;
  i=sh_dwtb(axis , sizeof(axis), coord, tab_ids[3]);  // M_PD_ECS_ORB_AXI
  if(i<0) {
    sh_put_data_er(i);
    printf("Ошибка %x записи Axes в таблицу M_PD_ECS_ORB_M_AXI\n",i);
    return(1);
  }
  sh_put_meas_er(0);  // подтверждение обновления измерений в БД
  return(0);
}

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
  i=API_ECSini();
  if(i)  // инициализация связи со связной программой
  {
    printf("Ошибка при вызове API_ECSini = %d\n", i);
    i=API_ECSini();
    if(i)
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
      syslog(LOG_NOTICE,"Ошибка %x открытия таблицы %s\n",i,tb_names[j]);
      printf("Ошибка %x открытия таблицы %s\n",i,tb_names[j]);
      db_status|=1;
      continue;
    }
    tab_ids[j]=i;
  }

  db_status|=staticInit(1); // initialization of statical fealds of DB
  db_status|=read_controls() ;

  memcpy(tim_, cur_tim_, sizeof(cur_tim_));
  memcpy(cntr , cur_cntr, sizeof(cur_cntr));

  //  открыты все таблицы, управляющие данные прочитаны --------------------------------------------------
  //  ЗДЕСЬ надо прописать аппаратуру последними данными, которые были сохранены в БД

  was_err=0;
  Nc=0;
  first_run=1;

  up_count=sh_get_count(); // циклический счётчик действий сверху
  if(db_status==0) sh_put_data_er(0); //error not found
  sh_put_mess("Задача srvOrbita начинает работу");
  return(db_status);
}

// processing DB ~1/10 s
int db_process(unsigned int *update,int *realStart,int *realStop)
{
  int was_change=first_run;
  *update=0;

  if(first_run){
    *update=0xffffff; //update all parameters
  }
  first_run=0;
  was_err=0;
  *realStart=0; *realStop=0;
  j=sh_get_count();      // циклический счётчик действий сверху
  if(j!=up_count) {       // есть новые данные сверху
    up_count=j;
    int db_errcode;
    if((db_errcode=read_controls())) { // прочитал все управляющие данные в cur_***
      printf("Ошибка %x чтения таблицы M_PD_ECS_TIMIT\n",db_errcode);
      return(0);    // Ошибка БД - это криминал don't write data into HW
    }
    for(j=0;j<16;j++) {     // для 16 значений
      if(cur_tim_[j] != tim_[j]) {
        printf("Управление Time,ms: элемент %d было %lg стало %lg\n", j+1,tim_[j], cur_tim_[j]);
        tim_[j]=cur_tim_[j];
        cur_par.ti[j]=tim_[j];
        *update|=0x0001;
        was_change=1;
      }
    }
    if(((double *)cntr)[0] != ((double *)cur_cntr)[0]) {
      printf("Управление Threshold,10^11: было %lg стало %lg\n", ((double *)cntr)[0] , ((double *)cur_cntr)[0]);
      ((double *)cntr)[0] = ((double *)cur_cntr)[0];
      *update|=0x0002;
      cur_par.tresholdI=((double *)cur_cntr)[0];
      was_change=1;
    }
    if(cntr[9] != cur_cntr[9]) {
      printf("Управление System Start: было %d стало %d\n", cntr[9] , cur_cntr[9]);
      cntr[9] = cur_cntr[9];
      cur_par.launch=cur_cntr[9];
      *update|=0x0008;
      was_change=1;
    }
    if(cntr[10] != cur_cntr[10]) {
      printf("Управление Direction: было %d стало %d\n", cntr[10] , cur_cntr[10]);
      cntr[10] = cur_cntr[10];
      cur_par.dir=cur_cntr[10];
      *update|=0x0010;
      was_change=1;
    }
    if(cntr[11] != cur_cntr[11]) {
      printf("Управление Measure Type: было %d стало %d\n", cntr[11] , cur_cntr[11]);
      cntr[11] = cur_cntr[11];
      cur_par.single=cur_cntr[11];
      *update|=0x0020;
      was_change=1;
    }
    if(cntr[12] != cur_cntr[12]) {
      printf("Управление Mode: было %d стало %d\n", cntr[12] , cur_cntr[12]);
      cntr[12] = cur_cntr[12];
      cur_par.mode=cur_cntr[12];
      *update|=0x0040;
      was_change=1;
    }
    if(cntr[13] != cur_cntr[13]) {
      printf("Управление Math: было %d стало %d\n", cntr[13] , cur_cntr[13]);
      cntr[13] = cur_cntr[13];
      cur_par.math=cur_cntr[13];
      *update|=0x0080;
      was_change=1;
    }
    if(cntr[14] != cur_cntr[14]) {
      printf("Управление Work/Test: было %d стало %d\n", cntr[14] , cur_cntr[14]);
      cntr[14] = cur_cntr[14];
      cur_par.work=cur_cntr[14];
      *update|=0x0100;
      was_change=1;
    }
    if(cntr[15] != cur_cntr[15]) { // Это редактирование обязательно подтверждать !!!
      printf("Управление Графики: было %d стало %d\n", cntr[15] , cur_cntr[15]);
      cntr[15] = cur_cntr[15];
      *update|=0x0200;
      was_change=1;
    }
    if(cntr[8] != cur_cntr[8]) {
      printf("Управление Start/Stop: было %d стало %d\n", cntr[8] , cur_cntr[8]);
      cntr[8] = cur_cntr[8];
      cur_par.stStop=cur_cntr[8];
      if(cur_par.stStop==0){
        *realStart=1;
      }
      if(cur_par.stStop==1){
        *realStop=1;
      }
      *update|=0x0004;

    }
    if(!was_change){
      if(cur_par.stStop==0){
        *realStart=1;
        was_change=1;
        *update|=0x0004;
      }
      if(cur_par.stStop==1){
        *realStop=1;
        was_change=1;
        *update|=0x0004;
      }
    }
// здесь нужна проверка введённых данных
    if(!was_err)
      sh_put_data_er(0);  // подтверждение правильности введённых данных
  }
  return(was_change);
}
