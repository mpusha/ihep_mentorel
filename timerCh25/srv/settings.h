#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#define MAX_NUM_TIMERS 10  // maximal number of timers
#define MAX_NUM_CH 16 // maximal number of channels
const int MIN_TIME=10,MAX_TIME=10000000,MIN_WIDTH=10,MAX_WIDTH=2000000;


struct STimerData {
  int time[16];
  int width[16];
  int chenable[16];
  int address;
  int tenable;
  QString name;
};

class TSettings
{
public:
    TSettings();
    int getAllTimers(void) {return numTimers;}
    int load(QString);
    int loadSetup(QString fName);
    struct STimerData tdata[MAX_NUM_TIMERS];
    QString getDevName(){return devName;}
    QString getSpeed(){return speed;}
    QString getWorkFileName(){return wFileName;}
    QString getfullWFileName(){return sF;}
    int mkDatFile(QString fName, int t);
private:
    int numTimers;
    QString wFileName,devName,speed;
    QString sF;
};

#endif // SETTINGS_H
