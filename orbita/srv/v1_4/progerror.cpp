
#include "progerror.h"


TDevErrors::TDevErrors(QObject *parent) : QObject(parent)
{
  devErrorsText.clear();
  bitError.reset();
}
TDevErrors::~TDevErrors()
{
}
void TDevErrors::setDevError(DevErrors err)
{
  bitError.set((int)err);
}
//-----------------------------------------------------------------------------
//---Return simple error for server answer
//-----------------------------------------------------------------------------
short int TDevErrors::analyseError(void)
{//return 1;
  short int ret=NONE;
  if(bitError.any()){
    for(uint i=0;i<bitError.size();i++){
      if(bitError.test(i))
        switch(i)
        {
          case FPGA_LOAD_ERROR:     ret=FPGA_LOAD_ERROR;  break;
          case GPIO_ERROR:          ret=GPIO_ERROR; break;
          case RW_DATA_ERROR:       ret=RW_DATA_ERROR; break;
          case INFILE_ERROR:        ret=INFILE_ERROR;break;
          case SYNCHR_B2_ERROR:     ret=SYNCHR_B2_ERROR;break;
          case SYNCHR_KC1_ERROR:    ret=SYNCHR_KC1_ERROR;
          case SYNCHR_RES_ERROR:    ret=SYNCHR_RES_ERROR;
          case SETLOCALE_ERROR:     ret=SETLOCALE_ERROR; break;
          case LOW_INTENSITY_ERROR: ret=LOW_INTENSITY_ERROR ; break;
          default:;
        }
    }
  }
  return ret;
}

QStringList TDevErrors::readDevTextErrorList(void)
{
  devErrorsText.clear();
  if(bitError.any()){
    for(uint i=0;i<bitError.size();i++){
      if(bitError.test(i))
        switch(i)
        {
          case FPGA_LOAD_ERROR:     devErrorsText.append(tr("FPGA load error.")); break;
          case GPIO_ERROR:          devErrorsText.append(tr("GPIO/SPI error.")); break;
          case RW_DATA_ERROR:       devErrorsText.append(tr("R/W file error.")); break;
          case INFILE_ERROR:        devErrorsText.append(tr("Ini file with coeff & amendment for ADCs is absent.")); break;
          case SYNCHR_B2_ERROR:     devErrorsText.append(tr("Synchr pulse B2 is absent.")); break;
          case SYNCHR_KC1_ERROR:    devErrorsText.append(tr("Synchr pulse KC1 is absent.")); break;
          case SYNCHR_RES_ERROR:    devErrorsText.append(tr("Synchr pulse Reset is absent.")); break;
          case SETLOCALE_ERROR:     devErrorsText.append(tr("System in LOCAL mode.")); break;
          case LOW_INTENSITY_ERROR: devErrorsText.append(tr("Low intensity in one of measurement cycle.")); break;
          default:;
        }
    }
  }
  return devErrorsText;
}
// set qstrings with errors descriptions for DB (16 char length max)
QString TDevErrors::readDevTextErrorListDB(void)
{
  QString devError;
  if(bitError.any()){
    for(uint i=0;i<bitError.size();i++){
      if(bitError.test(i))
        switch(i)
        {
          case FPGA_LOAD_ERROR:     devError.append(tr("FPGA load error.")); break;
          case GPIO_ERROR:          devError.append(tr("GPIO/SPI error.")); break;
          case RW_DATA_ERROR:       devError.append(tr("R/W file error.")); break;
          case INFILE_ERROR:        devError.append(tr("Ini file absent.")); break;
          case SYNCHR_B2_ERROR:
          case SYNCHR_KC1_ERROR:
          case SYNCHR_RES_ERROR:    devError.append(tr("B2|KC1|Res abs.")); break;
          case SETLOCALE_ERROR:     devError.append(tr("System in LOCAL.")); break;
          case LOW_INTENSITY_ERROR: devError.append(tr("Low intensity.")); break;
          default:;
        }
    }
  }
  return devError;
}
