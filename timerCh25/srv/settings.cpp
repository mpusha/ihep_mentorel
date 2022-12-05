#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include "settings.h"

TSettings::TSettings()
{
  devName="/dev/tty"; // default UART device name
  speed="9600";
  wFileName="Settings1"; // default work file name
  numTimers=0;
  for(int i=0;i<MAX_NUM_TIMERS;i++) tdata[i].tenable=0; // all timers disable
}

int TSettings::loadSetup(QString fName)
{
  QFile xmlFile(fName);
  if(!xmlFile.exists()) return 1; // xml file exists error
  xmlFile.open(QIODevice::ReadOnly);
  QXmlStreamReader reader(&xmlFile);
  while(reader.readNextStartElement()){
    if(reader.name()=="settings"){
      while(reader.readNextStartElement()){
        if(reader.name()=="workfile"){
          wFileName=reader.readElementText();
        }
        else if(reader.name()=="uart_device"){
          devName=reader.readElementText();
        }
        else if(reader.name()=="speed"){
          speed=reader.readElementText();
        }
        else reader.skipCurrentElement();
      }
    }
    else
      return 1;
  }
  return 0;
}

int TSettings::load(QString fName)
{
  QFile xmlFile(fName);
  if(!xmlFile.exists()) return 1; // xml file exists error
  sF=fName;
  xmlFile.open(QIODevice::ReadOnly);
  QXmlStreamReader reader(&xmlFile);

  int t=0,i=0;
  bool ok;
  int tErr=0,chErr=0,parErr=0,xmlErr=0,error=0;
  int val=0;
  while(reader.readNextStartElement()){
    if(reader.name()=="root"){
      while(reader.readNextStartElement()){
        if(reader.name()=="timer"){
          i=0;
          while(reader.readNextStartElement()){
            if(reader.name()=="name"){
              tdata[t].name=reader.readElementText();
            }
            else if(reader.name()=="address"){
              tdata[t].address=reader.readElementText().toInt(&ok);
              if(!ok) {
                tErr=t+1; // error in input on timer t
                tdata[t].address=100;
              }
            }
            else if(reader.name()=="enable"){
              tdata[t].tenable=reader.readElementText().toInt(&ok);
              if(!ok) {
                tErr=t+1; // error in input on timer t
                tdata[t].tenable=0;
              }
            }
            else if(reader.name()=="channel"){
              while(reader.readNextStartElement()){
                if(reader.name()=="time"){
                  val=((int)(reader.readElementText().toDouble(&ok)*100))*10; if(!ok) { tErr=t+1; chErr=i+1; parErr=1;} // error in input time value on timer t
                  if(val<MIN_TIME) { val=MIN_TIME; tErr=t+1; chErr=i+1; parErr=1;} // error in input time range on timer t ch i
                  if(val>MAX_TIME) { val=MAX_TIME; tErr=t+1; chErr=i+1; parErr=1;} // error in input time range on timer t ch i
                  tdata[t].time[i]=val;
                }
                else if(reader.name()=="width"){
                  val=reader.readElementText().toInt(&ok); if(!ok) { tErr=t+1; chErr=i+1; parErr=2;} // error in input width value on timer t
                  if(val<MIN_TIME) { val=MIN_TIME; tErr=t+1; chErr=i+1; parErr=2;} // error in input width range on timer t ch i
                  if(val>MAX_TIME) { val=MAX_TIME; tErr=t+1; chErr=i+1; parErr=2;} // error in input width range on timer t ch i
                  tdata[t].width[i]=val;
                }
                else if(reader.name()=="enable"){
                  val=reader.readElementText().toInt(&ok); if(!ok) { tErr=t+1; chErr=i+1; parErr=3;} // error in input enable value on timer t
                  if(val<0) { val=0; tErr=t+1; chErr=i+1; parErr=3;} // error in input enable range on timer t ch i
                  if(val>1) { val=1; tErr=t+1; chErr=i+1; parErr=3;} // error in input enable range on timer t ch i
                  tdata[t].chenable[i]=val;
                }
                else reader.skipCurrentElement();
              }
              i++;
            }
            else reader.skipCurrentElement();
          }
          t++;
        }
      }
    }
    else
      xmlErr=1;
  }
  error=xmlErr;
//16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
// 0  0 ------TI------  ------CH-----  -PR-  MT MC XML
//XML 1 bit;  error in file
//MC  1 bit;  limit on maximal number of channels was increased. May be error in XML
//MT  1 bit;  limit on maximal number of timers was increased. May be error in XML
//PR  2 bits; number of parameters where was error: 0 - absent, 1 - time, 2 - width, 3- enable
//CH  5 bits; number of channel where was error
//TI  5 bits; number of timer where was error

  if(i>MAX_NUM_CH) { i=MAX_NUM_CH; error|=(1<<1); } // max channel limit increase
  if(t>MAX_NUM_TIMERS) { t=MAX_NUM_TIMERS; error|=(1<<2);} // max timers limit increase
  numTimers=t;
  error|=((parErr&0x3)<<3);
  error|=((chErr&0x1f)<<5);
  error|=((tErr&0x1f)<<10);
/*  qDebug()<<"Error in Sett"<<tErr;
  for(int j=0;j<t;j++){
    qDebug()<<"Timer"<<j<<"address"<<tdata[j].address<<"Name"<<tdata[t].name;
    for(int k=0;k<i;k++)
    qDebug()<<"   Time["<<k<<"]="<<tdata[j].time[k]<<"  Width["<<k<<"]="<<tdata[j].width[k]<<"  En["<<k<<"]="<<tdata[j].chenable[k];
  }
*/
  return error;
}
int TSettings::mkDatFile(QString fName,int t)
{
  int chen=0;
  QFile datFile(fName);
  datFile.open(QIODevice::WriteOnly | QIODevice::Text);
  QTextStream out(&datFile);
  for(int i=0;i<MAX_NUM_CH;i++) {
    out << i+1 <<" "<< tdata[t].time[i]<<" "<<tdata[t].width[i]<< "\n";
    chen|=((tdata[t].chenable[i]&1)<<i);
  }
  datFile.close();
  return chen;
}
