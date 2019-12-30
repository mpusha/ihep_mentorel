#include <stdio.h>
#include "ccydalib.h"
#define b_size 31*29
short opnglb(),inidbl(),setdbl(),detdbl(),dirdbl(),txrdbl(),ropdbl(),
      rcldbl(),reddbl(),rlpdbl(),tupdbl(),atrdbl(),dtrdbl(),dtwdbl();
struct att_opis {
	unsigned char at_len;
	unsigned char at_typ;
	unsigned short at_offset;
		} *a_opis;
struct tup_opis {
	unsigned short n_tup;
	unsigned short n_attr;
	unsigned short tup_len;
	unsigned short n_plan;
	unsigned char r_type;
	unsigned char r_stat;
	    }  r_opis;
struct u70_menus {
  char right;
  char what_next;
  short refer;
  char legend[26];
  short graph;
  short rus;
  short danger;
  short res1;
  short user2;
  short user3;
  } menu;
static char r_ccc[]={"B_TD_DOZ_ALARM"},use_name[]={"supersys"};
static short  buf[b_size],buf_1[b_size],buf_2[b_size];
char bbb[10];
static short *iref,i,j,k1,k,k2,l,m,coord[7], bufI[32];
double aa=1.1;
extern short cur_db;
main()
  {

   inidbl(2);
   setdbl(2);
/*
   i=rdirdbl();
   printf("Num.Tabs=%d\n",i);
   for(j=1;j<=i;j++)
     {
      if(!(k1=getname(buf,1000,j,1)))
        printf("Err.Name\n");
      else
       {
        buf[(k1+1)/2]=0;
        printf(" %d %d. %s\n",k1,j,buf);
       }
     }
*/
   if((k=ropdbl("U70_COMMENT_TEXT", use_name))<0)
    {
     printf("err.open U70_COMMENT_TEXT = %x\n",k);
     exit(0);
    }
   reddbl(1,k);
   coord[6]=1;
   coord[0]=1;
   coord[1]=1;
   coord[2]=75;
   coord[3]=75;
   coord[4]=1;
   coord[5]=1;
   for(j=1;j<=5;j++)
     {
      coord[4]=coord[5]=j;
      sprintf((char*)buf,"%d.abcd",j);
      if((i=dtwdbl(buf,14,coord,k))<0)
        {
         printf("DTWDBL ==> %x relnum=%d\n", i&0xffff, k);
        }
     }

   exit(0);

/*   INITIALIZATION   */
/*   OPEN TABLE   */
   if((k=opnglb(r_ccc,use_name,&i))<0)
    {
     printf("err.open '%s' = %x\n",r_ccc,k);
     exit(0);
    }
   printf("%s is opened as %d\n",r_ccc,k);
   i=k-max_rel;
   printf("par=%d chi=%d mic= &%x BC=%d RT=%d\n",
     gtab[i].id_parent,
     gtab[i].id_child,
     gtab[i].id_micro,
     gtab[i].BC,
     gtab[i].RT);
   printf("ntup=%d nat=%d lotup=%d third=%d rtype=%d state=%d\n",
     gtab[i].tab_inf.ntup,
     gtab[i].tab_inf.nat,
     gtab[i].tab_inf.lotup,
     gtab[i].tab_inf.third,
     gtab[i].tab_inf.rtype&255,
     gtab[i].tab_inf.rstate&255);
   printf("res1=%d res2=%d nsub=%d nowner=%d nrr=%d nrt=%d nra=%d\n",
     gtab[i].tab_inf.res1,
     gtab[i].tab_inf.res2,
     gtab[i].tab_inf.nsub,
     gtab[i].tab_inf.nowner,
     gtab[i].tab_inf.nexlis.nrr,
     gtab[i].tab_inf.nexlis.nrt,
     gtab[i].tab_inf.nexlis.nra);
memcpy(bbb,&gtab[i].parent.sin_addr,4);
   printf("port=%d %d %d %d %d \n",
     ntohs(gtab[i].parent.sin_port),bbb[0]&255,bbb[1]&255,bbb[2]&255,bbb[3]&255);
memcpy(bbb,&gtab[i].child.sin_addr,4);
   printf("port=%d %d %d %d %d \n",
     ntohs(gtab[i].parent.sin_port),bbb[0]&255,bbb[1]&255,bbb[2]&255,bbb[3]&255);
   coord[6]=3;
cc:
   for(i=0;i<b_size;i++)
     {
      buf[i]=buf_2[i]=i;
      buf_1[i]=0;
     }
   memset(bufI,1,sizeof(bufI));
   coord[0]=1;
   coord[1]=1;
   coord[2]=1;
   coord[3]=16;
   coord[4]=1;
   coord[5]=2;
   if((i=dtwdbl(bufI,sizeof(bufI),coord,k))<0)
    {
     printf("DTWDBL ==> %x relnum=%d\n",
      i&0xffff,k);
    }
   memset(bufI,0,sizeof(bufI));

   if((i=dtrdbl(&bufI,sizeof(bufI),coord,k))<0)
    printf("DTRDBL --> %x relnum=%d\n",
     i&0xffff,k);
   else
    {
     printf("%d %d %d %d %d %d %d\n",bufI[0], bufI[1], bufI[2],bufI[3],bufI[4],bufI[5],bufI[6]);
     printf("End of job\n");
    }
   exit(0);
/*   goto cc;
*/
/*    TABLE DISCRIPTIONS    */
   if(i=rlpdbl(&r_opis,k))printf("RLPDBL--> %x\n",i);
   printf("n_tup=%d n_attr=%d tup_len=%d n_plan=%d r_type=%x r_stat=%x\n"
	   ,r_opis.n_tup,r_opis.n_attr,r_opis.tup_len,r_opis.n_plan,
	    r_opis.r_type&255,r_opis.r_stat&255);
   j=1;  l=r_opis.n_tup;
l=2;
   while(l--)
     {
       if((i=tupdbl(buf,100,j++,k))<0)
        {
         printf("TUPDBL-%d--> %x\n",j-1,i);
         continue;
        }
       printf("name_len=%d %s \n",i,buf);
     }
   j=1;   l=r_opis.n_attr;
l=2;
   a_opis=(struct att_opis *)buf;
   while(l--)
     {
       if((i=atrdbl(buf,40,j++,k))<0)
         {
          printf("ATRDBL-%d--> %x\n",j-1,i);
          continue;
         }
       printf("name_len=%d %d %d 0x%x %s\n",i,a_opis->at_len,a_opis->at_typ,
        a_opis->at_offset,buf+sizeof(struct att_opis)/2);
     }  

   if((k=opnglb("A_PD_A_DO341",use_name,&i))<0)
    {
     printf("err.open A_PD_A_DO341 = %x\n",k);
     exit(0);
    }
   printf("A_PD_A_DO341 is opened as %d\n",k);
   i=k-max_rel;
   printf("par=%d chi=%d mic= &%x BC=%d RT=%d\n",
     gtab[i].id_parent,
     gtab[i].id_child,
     gtab[i].id_micro,
     gtab[i].BC,
     gtab[i].RT);

/*   READ/WRITE TABLE BOT */

/*   coord[0]=1;
   coord[1]=1;
   coord[2]=1;
   coord[3]=1;
   coord[4]=2;
   coord[5]=2;
   coord[6]=3;
   iref = (short *)buf;
   *iref=0;
   for(l=coord[0];l<=coord[1];l++)
     for(j=coord[2];j<=coord[3];j++)
       {
        *(iref+1) = *iref+1;
        ++iref;
        *(iref+1) = *iref+1;
        ++iref;
       }
bce_wr:
   if((i=dtrdbl(buf,sizeof(buf),coord,k))<0)printf("DTRDBL--> %x\n",i&0xffff);
*/
   printf("tup_opis=%d att_opis=%d cur_db=%x\n",sizeof(struct tup_opis),sizeof(struct att_opis),cur_db);
   printf("Hello,it is all!\n");
  }

