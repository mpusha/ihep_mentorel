#ifndef PARSE_H
#define PARSE_H

#include <QtCore>
#include "hw.h"

class TParse : public QObject
{
    Q_OBJECT

public:
    TParse();
    ~TParse();
    const u_int32_t size;
    bool parseFile(QString fname);
    void printAdcData(int count);
    void createOrbData(int mode);

private:
    THw *dev;
public slots:

private slots:

private:

};



#endif // PARSE_H

