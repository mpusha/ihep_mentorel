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


//typedef QVector <float> floatA;

#define PIN_LED_PWR 112  // P2_30 GPIO3_16 GPIO112 OUTPUT  MODE7 pulldown    - P7 POWER Led
#define PIN_BTN_PWR 113  // P2_28 GPIO3_17 GPIO113 INPUT   MODE7 pullup      - P3 POWER switch

extern "C" int api_watchdog_open(const char * watchdog_device);
extern "C" int api_watchdog_hwfeed(void);
extern "C" int api_watchdog_init(const char *pcDevice);
extern "C" int api_watchdog_gettimeout(int *timeout);
extern "C" int api_watchdog_gettimeleft(int *timeout);
extern "C" int api_watchdog_settimeout(int *timeout);

/****************************************************************
*  HW function
****************************************************************/
class THw : public QThread
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
    int usr_initalBBHW(void);

    void usr_pwrLedOff(void);
    void usr_pwrLedOn(void);
    int  usr_getBtnPwr(void);

    bool usr_gpioIsOpen(void) { return (gpioInit) ; }
    int usr_closeBBHW(void);
    int usr_pollingGPIOs(int);

protected:
    void run();
    QMutex mutex;

  private:
    int fd_spi;
    int pins_export(void);
    int pins_unexport(void);
    int pins_set_dir(void);
    bool abort,gpioInit;
    int fd_d0;
    int epfd;
    struct epoll_event ev_d0;
    struct epoll_event events[10];
};



#endif /* HW_H */
