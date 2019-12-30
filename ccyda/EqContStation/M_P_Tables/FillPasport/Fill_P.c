//                     cc -o Fill_P Fill_P.c /usr/usera/voevodin/rt-data/ccydalib/ccydalib.a
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <signal.h>
#include <string.h>

#include "/usr/usera/voevodin/rt-data/ccydalib/ccydalib.h"

static FILE *fpoi;
static struct _U70_P_pass {
        short flag1;        // is set by appl. if <0 - error code 
        short flag2;        // is set by console prog. if 81 - new data 
        short copy;
        char par_name[16];
        char out_form[10];
        short num_items;
        short var_type;
        short par_type;
        short control;
        double min;
        double max;
       } For_Passport;
static char buf[200], pp_name[22];
static int i, j, Fl_DB=0;       // флаг записи в БД = 0 - писать, иначе была ошибка и дальше не писать 
static short k, tab_id, coord[7];

void put_end_0()          // в buf меняет NewLine на 0
  {
  int i;
   for(i=0;i<sizeof(buf);i++)
     if(buf[i]==10 || buf[i]==13)
       {
        buf[i]=0;
        break;
       }
  }
main( int argc, char ** argv )
  {
   if(argc != 2)
     {
      printf("Нужно одно имя файла описания паспортов!!!\n");
      exit(0);
     }
   fpoi=fopen(argv[1],"r");
   if(fpoi==NULL)
     {
      printf("Не могу открыть файл %s\n", argv[1]);
      exit(0);
     }
   if(!fgets(buf,sizeof(buf),fpoi))
     {
      printf("Не могу прочитать первую строку файла %s\n", argv[1]);
      exit(0);
     }
   put_end_0();
   if(strcmp(buf,"ECS_P_PASSPORT"))
     {
      printf("В первой строке жду ECS_P_PASSPORT, а имею: %s\n", buf);
      exit(0);
     }
cyc_1:
   if(fgets(buf,sizeof(buf),fpoi))
    {
     if(buf[0]!='@')
       goto cyc_1;
    }
   else
     {
      printf("В файле нет строки, начинающейся с символа @\n");
      exit(0);
     }
   if(inidbl(3))
     {
      printf("Не могу инициировать БД с номером 3 (inidbl) ==> конец работы\n");
      exit(0);        
     }
   if(setdbl(3))
     {
      printf("Не могу инициировать БД с номером 3 (setdbl) ==> конец работы\n");
      exit(0);        
     }
   while(fgets(buf,sizeof(buf),fpoi))     // buf = строка описания параметра в таблице паспортов
     {
      if(buf[0]==0 || buf[0]=='\n')       // пустая строка заканчивает работу
       {
        printf("Встретил пустую строку ==> конец работы\n");
        exit(0);        
       }
      put_end_0();         // Обнулил NewLine
      if(buf[1]!='_' || buf[2]!='P' || buf[3]!='P')
        {
         printf("Неверное имя таблицы - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      memset(&For_Passport,0,sizeof(For_Passport));
      memset(pp_name,0,sizeof(pp_name));
      for(i=0;i<20;i++)    // копировать имя паспортной таблицы
        {
         if(buf[i]!=' ' && buf[i]!='"')
           pp_name[i]=buf[i];
         if(buf[i]=='"')
           break;
        }
      if(buf[i]!='"')
        {
         printf("Нет \" после имени - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%d",&j))
        {
         printf("Нет номера плоскости - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после номера плоскости - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      coord[0]=coord[1]=j;
      if(!sscanf(buf+i,"%d",&j))
        {
         printf("Нет номера строки - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после номера строки - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      coord[2]=coord[3]=j;
      coord[4]=coord[6]=1;
      coord[5]=11;
      if(!sscanf(buf+i,"%x",&For_Passport.copy))
        {
         printf("Нет профессия/приоритет - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после профессия/приоритет - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      for(j=0;j<16;j++,i++)    // копировать имя параметра
        {
         if(buf[i]!=' ' && buf[i]!='"')
           For_Passport.par_name[j]=buf[i];
         if(buf[i]=='"')
           break;
        }
      if(buf[i]!='"')
        ++i;
      if(buf[i]!='"')
        {
         printf("Нет \" после имени параметра - %s %d\n", buf,i);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      for(j=0;j<10;j++,i++)    // копировать формат вывода
        {
         if(buf[i]!=' ' && buf[i]!='"')
           For_Passport.out_form[j]=buf[i];
         if(buf[i]=='"')
           break;
        }
      if(buf[i]!='"')
        ++i;
      if(buf[i]!='"')
        {
         printf("Нет \" после формата вывода - %s %d\n", buf,i);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%d",&For_Passport.num_items))
        {
         printf("Нет числа значений параметра - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после числа значений параметра - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%d",&For_Passport.var_type))
        {
         printf("Нет типа переменной - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после типа пременной - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%d",&For_Passport.par_type))
        {
         printf("Нет типа параметра - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после типа параметра - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%d",&For_Passport.control))
        {
         printf("Нет типа контроля - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после типа контроля - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%lf",&For_Passport.min))
        {
         printf("Нет минимального значения - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после минимального значения - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%lf",&For_Passport.max))
        {
         printf("Нет максимального значения - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      memset(buf,0,sizeof(buf));
      for(j=0;j<16;j++)
        buf[j]=For_Passport.par_name[j];
      for(j=0;j<10;j++)
        buf[20+j]=For_Passport.out_form[j];
      if(!Fl_DB)
       {
        if((tab_id=ropdbl(pp_name, ""))<0)
         {
          printf("Не могу открыть таблицу - %s ошибка = %hx\n", pp_name, tab_id);
          printf("   Параметр %s copy=%x формат вывода %s число значений=%hd тип переменной=%hd тип параметра=%hd управление=%hd m/m от %lg до %lg\n",
           buf,For_Passport.copy,buf+20,For_Passport.num_items,For_Passport.var_type,For_Passport.par_type,For_Passport.control,
             For_Passport.min,For_Passport.max);
          Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
          continue;
         }
        else
         {
          if((k=dtwdbl(&For_Passport,sizeof(For_Passport),coord,tab_id))<0)
           {
            printf("Не могу записать в таблицу - %s ошибка = %hx координаты %hd-%hd %hd-%hd %hd-%hd\n", pp_name, k, coord[0], coord[1],
              coord[2], coord[3],coord[4], coord[5]);
            printf("   Параметр %s copy=%hx формат вывода %s число значений=%d тип переменной=%d тип параметра=%d управление=%d m/m от %lg до %lg\n",
              buf,For_Passport.copy,buf+20,For_Passport.num_items,For_Passport.var_type,For_Passport.par_type,For_Passport.control,
                For_Passport.min,For_Passport.max);
            rcldbl(tab_id);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            continue;
           }
          else
           {
            printf("Записана в таблицу - %s плоскость %hd строка %hd\n", pp_name, coord[0], coord[2]);
         printf("   Параметр %s copy=%hx формат вывода %s число значений=%hd тип переменной=%hd тип параметра=%hd управление=%hd m/m от %lg до %lg\n",
              buf,For_Passport.copy,buf+20,For_Passport.num_items,For_Passport.var_type,For_Passport.par_type,For_Passport.control,
                For_Passport.min,For_Passport.max);
           }
          rcldbl(tab_id);
         }
       }  // if(!Fl_DB)
     }    // while
   printf("Встретил конец файла ==> конец работы\n");
  }

