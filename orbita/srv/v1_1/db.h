#ifndef DB_H
#define DB_H

// данные управляющие
//static double tim_[2], cur_tim_[2];      // время в секундах
//static char   puls[2], cur_puls[2];      // стартовый импульс
#define allAxis 60
#define timePoints 16
struct orbSetData {
  double ti[16];
  double tresholdI;
  char stStop,launch,dir,single,mode,math,work,graph;
};

//
struct k_opis {
   char r_name[20];
   unsigned short n_tup;    // число строк
   unsigned short n_attr;   // число столбцов
   unsigned short tup_len;  // длина строки
   unsigned short last_act; // последнее действие 0 - чтение, 0xcccc - запись связной, 0xaaaa - запись управляющей
  } ;




#endif

