/*
 * hw.h
 *
 */

#ifndef HW_H
#define HW_H

#include <QtCore>
#include <QMutex>
#include <sys/epoll.h>

#include "SimpleGPIO.h"
#include "spi.h"
#include "fpga.h"

//typedef QVector <float> floatA;

typedef float floatA;

const int32_t commInMCycle=2; // all count of commutation Com1/Com2  in one measure cycle
const int32_t timePoints=16;  // all timepoints
const int32_t allADCs=2;      // all ADCs
const int32_t adcCh=16;       // max number Ch in ADC
const int32_t workAdcCh=15;   // work channels of ADCs for meas orbit
const int32_t allAxis=60;     // all count of axis on the ring

#define PIN_HOST_RST 60      // host_rst P2_12 GPIO1_28 (1*32+28=60)
#define PIN_HOST_START 49    // host_start P2_23 GPIO1_17
#define PIN_HOST_STOP 115    // host_stop P2_27 GPIO3_19
#define PIN_LED_SYSTEM 117   // Led system activity P2_25 GPIO3_21
#define PIN_WDO   89         // P1_30 GPIO2_25

enum PIN_FUNCTION{
    RST_LOW=0,
    RST_HIGH=1
};

/****************************************************************
*  HW function
****************************************************************/
class THw : public QThread
{
  Q_OBJECT
  public:
    static const u_int32_t fifo_size=512;
    //const u_int32_t size;
    THw();
    ~THw();
    enum SPI_REGS{        
      R_FW_REG     = 0x000000,
      R_MODE_REG   = 0x100000,
      R_TIME_REG   = 0x200000,
      R_DMEM_REG   = 0x300000,
      R_ADDR_REG   = 0x400000,
      R_STATUS_REG = 0x500000,
      R_ALLWS_REG  = 0x600000,

      W_WEN_REG    = 0x800000,
      W_MODE_REG   = 0x900000,
      W_TIME_REG   = 0xa00000,
      W_DMEM_REG   = 0xb00000,
      W_ADDR_REG   = 0xc00000
    };

   enum SPI_OPER{
      ENABLE_WRITE  = 0x0000e531,
      DISABLE_WRITE = 0x0000e532
   };

    void m_sleep(qint64 msec)
    {
       QEventLoop loop;
       QTimer::singleShot(msec, &loop, SLOT(quit()));
       loop.exec();

    }
    int usr_initalBBHW(void);
    void usr_rst(void);
    void usr_hostStartPulse();
    void usr_hostStartOn(void);
    void usr_hostStartOff(void);
    void usr_hostStopPulse();
    void usr_hostStopOn(void);
    void usr_hostStopOff(void);
    void usr_sysLedOff(void);
    void usr_sysLedOn(void);
    int  usr_getWDO(void);

    u_int16_t usr_getHW(void);
    u_int32_t usr_rdMode(void);
    u_int32_t usr_rdTimerVal( u_int32_t index);
    void usr_rdMem(u_int32_t *data,u_int32_t len);
    u_int32_t usr_rdStatus(void);
    u_int32_t usr_rdAllWords(void);
    u_int32_t usr_rdAddress(u_int32_t addr);

    void usr_wrMode(u_int32_t mode);
    void usr_wrTimerVal(u_int32_t val, u_int32_t index);
    void usr_wrMem(u_int32_t *data, u_int32_t len);
    void usr_wrAddress(u_int32_t addr);


    void usr_rdADCData(floatA dfAdc1[][adcCh], floatA dfAdc2[][adcCh], int allPoints);


    void usr_waitCanRead(void);


    int usr_configureFPGA(char * fName);
    bool usr_gpioIsOpen(void) { return (gpioInit) ; }
    int usr_closeBBHW(void);
    int usr_pollingGPIOs(int);

protected:
    QMutex mutex;

  private:
    int fd_spi;
    int pins_export(void);
    int pins_unexport(void);
    int pins_set_dir(void);
    void SPI_writeReg(u_int32_t data, u_int32_t reg);
    u_int32_t SPI_readReg(u_int32_t reg);
    void SPI_writeReadData(u_int32_t *dataw, u_int32_t *datar,u_int32_t len,u_int32_t reg);
    bool gpioInit;
    int fd_d0;
    int epfd;
    struct epoll_event ev_d0;
    struct epoll_event events[10];
// ADC data
    void readSettings(QString fname);
    int16_t zeroShAdc1[16], zeroShAdc2[16];
    floatA kADC1[16], kADC2[16];
};



#endif /* HW_H */
