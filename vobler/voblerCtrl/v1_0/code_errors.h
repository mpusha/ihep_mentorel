#ifndef CODE_ERRORS_H
#define CODE_ERRORS_H

namespace CODEERR{

// Error 16 bits word
enum TCodeErrors
{
  NONE = 0,
  INITIALISE_ERROR,// ошибка инициализации (FPGA don't work)
  HW_ERROR,        // ошибка in HW
  INFILE_ERROR,    // input file don't found
  PS_ERROR,        // ошибка in PS HW
  NOTREADY         // не готов
};

}

#endif // CODE_ERRORS_H
