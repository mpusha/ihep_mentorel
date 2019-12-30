//                     cc -o Fill_M Fill_M.c /usr/usera/voevodin/rt-data/ccydalib/ccydalib.a
#define MAX_NB_PARAM  32

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
struct A_par {
        char PASS_tab[20];
        short plane;        // может быть < 0
        short tuple;        // может быть < 0
        char DATA_tab[20];
         } ;
static struct _U70_main {
        short flag0;        // set by meas.task if <0  or >255 - error code 
                            // 0-255 is counter after each cycle of appl. 
        short flag1;        // set by cont.task if <0  or >255 - error code 
                            // 0-255 is counter after each cycle of appl. 
        short flag2;        // is set by console prog. if 1 - new data  
                            // 1 - new data 
                            // 2 - write data to the equipment 
                            // 4 - read data from the equipment 
                            // 8 - new console is turned on 
                            // 0x8000 - console finished work with appl. 
        char name_form;     // 0 -  имена берутся из Паспорта, (5)M/(3)N - M из Паспорта/остальные из данных последних N параметров
        char par_num;       // number of parameters to be show (number of structures A) 
        struct A_par params[MAX_NB_PARAM];
       } For_Main;

static char buf[200], pm_name[22];
static int i, j, n, Fl_DB=0;       // флаг записи в БД = 0 - писать, иначе была ошибка и дальше не писать 
static int Fl_Main;                // флаг о прочитанной строке при обработке строк описаний паспортов/данных
                                   // = 1 - прочитано имя таблицы _PM (паспорта/данные ещё не читал)
                                   // = 2 - прочитана пустая строка
                                   // = 3 - прочитан конец файла
                                   // = 4 - прочитана не паспортная строка
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
      printf("Нужно одно имя файла описания экранов!!!\n");
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
   if(strcmp(buf,"ECS_P_MAIN"))
     {
      printf("В первой строке жду ECS_P_MAIN, а имею: %s\n", buf);
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
   while(fgets(buf,sizeof(buf),fpoi))     // buf = строка с именем и четырьмя параметрами для MAIN 
     {
cyc_2:
      if(buf[0]==0 || buf[0]=='\n')       // пустая строка заканчивает работу
       {
        printf("Встретил пустую строку ==> конец работы\n");
        exit(0);        
       }
      put_end_0();         // Обнулил NewLine
      if(buf[1]!='_' || buf[2]!='P' || buf[3]!='M')
        {
         printf("Неверное имя таблицы - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      memset(&For_Main,0,sizeof(For_Main));
      memset(pm_name,0,sizeof(pm_name));
      Fl_Main=1;           // прочитано имя таблицы _PM
      for(i=0;i<20;i++)    // копировать имя MAIN таблицы
        {
         if(buf[i]!=' ' && buf[i]!='"')
           pm_name[i]=buf[i];
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
      if(!sscanf(buf+i,"%x",&j))
        {
         printf("Нет / - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      For_Main.name_form = j&255;
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после способа формирования имён параметров - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      ++i;
      if(!sscanf(buf+i,"%d",&n))  // n = число структур A_par для записи в MAIN
        {
         printf("Нет числа параметров - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      while(buf[i]!=0 && buf[i]!='"')
        i++;
      if(!buf[i])
        {
         printf("Нет \" после числа параметров - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      if(n>MAX_NB_PARAM || n<=0)
        {
         printf("Число параметров больше допустимых MAX_NB_PARAM или 0 или <0 - %s\n", buf);
         Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         continue;
        }
      For_Main.par_num=n;
Cyc_intern:
      if(fgets(buf,sizeof(buf),fpoi))     // buf = следующая строка 
        {
         if(buf[0]==0 || buf[0]=='\n')       // пустая строка заканчивает работу
          {
           printf("Встретил пустую строку ==> конец работы\n");
           Fl_Main=2;           // прочитана пустая строка
           goto end_write;        
          }
         put_end_0();           // Обнулил NewLine
         if(buf[0]!=' ') 
           {
            Fl_Main=4;          // прочитана не паспортная строка
            goto end_write;        
           }
         i=1;
         if(!sscanf(buf+i,"%d",&j))  // j = номер параметра в A_par для записи в MAIN
           {
            printf("Нет номера параметра - %s\n", buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         if(j>For_Main.par_num)
           {
            printf("Номер параметра больше числа параметров %d > %hhd - %s\n",j,For_Main.par_num, buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         while(buf[i]!=0 && buf[i]!='"')
          i++;
         if(!buf[i])
           {
            printf("Нет \" после номера параметра - %s\n", buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         i++;
         if(For_Main.params[j-1].PASS_tab[0])
           {
            printf("Имя паспортной таблицы уже есть у параметра %d - %s\n",j, buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         for(k=0;k<20;k++,i++)    // копировать имя паспортной таблицы
          {
           if(buf[i]!=' ' && buf[i]!='"')
             For_Main.params[j-1].PASS_tab[k]=buf[i];
           if(buf[i]=='"')
             break;
          }
         if(buf[i]!='"')
          {
           printf("Нет \" после имени паспортной - %s\n", buf);
           Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
           goto Cyc_intern;
          }
         ++i;
         if(!sscanf(buf+i,"%hd",&For_Main.params[j-1].plane))  
           {
            printf("Нет номера плоскости параметра номер %d - %s\n",j, buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         while(buf[i]!=0 && buf[i]!='"')
          i++;
         if(!buf[i])
           {
            printf("Нет \" после номера плоскости параметра номер %d - %s\n",j, buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         ++i;
         if(!sscanf(buf+i,"%hd",&For_Main.params[j-1].tuple))  
           {
            printf("Нет номера строки параметра номер %d - %s\n",j, buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         while(buf[i]!=0 && buf[i]!='"')
          i++;
         if(!buf[i])
           {
            printf("Нет \" после номера строки параметра номер %d - %s\n",j, buf);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
            goto Cyc_intern;
           }
         ++i;
         for(k=0;k<20;k++,i++)    // копировать имя таблицы данных
          {
           if(buf[i]!=' ' && buf[i]!='"')
             For_Main.params[j-1].DATA_tab[k]=buf[i];
           if(buf[i]=='"')
             break;
          }
         if(buf[i]!='"')
          {
           printf("Нет \" после имени таблицы данных - %s\n", buf);
           Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
           goto Cyc_intern;
          }
         --n;
         if(n < 0)
          {
           printf("Число описаний превысило указанное для MAIN число параметров\n");
           Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
          }
         goto Cyc_intern;
        }
      else 
        {
         printf("Встретил конец файла ==> конец работы\n");
         Fl_Main=3;           // прочитан конец файла
         goto end_write;        
        }
end_write:
      coord[5]=5+4*For_Main.par_num;  
      if(!Fl_DB)
       {
        if((tab_id=ropdbl(pm_name, ""))<0)
         {
          printf("Не могу открыть таблицу - %s ошибка = %hx\n", pm_name, tab_id);
          Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
         }
        else
         {
          if((k=dtwdbl(&For_Main,sizeof(For_Main),coord,tab_id))<0)
           {
            printf("Не могу записать в таблицу - %s ошибка = %hx координаты %hd-%hd %hd-%hd %hd-%hd\n", pm_name, k, coord[0], coord[1],
              coord[2], coord[3],coord[4], coord[5]);
            rcldbl(tab_id);
            Fl_DB=1;          // тогда далее только контроль синтаксиса без записи в БД
           }
          else
           {
            printf("Записана в таблицу - %s плоскость %hd строка %hd и ссылки:\n", pm_name, coord[0], coord[2]);
            for(k=0;k<For_Main.par_num;k++)
              printf("   %s p=%hd t=%hd %s\n",For_Main.params[k].PASS_tab,For_Main.params[k].plane,
                 For_Main.params[k].tuple,For_Main.params[k].DATA_tab);
           }
          rcldbl(tab_id);
         }
       }
      if(Fl_Main == 2 || Fl_Main == 3)
        exit(0);
      if(Fl_Main == 4)
        goto cyc_2;
     }    // while
   printf("Встретил конец файла ==> конец работы\n");
  }

