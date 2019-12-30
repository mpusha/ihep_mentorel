#include<stdio.h>

#define mx 66
#define mt 1056
short opnglb();
static char tb_name[]={"B_TD_SSP_ALARM"},user_name[]={""};
static char tb_name1[]={"B_TD_SSP_READY"};
unsigned short i;
short k,tb_id,coord[7],bs_num,tb_id1;
short DBUF[66],OBUF[1056],NBUF[1056];
struct Alarm {
 unsigned char my_counter;
 unsigned char err_type;
 unsigned short H_supcyc;
 unsigned short L_supcyc;
 unsigned short err_code;
 unsigned char crate;
 unsigned char eq_type;
 unsigned char function;
 unsigned char number;
 unsigned short address;
 unsigned short res_co;
 unsigned short res[8];
} alarm;
main()
    {

printf("%s %s\n",tb_name,tb_name1);
if((tb_id=opnglb(tb_name,user_name,&bs_num))<0)
   {
   printf("%s OPEN_ERR=%d\n",tb_name,tb_id);
   goto ep;
   }
printf("TB_ID=%x\n",tb_id);
if((tb_id1=opnglb(tb_name1,user_name,&bs_num))<0)
   {
   printf("%s OPEN_ERR=%d\n",tb_name1,tb_id1);
   goto ep;
   }
printf("TB_ID=%x\n",tb_id1);
   coord[0]=1;
   coord[1]=1;
   coord[2]=1;
   coord[3]=16;
   coord[4]=1;
   coord[5]=1;
   coord[6]=3;

   if((k=dtrdbl((char *)&alarm,sizeof(alarm),coord,tb_id))<0)
      {
      printf("ALARM READ_ERR=>%x\n",k);
      goto ep;
      }
/*   for(i=0;i<8;i++)
     printf("%x ",DBUF[i]);
   printf("\n");
*/
printf("err_type=%x my_con=%d supcyc=%d - %d err_code=%x eq_type=%d CNAF = %d %d %d %d \n",
alarm.err_type,
alarm.my_counter,
alarm.H_supcyc,
alarm.L_supcyc,
alarm.err_code,
alarm.eq_type,
alarm.crate,
alarm.number,
alarm.function,
alarm.address);
   coord[0]=1;
   coord[1]=1;
   coord[2]=1;
   coord[3]=4;
   coord[4]=1;
   coord[5]=1;
   coord[6]=3;

   if((k=dtrdbl((char *)&DBUF,sizeof(DBUF),coord,tb_id1))<0)
      {
      printf("READY READ_ERR=>%x\n",k);
      goto ep;
      }
printf("Ready = %x %x %x %x\n", DBUF[0], DBUF[1], DBUF[2], DBUF[3]);
ep:
       rcldbl(tb_id);
      }



















