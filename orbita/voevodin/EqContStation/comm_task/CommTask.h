#define MAX_NB_PARAM  32
#define MAX_MEAS_TABS 20
#define MAX_TABS 30

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
        char axis;          // 0 - parameter is row, 1 - column 
        char par_num;       // number of parameters to be show (number of structures A) 
        struct A_par params[MAX_NB_PARAM];
       } For_MAIN;

static struct _U70_P_pass {
        short flag1;        /* is set by appl. if <0 - error code */
        short flag2;        /* is set by console prog. if 81 - new data */
        short copy;
        char par_name[16];
        char out_form[10];
        short num_items;
        short var_type;
        short par_type;
        short control;
        char min[8];
        char max[8];
       } For_Passport;

struct _U70_T_pass {
        short flag1;     /* is set by appl. if <0 - error code */  
        short flag2; 
        short copy;
        char par_name[16];
        char out_form[10];
        short num_items;
        short num_plane;
        short par_type;
       } ;

struct my_ref_to_main {
    short main_tab_id;
    short m_pl;
    short m_tup;
    short num_of_par;
    short main_state;       // 0-ready, 1-ожидание ответа от управляющей и в MAIN flag2=81
   struct ref_to_pass {
          short pass_tab_id;
          short p_pl;                 // < 0 ==> эталоны
          short p_tup;                // < 0 ==> нередактируемые
          short num_of_val;
          short dat_tab_id;           // идентификатор таблицы данных на диске
          short sh_tab_id;            // идентификатор той же таблицы данных в памяти
          short stat_flag;            // флаг наличия статистики измерений у параметра = 1
       } r_to_p[MAX_NB_PARAM];
   };

struct Tab_Opened {
    short t_i;                 // идентификатор таблицы
    char  t_nam[20];           // имя таблицы
   } TabOp[MAX_TABS];

static short meas_tabs[MAX_MEAS_TABS], meas_data_tabs[MAX_MEAS_TABS];  // идентификаторы таблиц измерений соответственно в памяти и на диске
static char stat_meas_tabs[MAX_MEAS_TABS];   // флаги наличия статистики измерений у таблицы
static int ind_m_t=0, pl_stat_m=5;           // число измерительных таблиц, текущая плоскость записи измерений в статистику


