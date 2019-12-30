#include "../sh_ccydalib/sh_ccydalib.h"
#define num_in_string 10            // максимум чисел в строке вывода данных

static short i;
static char *sh_ref, *dat_ref, buf[200], *to_buf;
static struct sh_start *to_start;
static struct r_opis   *tab_opis;
static struct at_opis  *to_attr;
static int l, n, m;
static char *vari[]={
     "8-бит целое",
     "16-бит целое",
     "32-бит целое",
     "32-бит с плавающей",
     "64-бит с плавающей",
     "шестнадцатиричное",
     "текст"
      };
char *get_sh_addr();

void pri_tid(char *txt, char *tid)
  {
  int i;
   printf("  %s  ",txt);
   for(i=0; i<sizeof(pid_t);i++)
     printf("%3d",tid[i]&255);
   printf("\n");
  }
binhex(char c,char *bb)
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
   *bb =c;
  }

main()
  {
   if(i=sh_ECSini(0))
     {
      printf("Ошибка при вызове sh_ECSini = %d\n", i);
      exit(0);
     }
   sh_ref=get_sh_addr();
   to_start=(struct sh_start *)sh_ref;
   printf("\nПолный размер общей памяти = %ld\n",to_start->shmem_size);
   if(to_start->sh_flag)
     printf("  память размечена\n");
   else
     printf("  память не размечена!!??\n");
   printf("PIDы задач по байтам в десятичной форме\n");
   pri_tid("Связная прг", (char *)&to_start->comm_pid);
   pri_tid("Управляющая", (char *)&to_start->cntr_pid);
   printf("Связная программа установила циклический счётчик = %d\n",to_start->S_put_sig);
   printf("Управляющая счётчики управления =%d измерений =%d\n",to_start->C_put_count, to_start->C_count);
   printf("            коды ошибок управления=%hx измерения=%hx\n",to_start->err_data, to_start->err_meas);
   printf("В таблице U70_ECS_MESSAGES используется строка %hd, текущая плоскость %hd\n",to_start->Mess_tuple,to_start->Mess_cur_pl);
   printf("В память загружены следующие таблицы:\n");
step_0:
   tab_opis= (struct r_opis *)(sh_ref+sizeof(struct sh_start));
   m=1;
   while(tab_opis)
    {
     if(tab_opis->r_name[0])
       {
        if( !tab_opis->flag)
         printf("  %2d. таблица %20s столбцов=%3d строк=%3d длина строки=%d смещение данных=%ld\n",
            m++,tab_opis->r_name,tab_opis->n_attr, tab_opis->n_tup, tab_opis->tup_len, tab_opis->tab_data);
        else
         printf("  %2d. матрица %20s столбцов=%3d строк=%3d длина строки=%d длина элемента=%d смещение данных=%ld\n",
            m++,tab_opis->r_name,tab_opis->n_attr, tab_opis->n_tup, tab_opis->tup_len, tab_opis->flag, tab_opis->tab_data);
        switch(tab_opis->last_act)
          {
           case 0:
           printf("      Последнее действие с данными -Чтение\n");
           break;
           case 0xcccc:
           printf("      Последнее действие с данными -Запись связной программой\n");
           break;
           case 0xaaaa:
           printf("      Последнее действие с данными -Запись управляющей программой\n");
           break;
           default:
           printf("      Неверный код последнего действия = %x !!!\n",tab_opis->last_act);
          }
        if( !tab_opis->next)
          break;
        tab_opis = (struct r_opis *)(sh_ref+tab_opis->next);
       }
    }
   printf("Введи номер таблицы для просмотра (0 - конец работы): ");
   i=0;
   while((buf[i]=getchar())!='\n')
     i++;
   buf[i++]=0;
   n=0;
   sscanf(buf,"%d",&n);
   if(n<=0)
     {
      printf("Конец работы!!!\n");
      exit(0);
     }
   if(n>m)
     {
      printf("Неверный номер таблицы. Конец работы!!!\n");
      exit(0);
     }
   tab_opis= (struct r_opis *)(sh_ref+sizeof(struct sh_start));  // для первой
   --n;
   while(n--)
     tab_opis = (struct r_opis *)(sh_ref+tab_opis->next);        // для последующих
step_1:
   printf("   %s\nЧто смотрим\n1.Описания столбцов\n2.Данные таблицы\nВведи номер (0 - назад): ", tab_opis->r_name);
   i=0;
   while((buf[i]=getchar())!='\n')
     i++;
   buf[i++]=0;
   n=0;
   sscanf(buf,"%d",&n);
   if(n<0 || n>2)
     goto step_1;
   printf("\n");
   if(!n)
     goto step_0;
   switch(n)
     {
      case 1:             // Описания столбцов
      to_attr=(struct at_opis *)((char *)tab_opis + sizeof(struct r_opis));
      if(tab_opis->flag)  // матрица
        printf("   Все столбцы %s, длина элемента в байтах = %d\n", vari[to_attr->at_type-1], to_attr->at_len); 
      else                // таблица
        for(n=0;n<tab_opis->n_attr;n++)
         {
          printf("   %3d. %s, длина элемента в байтах = %d\n", n+1, vari[to_attr->at_type-1], to_attr->at_len);
          ++to_attr;
         }
      break;
      case 2:             // Данные таблицы
      dat_ref=tab_opis->tab_data + (char *)tab_opis;
      for(m=0;m<tab_opis->n_tup;m++)        //по строкам
        {
         printf("Строка %d\n  ",m+1);
         to_attr=(struct at_opis *)((char *)tab_opis + sizeof(struct r_opis));
         for(n=1;n<=tab_opis->n_attr;n++)    // по столбцам
           {
            switch(to_attr->at_type)
              {
               case 1:  // 8-бит целое
               printf("%hhu ", *dat_ref);
               ++dat_ref;
               break;
               case 2:  // 16-бит целое
               printf("%hu ",*(short *)dat_ref);
               dat_ref += 2;
               break;
               case 3:  // 32-бит целое
               printf("%lu ",*(long *)dat_ref);
               dat_ref += 4;
               break;
               case 4:  // 32-бит с плавающей
               break;
               printf("%g ",*(float *)dat_ref);
               dat_ref += 4;
               case 5:  // 64-бит с плавающей
               printf("%lg ",*(double *)dat_ref);
               dat_ref += 8;
               break;
               case 6:  // шестнадцатиричное
               memset(buf,0,sizeof(buf));
               to_buf=buf;
               for(l=0; l<to_attr->at_len; l++)
                 {
                  binhex(dat_ref[l], to_buf);
                  to_buf += 2;
                 }
               printf("%s ", buf);
               dat_ref += to_attr->at_len;
               break;
               case 7:  // текст
               memset(buf,0,sizeof(buf));
               strncpy(buf,dat_ref,to_attr->at_len);
               printf("'%s' ", buf);
               dat_ref += to_attr->at_len;
               break;
               default:
               printf("Неверный тип данных = %d строка=%d столбец=%d\n", to_attr->at_type, m+1, n+1);
               exit(0);
               break;
              }
            if( !(n%num_in_string) && tab_opis->n_attr!=num_in_string)
              printf("\n  ");
            if(!tab_opis->flag)  // таблица
              ++to_attr;
           }
         printf("\n");
        }
      break;
     }
   while(getchar()!='\n') // жду ввода Enter
     ;
   goto step_1;
  }

