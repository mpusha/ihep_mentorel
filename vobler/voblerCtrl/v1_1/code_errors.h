#ifndef CODE_ERRORS_H
#define CODE_ERRORS_H

namespace CODEERR{

// Error 16 bits word
enum TCodeErrors
{
  NONE = 0,
  INITIALISE_ERROR,// ошибка инициализации (FPGA don't work)
  BBB_ERROR,       // ошибка в процессорном модуле GPIO or SPI
  HW_ERROR,        // ошибка in HW
  INFILE_ERROR,    // input file don't found
  SYNCHRO_ERROR,
  PS_ERROR,        // ошибка in PS HW
  NOTREADY         // не готов
};

}

#endif // CODE_ERRORS_H
