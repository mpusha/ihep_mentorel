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
    bool parseFile(QString fname);

private:
    THw *dev;
public slots:

private slots:

private:

};



#endif // PARSE_H

