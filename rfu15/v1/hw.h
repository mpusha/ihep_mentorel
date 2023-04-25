/*
 * SimpleGPIO.h
 *
 */

#ifndef HW_H
#define HW_H
#include <syslog.h>
#include <QApplication>
#include <QtCore>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "SimpleGPIO.h"
#include "spi.h"


#define PIN_HOST_RST 60 // host_rst P2_12 GPIO1_28 (1*32+28=60)
#define PIN_HOST_DAC_PD 49 // host_dac_pd P2_23 GPIO1_17
#define PIN_HOST_WD 115 // host_dac_clr P2_27 GPIO3_19 use as host WD output for FPGA V1.4!!!
#define PIN_WDO_H 89 //  WDO_H P1_30 GPIO2_25
#define PIN_HOST_ALIVE 117 // output alive signal on LED P2_25 GPIO3_21


enum PIN_FUNCTION{
    RST_LOW=0,
    RST_HIGH=1
};


/****************************************************************
*  HW vobler function
****************************************************************/
class THw : public QObject
{
  Q_OBJECT
  public:
    static const u_int32_t fifo_size=512;
    const u_int32_t size;
    THw(int, double, int dac1Sh, int dac2Sh);
    ~THw();
    enum SPI_REGS{
      EN_SPI_WR_REG = 0x000000,
      WR_DACADC_DATA= 0x100000,
      W_DAC_DATA    = 0x200000,
      W_ADDR_REG    = 0x300000,
      W_MODE_REG    = 0x400000,
      W_STROBE_REG  = 0x500000,
      W_SAMPLE_REG  = 0x600000,
      R_ADCINC_DATA = 0x700000,
      R_STATUS_REG  = 0x800000,
      R_ADC_DATA    = 0x900000,
      R_DAC_DATA    = 0xa00000,
      R_ADDR_REG    = 0xb00000,
      R_MODE_REG    = 0xc00000,
      R_STROBE_REG  = 0xd00000,
      R_SAMPLE_REG  = 0xe00000,
      R_FW_REG      = 0xf00000
    };


    void m_sleep(qint64 msec)
    {
        QEventLoop loop;
        QTimer::singleShot(msec, &loop, SLOT(quit()));
        loop.exec();
    }

    int initalBBHW(void);

// Host GPIO function
    void usr_rst(void);
    void usr_pdOn(void);
    void usr_pdOff(void);
    void usr_hostWd(void);
    void usr_hostAliveOn(void);
    void usr_hostAliveOff(void);
    int usr_getWDO(void);
    int usr_getHW(void);

// Host SPI function
    int usr_WrDACFile(QString fname);
    int usr_RdADCFile(QString);
    void usr_writeMode(int);
    u_int32_t usr_RdStatus(void);
    void usr_waitCanRead(void);
    bool usr_waitCanWrite(void);
    void usr_waitStrobe(void);
    void usr_WrSample(u_int32_t);
    void usr_WrStrobe(u_int32_t) ;
    u_int32_t usr_RdSample(void);
    u_int32_t usr_RdStrobe(void) ;

  private:
    int adc1Shift,adc2Shift,dac1Shift,dac2Shift;
    double coeffAdc,coeffDac;
    int fd_spi;
 // host GPIO function
    int pins_export(void);
    int pins_unexport(void);
    int pins_set_dir(void);
// host SPI function
    void SPI_writeReg(u_int32_t data, u_int32_t reg);
    u_int32_t SPI_readReg(u_int32_t reg);
    void SPI_writeReadData(u_int32_t *dataw, u_int32_t *datar,u_int32_t len,u_int32_t reg);
    bool canWr;
public slots:
    void boosterCycleEnd();
};



#endif /* HW_H */
