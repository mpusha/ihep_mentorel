
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
{
  short int ret=::CODEERR::NONE;
  if(bitError.any()){
    for(uint i=0;i<bitError.size();i++){
      if(bitError.test(i))
        switch(i)
        {
          case FPGA_LOAD_ERROR: ret=::CODEERR::INITIALISE_ERROR;  break;
          case GPIO_ERROR:  ret=::CODEERR::BBB_ERROR; break;
          case RW_DATA_ERROR:  ret=::CODEERR::HW_ERROR; break;
          case INFILE_ERROR: ret=::CODEERR::INFILE_ERROR;break;
          case SYNCHR_ERROR: ret=::CODEERR::SYNCHRO_ERROR;break;
          case PS_HW_ERROR:  ret=::CODEERR::PS_ERROR; break;
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
          case FPGA_LOAD_ERROR: devErrorsText.append(tr("FPGA load error")); break;
          case GPIO_ERROR: devErrorsText.append(tr("Processor module GPIO/SPI error.")); break;
          case RW_DATA_ERROR: devErrorsText.append(tr("R/W operation error.")); break;
          case INFILE_ERROR: devErrorsText.append(tr("File with dataform dont't exist.")); break;
          case SYNCHR_ERROR: devErrorsText.append(tr("Synchr pulse don't present.")); break;
          case PS_HW_ERROR: devErrorsText.append(tr("PS return status of error.")); break;
          default:;
        }
    }
  }
  return devErrorsText;
}
