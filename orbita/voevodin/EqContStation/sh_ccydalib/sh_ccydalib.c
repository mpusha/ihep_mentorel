#include "sh_ccydalib.h"
#include "/usr/usera/voevodin/rt-data/ccydalib/ccydalib.h"
#define tab_id_base 100

static int fpt1;                    // дескриптор псевдофайла
static sigset_t mask;
static struct sigaction Xx;
static char *sh_ref;                // указатель на разделяемую память
static struct sh_start *to_start;   // указатель на заголовок разделяемой памяти
static struct r_opis *tab_first;    // указатель на первую таблицу
static unsigned short tab_wr_code;  // код последней записи в таблицу -> 0xcccc - запись связной, 0xaaaa - запись управляющей
static unsigned short mes_tab_id;   // tab_id для U70_ECS_MESSAGES
static struct ECS_mess Mes_line;
///------------------- Время и дата на русском
static  char *days[]={
     "Вск",
     "Пнд",
     "Втр",
     "Срд",
     "Чтв",
     "Птн",
     "Сбт"
    };
static char DateTime[32];
static  char *months[]={
     "Янв",
     "Фев",
     "Мар",
     "Апр",
     "Май",
     "Июн",
     "Июл",
     "Авг",
     "Сен",
     "Окт",
     "Ноя",
     "Дек"
    };
char *our_time()
  {
  time_t timer;
  struct tm  *timstr;
   memset(DateTime, 0, sizeof(DateTime));
   time(&timer);
   timstr=localtime(&timer);
   sprintf(DateTime,"%02d:%02d:%02d %s %2d %s %4d",
   timstr->tm_hour, timstr->tm_min, timstr->tm_sec, 
    days[timstr->tm_wday], timstr->tm_mday, months[timstr->tm_mon],
     1900+timstr->tm_year);
   return(DateTime);
  }

static void handler(int a)
  {
   switch (a)
     {
      case SIGUSR1:
      break;
     }
  }

void tabs_to_mem(size_t *m_size) // Вызывает только CommTask !!!
  {
  FILE *fpoi;
  char buf1[52], *ref_wrk;
  short crd[7],tb_id,i;
  struct relis {
        short unsigned ntup;
        short unsigned nat;
        short unsigned lotup;
        short unsigned third;
        char rtype;
        char rstate;
       } opis;
  size_t sum_size=sizeof(struct sh_start); // заголовок памяти
  struct r_opis *cur_opis, *prev_opis;
  int j;
  struct at_opis *cur_attr;

   if(!(fpoi=fopen(name2,"r")))             // файл tabs_to_shmem.ecs
     {
      syslog(LOG_NOTICE,":fopen:%s\n",strerror(errno));
      perror("fopen:");
      exit(0);
     }
   cur_opis=tab_first;  // на первую таблицу при разметке памяти
   prev_opis=NULL;
   fgets(buf1,sizeof(buf1),fpoi);           // установка/подсистема
   for(i=0;i<sizeof(buf1);i++)
     if(buf1[i]=='\n')
       {
        buf1[i]=0;
        break;
       }
   memset(&Mes_line,0,sizeof(Mes_line));
   if(*m_size)  // память уже есть, установить Mess_tuple и Mess_cur_pl для таблицы U70_ECS_MESSAGES
     {
      if((mes_tab_id=opnglb(name3,"",&i))<0)
        {
         syslog(LOG_NOTICE,":err.open '%s' = %x\n",name3,mes_tab_id);
         printf("err.open '%s' = %x\n",name3,mes_tab_id);
         goto Dalee_0;
        }
      if((i=rlpdbl(&opis,mes_tab_id))<0)         // заполнил структуру relis
        {
         syslog(LOG_NOTICE,":err.rlpdbl '%s' = %x\n",name3,i);
         printf("err.rlpdbl '%s' = %x\n",name3,i);
         goto Dalee_0;
        }
      crd[0]=crd[1]=crd[2]=crd[3]=crd[4]=crd[5]=crd[6]=1;
      for(j=0;j<opis.ntup;j++,crd[2]++,crd[3]++)
        {
         if((i=dtrdbl(Mes_line.ECS_name, sizeof(Mes_line.ECS_name), crd,mes_tab_id))<0)  // читаю первый столбец
           {
            syslog(LOG_NOTICE,":err.dtrdbl '%s' = %x\n",name3, i);
            printf("err.dtrdbl '%s' = %x\n",name3, i);
            goto Dalee_0;
           }
         if(!strcmp(Mes_line.ECS_name,buf1))  // СКО уже есть в U70_ECS_MESSAGES
           {
            to_start->Mess_tuple=crd[2];
            to_start->Mess_cur_pl=1;          // после перезагрузки CommTask всегда с плоскости 1
            goto Dalee_0;                     // O.K.
           }
        }
      crd[0]=crd[1]=crd[2]=crd[3]=crd[4]=crd[5]=crd[6]=1;
      for(j=0;j<opis.ntup;j++,crd[2]++,crd[3]++)
        {
         if((i=dtrdbl(Mes_line.ECS_name, sizeof(Mes_line.ECS_name), crd,mes_tab_id))<0)  // читаю первый столбец
           {
            syslog(LOG_NOTICE,":err.dtrdbl '%s' = %x\n",name3, i);
            printf("err.dtrdbl '%s' = %x\n",name3, i);
            goto Dalee_0;
           }
         if(!Mes_line.ECS_name[0])            // свободная строка
           {
            strcpy(Mes_line.ECS_name,buf1);
            if((i=dtwdbl(Mes_line.ECS_name, sizeof(Mes_line.ECS_name), crd,mes_tab_id))<0)  // пишу первый столбец
             {
              syslog(LOG_NOTICE,":err.dwrdbl '%s' = %x\n",name3, i);
              printf("err.dtwdbl '%s' = %x\n",name3, i);
              goto Dalee_0;
             }
            to_start->Mess_tuple=crd[2];
            to_start->Mess_cur_pl=1;
            goto Dalee_0;
           }
        }
     }
Dalee_0:                                    // Mes_line.ECS_name либо установлено, либо нет (в случае ошибки)
   while(fgets(buf1,sizeof(buf1),fpoi))     // buf1 = имя таблицы
     {
      for(i=0;i<sizeof(buf1);i++)
        if(buf1[i]=='\n')
          {
           buf1[i]=0;
           break;
          }
      if(buf1[0]==0)
        break;
      if((tb_id=opnglb(buf1,"",&i))<0)
        {
         syslog(LOG_NOTICE,":err.open '%s' = %x\n",buf1,tb_id);
         printf("err.open '%s' = %x\n",buf1,tb_id);
         exit(0);
        }
      if((i=rlpdbl(&opis,tb_id))<0)         // заполнил структуру relis
        {
         syslog(LOG_NOTICE,":err.rlpdbl '%s' = %x\n",buf1,i);
         printf("err.rlpdbl '%s' = %x\n",buf1,i);
         exit(0);
        }
      if(! *m_size)  // определить размер разделяемой памяти (место для описания всех столбцов)
        {
         sum_size += opis.ntup*opis.lotup + opis.nat*sizeof(struct at_opis) + sizeof(struct r_opis); // только одна плоскость в памяти !!!
        }
      else           // разметить разделяемую память
        {
         strncpy(cur_opis->r_name, buf1, 20);  // имя таблицы
         cur_opis->next=0;
         cur_opis->n_tup=opis.ntup;            // число строк
         cur_opis->tup_len=opis.lotup;         // длина строки
         cur_opis->n_attr=opis.nat;            // число столбцов
         cur_opis->last_act=0;                 // якобы было чтение
         ref_wrk= (char *)cur_opis+sizeof(struct r_opis);
         cur_attr=(struct at_opis *)ref_wrk;   // на описание первого столбца
         if(opis.rtype&1)         // матрица
           {
            if((i=atrdbl(cur_attr, 4, 1,tb_id))<0)         // заполнил структуру at_opis
              {
               syslog(LOG_NOTICE,":err.atrdbl '%s' столбец 1 = %x\n",buf1,i);
               printf("err.atrdbl '%s' столбец 1 = %x\n",buf1,i);
               exit(0);
              }
            cur_opis->flag=cur_attr->at_len;            // длина элемента матрицы
            ++cur_attr;
           }
         else                     // таблица (не матрица)
           {
            for(j=0;j<opis.nat; j++)                    // чтение описаний столбцов
              {
               if((i=atrdbl(cur_attr, 4, j+1,tb_id))<0)   // заполнил структуру at_opis
                 {
                  syslog(LOG_NOTICE,":err.atrdbl '%s' столбец %d = %x\n",buf1, j, i);
                  printf("err.atrdbl '%s' столбец %d = %x\n",buf1, j, i);
                  exit(0);
                 }
               ++cur_attr;
               cur_opis->flag=0;
              }
           }
         ref_wrk= (char *)cur_attr;        // начало данных
         cur_opis->tab_data = ref_wrk - (char *)cur_opis; // смещение данных от начала описания таблицы
         crd[0]=crd[1]=crd[2]=crd[4]=crd[6]=1;
         crd[3]=cur_opis->n_tup;
         crd[5]=cur_opis->n_attr;
         if((i=dtrdbl(ref_wrk, cur_opis->tup_len*cur_opis->n_tup, crd,tb_id))<0)  // читаю первую плоскость целиком
           {
            syslog(LOG_NOTICE,":err.dtrdbl '%s' = %x\n",buf1, i);
            printf("err.dtrdbl '%s' = %x\n",buf1, i);
            exit(0);
           }
         if(prev_opis)
           {
            prev_opis->next= (char *)cur_opis-sh_ref;
           }
         prev_opis=cur_opis;
         cur_opis=(struct r_opis *)(ref_wrk+cur_opis->tup_len*cur_opis->n_tup);
        }
      rcldbl(tb_id);  // закрываю таблицу
     }
   if(! *m_size)     // определил размер разделяемой памяти
     {
     *m_size=sum_size;
     }
   else
     {
      to_start->comm_pid=getpid();    // PID связной программы
      to_start->sh_flag=1;            // разделяемая память назначена и размечена
      if(msync(sh_ref, sum_size, MS_SYNC | MS_INVALIDATE))
       if(errno != EINVAL)
        {
         syslog(LOG_NOTICE,":tabs_to_mem:msync:%s\n",strerror(errno));
         perror("tabs_to_mem:msync: ");
        }
     }
  }

short sh_ECSini(short flag)   // инициализация разделяемой памяти управляющей и связной программами
  {
  int c=0;
  size_t sh_length=0;
  struct stat fd;
  short crd[7],i;
   if(flag)                   // имя EquipmentControlStationMemory
     {
      fpt1=shm_open(name1,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
     }
    else
cyc:
      fpt1=shm_open(name1,O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(fpt1 == -1)
      {
       if( !flag && c != 10)  //  управляющая программа ждёт 10 с
         {
          sleep(1);
          ++c;
          goto cyc;
         }
       if( !flag)
         return 2;            //  управляющая программа - память не создана
       syslog(LOG_NOTICE,":shm_open:%s\n",strerror(errno));
       perror("shm_open:");
       exit(0);
      }
   if(flag)  // коммуникационная программа CommTask
     {
      tab_wr_code=0xcccc;
//  определить размер требуемой памяти
      tabs_to_mem(&sh_length);              // sh_length = 0
//  получить адрес памяти и разметить её
      if(ftruncate(fpt1,sh_length))
        {
         syslog(LOG_NOTICE,"ftruncate:%s\n",strerror(errno));
         perror("ftruncate:");
         exit(0);
        }
      sh_ref=mmap(NULL, sh_length, PROT_READ | PROT_WRITE, MAP_SHARED,fpt1, 0);
      if(sh_ref == MAP_FAILED)
        {
         syslog(LOG_NOTICE,"Связная:mmap:%s\n",strerror(errno));
         perror("Связная:mmap:");
         exit(0);
        }
      if(fchmod(fpt1, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))
        perror("fchmod:");
      close(fpt1);
//      if(chmod("/dev/shm/EquipmentControlStationMemory", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))
//        perror("chmod:");
      memset(sh_ref, 0, sh_length);
      to_start = (struct sh_start *)sh_ref;
      to_start->shmem_size=sh_length;
      tab_first = (struct r_opis *)(sh_ref+sizeof(struct sh_start));
      tabs_to_mem(&sh_length);              // sh_length != 0 - заполнение
     }
   else     // управляющая программа
     {
      tab_wr_code=0xaaaa;
      if(fstat(fpt1, &fd))   // нужен размер памяти
        {
         syslog(LOG_NOTICE,"Управляющая:fstat:%s\n",strerror(errno));
         perror("Управляющая:fstat:");
         exit(0);
        }
      sh_ref=mmap(NULL, fd.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fpt1, 0);
      if(sh_ref == MAP_FAILED)
        {
         syslog(LOG_NOTICE,"Управляющая:mmap:%s\n",strerror(errno));
         perror("Управляющая:mmap:");
         exit(0);
        }
      close(fpt1);
      c=0;
      to_start = (struct sh_start *)sh_ref;
cyc1:
      if(to_start->sh_flag)
        {
         memset(&Mes_line,0,sizeof(Mes_line));
         if((mes_tab_id=opnglb(name3,"",&i))<0)
          {
           syslog(LOG_NOTICE,":err.open '%s' = %x\n",name3,mes_tab_id);
           printf("err.open '%s' = %x\n",name3,mes_tab_id);
           goto bce;
          }
         crd[0]=crd[1]=crd[4]=crd[5]=crd[6]=1;
         crd[2]=crd[3]=to_start->Mess_tuple;               // Установлен в CommTask
         if((i=dtrdbl(Mes_line.ECS_name, sizeof(Mes_line.ECS_name), crd,mes_tab_id))<0)  // читаю первый столбец
           {
            syslog(LOG_NOTICE,":err.dtrdbl '%s' = %x\n",name3, i);
            printf("err.dtrdbl '%s' = %x\n",name3, i);
            goto bce;
           }
         goto bce;
        }
      if( c != 10)  //  память не размечена
        {
         sleep(1);  //  управляющая программа ждёт ещё 10 с
         ++c;
         goto cyc1;
        }
      else
        return 1;   // память не была размечена за время ожидания
     }
bce:
   tab_first = (struct r_opis *)(sh_ref+sizeof(struct sh_start));
   if(sigemptyset(&mask))
     {
      syslog(LOG_NOTICE,":sigemptyset:%s\n",strerror(errno));
      perror("sigemptyset");
      exit(0);
     }
   if(sigaddset(&mask,SIGUSR1))   // используется сигнал SIGUSR1
     {
      syslog(LOG_NOTICE,":sigaddset:%s\n",strerror(errno));
      perror("sigaddset");
      exit(0);
     }
   Xx.sa_handler=handler;
   Xx.sa_flags=SA_RESTART;
   if(sigaction(SIGUSR1,&Xx,NULL))  // set handler
     {
      syslog(LOG_NOTICE,":sigaction:%s\n",strerror(errno));
      perror("sigaction");
      exit(0);
     }
   return 0;
  }
unsigned int sh_get_count()            // получить счётчик обновлений от связной
  {
   return(to_start->S_put_sig);
  }
short sh_put_data_er(short er_code)    // из управляющей программы ошибка управляющих данных
  {
   to_start->err_data=er_code;         // < 0 - ошибка
   ++to_start->C_put_count;            // меняю счётчик управления
   if(msync(&to_start->C_put_count, 2*(sizeof(int)+sizeof(short)), MS_SYNC | MS_INVALIDATE))
    if(errno != EINVAL)
      {
       syslog(LOG_NOTICE,":sh_put_data_er:msync:%s\n",strerror(errno));
       perror("sh_put_data_er:msync: ");
       return 0x8000;
      }
   return 0;
  }
short sh_put_meas_er(short er_code)     // из управляющей ошибка измерений
  {
   to_start->err_meas=er_code;          // < 0 - ошибка
   ++to_start->C_count;                 // меняю счётчик управления
   if(msync(&to_start->C_count, sizeof(int)+sizeof(short), MS_SYNC | MS_INVALIDATE))
    if(errno != EINVAL)
      {
       syslog(LOG_NOTICE,":sh_put_meas_er:msync:%s\n",strerror(errno));
       perror("sh_put_meas_er:msync: ");
       return 0x8000;
      }
   return 0;
  }
void sh_put_mess(char *mes)             // положить сообщение в архив U70_ECS_MESSAGES
  {
  size_t a;
  short crd[7],i;
   if(mes_tab_id < 0 || !Mes_line.ECS_name[0])
     return;
   memset(Mes_line.messa,0,sizeof(Mes_line.messa));
   strcpy(Mes_line.mes_time,our_time());
   a=strlen(mes);
   if(a > (sizeof(Mes_line.messa)-2))
     a=sizeof(Mes_line.messa)-2;
   strncpy(Mes_line.messa,mes,a);
   crd[0]=crd[1]=to_start->Mess_cur_pl;
   crd[2]=crd[3]=to_start->Mess_tuple;
   crd[4]=crd[6]=1;
   crd[5]=3;
   if((i=dtwdbl(&Mes_line, sizeof(Mes_line), crd,mes_tab_id))<0)  // пишу строку
     {
      syslog(LOG_NOTICE,":err.dwrdbl '%s' = %x\n",name3, i);
      printf("err.dtwdbl '%s' = %x\n",name3, i);
      return;
     }
   ++to_start->Mess_cur_pl;
   if(to_start->Mess_cur_pl == 11)    // 10 плоскостей сообщений
     to_start->Mess_cur_pl=1;
   if(msync(&to_start->Mess_cur_pl, sizeof(to_start->Mess_cur_pl), MS_SYNC | MS_INVALIDATE))
    if(errno != EINVAL)
      {
       syslog(LOG_NOTICE,":sh_put_mess:msync:%s\n",strerror(errno));
       perror("sh_put_mess:msync: ");
      }
  }

short sh_optb(char *tab_name)
  {
  short t_id=tab_id_base;
  struct r_opis *cur_opis;
   cur_opis=tab_first;
cyc_1:
   if(!strcmp(tab_name,cur_opis->r_name))
     return t_id;
   else
     {
      ++t_id;
      if(cur_opis->next)
        {
         cur_opis= (struct r_opis *)(sh_ref+cur_opis->next);
         goto cyc_1;
        }
     }
   return 0x8000;     // нет такой таблицы
  }

struct r_opis *get_tab(short t_id)  // найти ранее открытую таблицу
  {
  short i;
  struct r_opis *cur_opis;
   cur_opis=tab_first;
   i=t_id-tab_id_base;
   if(i<0)
     return 0;         //  неправильный идентификатор таблицы
cyc_1:
   if(!i)
     return cur_opis;  // таблица найдена
   if(!cur_opis->next)
     return 0;         //  неправильный идентификатор таблицы
   cur_opis=(struct r_opis *)(sh_ref+cur_opis->next);
   --i;
   goto cyc_1;
  }

short sh_lptb(char *buf, short tab_id)
  {
  struct r_opis *cur_opis;
   if(!(cur_opis=get_tab(tab_id)))
    return 0x8000;     // нет такой таблицы
   memcpy(buf,&cur_opis->r_name,20+4*sizeof(short));  // имя (20), число строк, столбцов, длина строки, последнее действие
   return 0;
  }
short sh_attb(char *buf, short N_attr, short tab_id)
  {
  struct r_opis  *cur_opis;
  struct at_opis *to_attr;
  char *b_ref;
   if(!(cur_opis=get_tab(tab_id)))
    return 0x8000;                    // нет такой таблицы
   if(N_attr > cur_opis->n_attr)
    return 0x8000;                    // нет такого столбца
   b_ref= (char *)cur_opis+sizeof(struct r_opis);
   to_attr=(struct at_opis *)b_ref;   // на описание первого столбца или матрицы
   if(!cur_opis->flag)                // не матрица
     {
cyc_at:
      --cur_opis->n_attr;
      if(cur_opis->n_attr)
        {
         ++to_attr;
         goto cyc_at;
        }
     }
   memcpy(buf,to_attr,sizeof(struct at_opis)); // char длина, char тип, short смещение (не для матрицы!!!)
   return 0;
  }
short sh_rd_wr(int rdwr_fl, char *buf, unsigned int length, short *crd, short tab_id)
  {
  struct r_opis *cur_opis;
  struct at_opis *to_attr;
  char *b_ref;
  unsigned short offset=0, len_in_tup=0, buf_ind=0, i, sum=0;
   if(!length)
    return 0x8000;                             // нулевая длина
   if(!(cur_opis=get_tab(tab_id)))
    return 0x8000;                             // нет такой таблицы
   if(crd[0] < 1 || crd[0] > cur_opis->n_tup || crd[0] > crd[1])
    return 0x8000;                             // координата первой строки
   if(crd[1] > cur_opis->n_tup)
    return 0x8000;                             // координата последней строки
   if(crd[2] < 1 || crd[2] > cur_opis->n_attr || crd[2] > crd[3])
    return 0x8000;                             // координата первого столбца
   if(crd[3] > cur_opis->n_attr)
    return 0x8000;                             // координата последнего столбца
   if(cur_opis->flag)                   // матрица -> установить offset в строке и len_in_tup
     {
      len_in_tup=cur_opis->flag*(crd[3]-crd[2]+1);
      offset=cur_opis->flag*(crd[2]-1);
     }
   else                                 // таблица -> установить offset в строке и len_in_tup
     {
      b_ref= (char *)cur_opis+sizeof(struct r_opis);
      to_attr=(struct at_opis *)b_ref;  // на описание первого столбца
      for(i=1;i<crd[2];i++)
        offset += (to_attr+i-1)->at_len;
      for(i=crd[2];i<=crd[3];i++)
        len_in_tup +=(to_attr+i-1)->at_len;
     }
   b_ref= (char *)cur_opis+cur_opis->tab_data+offset; // на начало данных внутри строки
   for(i=crd[0];i<=crd[1];i++)
    {
     if(len_in_tup > length)
       len_in_tup = length;
     if(rdwr_fl)                    // запись в таблицу
       memcpy(b_ref+cur_opis->tup_len*(crd[0]-1)+cur_opis->tup_len*(i-crd[0]),buf+buf_ind,len_in_tup);
     else                           // чтение из таблицы
       memcpy(buf+buf_ind,b_ref+cur_opis->tup_len*(crd[0]-1)+cur_opis->tup_len*(i-crd[0]),len_in_tup);
     length -= len_in_tup;
     sum += len_in_tup;
     if(!length)
       break;                       // буфер заполнен
     buf_ind += len_in_tup;
    }
   if(rdwr_fl)                      // запись в таблицу
     cur_opis->last_act=tab_wr_code;
   else                             // чтение из таблицы
     cur_opis->last_act=0;
   return sum;
  }
short sh_drtb(char *buf, unsigned int length, short *coord, short tab_id)
  {
   return(sh_rd_wr(0, buf, length, coord, tab_id));
  }
short sh_dwtb(char *buf, unsigned int length, short *coord, short tab_id)
  {
   return(sh_rd_wr(1, buf, length, coord, tab_id));
  }
int API_wait_ev()
  {
  int sig;
   return(sigwait(&mask, &sig));
  }
short API_ECSini()
  {
  short i;
   if(i=sh_ECSini(0))
     return i;
   to_start->cntr_pid=getpid();    // PID управляющей программы
   if(msync(&to_start->cntr_pid, sizeof(pid_t), MS_SYNC | MS_INVALIDATE))
    if(errno != EINVAL)
      {
       syslog(LOG_NOTICE,":API_ECSini:msync:%s\n",strerror(errno));
       perror("API_ECSini:msync: ");
      }
   return(0);
  }
char *get_sh_addr()
  {
   return sh_ref;
  }

