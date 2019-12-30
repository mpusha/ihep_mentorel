/*
 * SimpleGPIO.h
 *
 */

#ifndef HW_H
#define HW_H

#include "QtCore"
#include "SimpleGPIO.h"
#include "spi.h"


enum PIN_FUNCTION{
    RST_LOW=0,
    RST_HIGH=1
};


/****************************************************************
*  HW vobler function
****************************************************************/
class THw : public QObject
{

  public:
    THw();
    ~THw();
    int pins_export(void);
    int pins_unexport(void);
    int pins_set_dir(void);
};



#endif /* HW_H */
