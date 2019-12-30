/*
 * SimpleGPIO.h
 *
 */

#ifndef HW_H
#define HW_H

#include "QtCore"
#include "SimpleGPIO.h"
#include "spi.h"


#define PIN_HOST_RST 60 // host_rst P2_12 GPIO1_28 (1*32+28=60)
#define PIN_HOST_DAC_PD 49 // host_dac_pd P2_23 GPIO1_17
#define PIN_HOST_DAC_CLR 115 // host_dac_clr P2_27 GPIO3_19
#define PIN_WDO_H 89 //  WDO_H P1_30 GPIO2_25

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
    THw();
    ~THw();
    void m_sleep(qint64 msec)
    {
        QEventLoop loop;
        QTimer::singleShot(msec, &loop, SLOT(quit()));
        loop.exec();
    }
    int pins_export(void);
    int pins_unexport(void);
    int pins_set_dir(void);

    void rst_FPGA(void);
    int getWDO(void);
    int SPI_getHW(void);
};



#endif /* HW_H */
