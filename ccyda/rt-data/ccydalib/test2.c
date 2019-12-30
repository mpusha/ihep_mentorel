#include<stdio.h>

#define mx 66
#define mt 1056
short opnglb();
static char tb_name[]={"B_TD_BTL1_MEAS"},user_name[]={""};
static char tb_name1[]={"B_TD_BTL1_EC1"};
static char tb_name2[]={"B_TD_BTL1_EC2"};
unsigned short i,j;
short k,tb_id,coord[7],bs_num,tb_id1;
short DBUF[200],OBUF[1056],NBUF[1056];
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

printf("%s %s \n",tb_name,tb_name1);
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
   coord[3]=8;
   coord[4]=1;
   coord[5]=8;
   coord[6]=3;

   if((k=dtrdbl((char *)DBUF,sizeof(DBUF),coord,tb_id))<0)
      {
      printf("dtrdbl: %s => %x\n",tb_name,k);
      goto ep;
      }

   for(i=0;i<15;i++) 
    {
     for(j=0;j<8;j++)
       printf("%4d ",DBUF[i*8+j]&0xffff);
     printf("\n");
    }
exit(0);
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
   coord[3]=1;
   coord[4]=1;
   coord[5]=1;
   coord[6]=3;

   if((k=dtrdbl((char *)&DBUF,sizeof(DBUF),coord,tb_id1))<0)
      {
      printf("READY READ_ERR=>%x\n",k);
      goto ep;
      }
printf("Ready = %x \n", DBUF[0]);
ep:
       rcldbl(tb_id);
      }



















