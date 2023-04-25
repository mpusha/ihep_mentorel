#ifndef DB_H
#define DB_H

// данные управляющие
#define ALLVECTORS 32 ///< колличество векторов в массиве входных данных
#define MAXTIME 50.0  ///< максимально устанавливаемое время
#define MAXAMP 10.01 ///< максимальная амплитуда ВЧ

/*!
 * \brief The rfAmpSetData struct структура записываемых данных в аппаратуру генератора функций (ГФ)
 */
struct rfAmpSetData {
  double time[ALLVECTORS];///< время начала вектора в мс (первый элемент всегда равен 0)
  double realAmp[ALLVECTORS];///< реальное ВЧ (вычисленное)
  double length[ALLVECTORS];///< длина вектора в мс
  double rfAmp[ALLVECTORS]; ///< амплитуда ВЧ в кВ на начало участка
  int forcing; ///< форсировка 0 - выкл, 1 - 13 %, 2 - 26 %
  int overload; ///< Превышение амплитуды
};

struct k_opis {
   char r_name[20];
   unsigned short n_tup;    // число строк
   unsigned short n_attr;   // число столбцов
   unsigned short tup_len;  // длина строки
   unsigned short last_act; // последнее действие 0 - чтение, 0xcccc - запись связной, 0xaaaa - запись управляющей
  } ;
#endif

