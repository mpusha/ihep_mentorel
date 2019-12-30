#ifndef PROGERROR_H
#define PROGERROR_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <bitset>


// Extended error of device
class TDevErrors : public QObject
{
  Q_OBJECT
public:
  explicit TDevErrors(QObject *parent = 0);
  ~TDevErrors();

// Error return from orbBehav
  enum DevErrors
  {
    NONE=0,
    FPGA_LOAD_ERROR,
    GPIO_ERROR,
    INFILE_ERROR,
    RW_DATA_ERROR,
    SYNCHR_B2_ERROR,
    SYNCHR_KC1_ERROR,
    SYNCHR_RES_ERROR,
    SETLOCALE_ERROR,
    LOW_INTENSITY_ERROR
  };

  void clearDevError(void){ bitError.reset();}

  void setDevError(DevErrors err);
  unsigned int readDevError(void){return (unsigned int) bitError.to_ulong();}
  short int analyseError(void);
  QStringList readDevTextErrorList(void);
  QString readDevTextErrorListDB(void);
  QStringList devErrorsText;

private:
  std::bitset<32> bitError;
};
#endif // PROGERROR_H

