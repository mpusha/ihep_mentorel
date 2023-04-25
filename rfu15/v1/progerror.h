#ifndef PROGERROR_H
#define PROGERROR_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <bitset>
#include "code_errors.h"

// Extended error of device
class TDevErrors : public QObject
{
  Q_OBJECT
public:
  explicit TDevErrors(QObject *parent = 0);
  ~TDevErrors();

  enum DevErrors
  {
    FPGA_LOAD_ERROR=0,
    GPIO_ERROR,
    RW_DATA_ERROR,
    INFILE_ERROR,
    SYNCHR_ERROR,
    PS_HW_ERROR
  };

  void clearDevError(void){ bitError.reset();}

  void setDevError(DevErrors err);
  unsigned int readDevError(void){return (unsigned int) bitError.to_ulong();}
  short int analyseError(void);
  QStringList readDevTextErrorList(void);
  QStringList devErrorsText;

private:
  std::bitset<32> bitError;
};
#endif // PROGERROR_H

