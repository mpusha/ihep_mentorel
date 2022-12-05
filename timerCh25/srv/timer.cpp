#include "timer.h"
void msleep(uint64_t ms)
{
  uint64_t t1;
  t1=QDateTime::currentMSecsSinceEpoch();
  while(QDateTime::currentMSecsSinceEpoch()-t1<ms);
}
TTimer::TTimer(QString devName,QString spd): TSerial()
{
  bool ok;
  device=devName;
  speed=spd.toInt(&ok);
  if(!ok) speed=9600;
  qDebug()<<"Constructor of object TTimer start";
  strErrors<<"None"<<"Input buffer overload or string without 0"<<"Input data in scanf function error"<<"Input data error"<<
              "write data dont't correspond off read data"<<"FPGA error. May be don't configured"<<" "<<" "<< // 7 Atmega162 errors
             "UART controller error"<<"UART transmitter"<<"Read timeout"<<"Incorrect input data count"<< //11
             "Incorrect addres device read"<<"Read data not correct"<<"Validate data error"<< //14
             "Setup address not valid"<<"Setup channel not valid"<<"Setup data not valid"<< //17
             "Can't open file on write"<<"Can't open file on read"<< //19
             "Absent setup.xml file"<<"Error of file with settings";//21
  canOperate=false;
  qDebug()<<"Constructor of object TTimer finish";
}

TTimer::~TTimer()
{
  qDebug()<<"Destructor of object TTimer start";
  qDebug()<<"Destructor of object TTimer finish";
}
int TTimer::initHW(void)
{
  int timerError=ERR_NONE;
  if(!UART_Init(device,speed)){
    timerError=ERR_UART_ABS;
    canOperate=false;
  }
  return timerError;
}
int TTimer::writeFile(char* filename)
{
  int timerError=ERR_NONE;
  QFile file(filename);
  int ch,t,w;
  bool ok;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return(ERR_FILE_READ);

  QTextStream in(&file);
  QStringList lst;
  while (!in.atEnd()) {
    lst=in.readLine().simplified().split(' ');
    ch=lst.at(0).toInt(&ok); if(!ok) ch=0;
    t=lst.at(1).toInt(&ok); if(!ok) t=0;
    w=lst.at(2).toInt(&ok); if(!ok) w=0;
//    printf("%d %d %d\n",ch,t,w);
    timerError=writeRegTime(ch,t); if(timerError) { file.close(); return timerError;}
    timerError=writeRegWidth(ch,w); if(timerError){ file.close(); return timerError;}
  }
  file.close();
  return timerError;
}

int TTimer::readFile(char* filename)
{
  int timerError=0;
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return(ERR_FILE_WRITE);
  QTextStream out(&file);
  int t,w;
  for(int ch=1;ch<17;ch++){
    timerError=readRegTime(ch,&t); if(timerError) return timerError;
    timerError=readRegWidth(ch,&w); if(timerError) return timerError;
    out << ch<<' ' << t<<' ' << w<<"\n";
  }
  return timerError;
}

int TTimer::setAddress(int addr)
{
  int timerError=ERR_NONE;
  if((addr>=0)&&(addr<=99)) {
    address=addr; canOperate=true;
  }
  else {
    canOperate=false;
    timerError=ERR_SETUP_ADDR;
  }
  return timerError;
}

QString TTimer::sendReadRawCmd(QString cmd)
{
  if(!canOperate) return getQStrErr(ERR_SETUP_ADDR);
  QString sendStr=QString("%1:%2").arg(address,2,10,QChar('0')).arg((cmd));

  QString retstr;
  int ret;
  ret=sendCmd(sendStr);
  if(ret!=ERR_NONE) { UART_Flush(); return "Send UART error";}
  ret=readAnswer(retstr);
  if(ret!=ERR_NONE) { UART_Flush(); retstr="Timeout of receiver";}
  return retstr;
}

int TTimer::writeRegTime(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINDATA)||(data>MAXDATA)) return ERR_SETUP_DATA;
  ret=writeReadData("ST",ch,data,0,"");
  return ret;
}

int TTimer::writeRegWidth(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINWIDTH)||(data>MAXWIDTH)) return ERR_SETUP_DATA;
  ret=writeReadData("SW",ch,data,0,"");
  return ret;
}

int TTimer::writeRegMap(int data)
{
  int ret=0;
  data&=0xffff;
  ret=writeReadData("SE",0,data,0,"");
  return ret;
}

int TTimer::writeRegTimeValidate(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINDATA)||(data>MAXDATA)) return ERR_SETUP_DATA;
  ret=writeReadData("ST",ch,data,1,"GT");
  return ret;
}

int TTimer::writeRegWidthValidate(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINWIDTH)||(data>MAXWIDTH)) return ERR_SETUP_DATA;
  ret=writeReadData("SW",ch,data,1,"GW");
  return ret;
}

int TTimer::writeRegMapValidate(int data)
{
  int ret=0;
  data&=0xffff;
  ret=writeReadData("SE",0,data,1,"GE");
  return ret;
}

int TTimer::writeHWTime(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINDATA)||(data>MAXDATA)) return ERR_SETUP_DATA;
  ret=writeReadData("WT",ch,data,0,"");
  return ret;
}

int TTimer::writeHWWidth(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINWIDTH)||(data>MAXWIDTH)) return ERR_SETUP_DATA;
  ret=writeReadData("WW",ch,data,0,"");
  return ret;
}

int TTimer::writeHWMap(int data)
{
  int ret=0;
  data&=0xffff;
  ret=writeReadData("WE",0,data,0,"");
  return ret;
}

int TTimer::writeHWTimeValidate(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINDATA)||(data>MAXDATA)) return ERR_SETUP_DATA;
  ret=writeReadData("WT",ch,data,1,"RT");
  return ret;
}

int TTimer::writeHWWidthValidate(int ch, int data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  if((data<MINWIDTH)||(data>MAXWIDTH)) return ERR_SETUP_DATA;
  ret=writeReadData("WW",ch,data,1,"RW");
  return ret;
}

int TTimer::writeHWMapValidate(int data)
{
  int ret=0;
  data&=0xffff;
  ret=writeReadData("WE",0,data,1,"WE");
  return ret;
}

// Read functions

int TTimer::readRegTime(int ch,int* data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  ret=readData("GT",ch,data);
  return ret;
}
int TTimer::readRegWidth(int ch,int* data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  ret=readData("GW",ch,data);
  return ret;
}
int TTimer::readRegMap(int* data)
{
  int ret=0;
  ret=readData("GE",0,data);
  return ret;
}
int TTimer::readHWTime(int ch,int* data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  ret=readData("RT",ch,data);
  return ret;
}
int TTimer::readHWWidth(int ch,int* data)
{
  int ret=0;
  if((ch<1)||(ch>16)) return ERR_SETUP_CH;
  ret=readData("RW",ch,data);
  return ret;
}
int TTimer::readHWMap(int* data)
{
  int ret=0;
  ret=readData("RE",0,data);
  return ret;
}

//команда записи всех данных в HW
int TTimer::wrHW(void)
{
  int ret=0;
  ret=writeReadData("WD",0,0,0,"");
  return ret;
}

//широковещательная команда записи всех данных в HW
int TTimer::wideBandWrHW(void)
{
  int ret=0;
  int addr=address;
  address=0;
  ret=writeReadData("WD",0,0,0,"");
  address=addr;
  return ret;
}


// private function
int TTimer::readData(QString cmd,int ch,int *readData)
{
  if(!canOperate) return ERR_SETUP_ADDR;
  QString sendStr;
  int ret=0;
  QString answer;
  QStringList rdata;
  bool ok;
// для широковещательного сообщения
  if(address==0){
    sendStr=QString("00:%1").arg(cmd);
    for(int i=0;i<REP;i++){
      ret=sendCmd(sendStr);
      if(ret!=ERR_NONE) { UART_Flush(); continue; }
      break;
    }
    return ret;
  }

  if(ch==0) sendStr=QString("%1:%1").arg(address,2,10,QChar('0')).arg(cmd);
  else
    sendStr=QString("%1:%2 %3").arg(address,2,10,QChar('0')).arg(cmd).arg(ch);
  for(int i=0;i<REP;i++){
    usleep(RS_DELAY);
    ret=sendCmd(sendStr);
    if(ret!=ERR_NONE) { UART_Flush(); continue; }
    ret=readAnswer(answer);
    if(ret!=ERR_NONE) { UART_Flush(); continue; }
    rdata.clear();
    rdata=answer.simplified().split(':');
    if(rdata.count()<2) {ret=ERR_IDATA_CNT; continue; } // no all data
    int addr=rdata.at(0).toInt(&ok);
    if(!ok) {ret=ERR_IDATA_ADDR; continue; }
    if(addr!=address) {ret=ERR_IDATA_ADDR; continue; } // address none correct
    int codret=rdata.at(1).toInt(&ok);
    if(!ok) {ret=ERR_IDATA_CRET; continue; }
    *readData=codret;
    if((ch!=0)&&(codret<8)) { ret=codret; continue; } //uc Atmega162 error
    break;
  }
  return ret;
}

int TTimer::writeReadData(QString cmd,int ch, int data, bool validate,QString cmdValidate)
{
  if(!canOperate) return ERR_SETUP_ADDR;
  QString sendStr,sendStrV;
  int ret=0;
// для широковещательного сообщения
  if(address==0){
    sendStr=QString("00:%1").arg(cmd);
    for(int i=0;i<REP;i++){
      ret=sendCmd(sendStr);
      if(ret!=ERR_NONE) { UART_Flush(); continue; }
      break;
    }
    return ret;
  }

  if(ch==0) sendStr=QString("%1:%2 %3").arg(address,2,10,QChar('0')).arg(cmd).arg(data);
  else
    sendStr=QString("%1:%2 %3 %4").arg(address,2,10,QChar('0')).arg(cmd).arg(ch).arg(data);

  sendStrV=QString("%1:%2 %3").arg(address,2,10,QChar('0')).arg(cmdValidate).arg(ch);
  QString answer;
  QStringList rdata;
  bool ok;
  for(int i=0;i<REP;i++){
    usleep(RS_DELAY);
    ret=sendCmd(sendStr);
    if(ret!=ERR_NONE) { UART_Flush(); continue; }
    ret=readAnswer(answer);
    if(ret!=ERR_NONE) { UART_Flush(); continue; }
    rdata.clear();
    rdata=answer.split(':');
    if(rdata.count()<2) {ret=ERR_IDATA_CNT; continue; } // no all data
    int addr=rdata.at(0).toInt(&ok);
    if(!ok) {ret=ERR_IDATA_ADDR; continue; }
    if(addr!=address) {ret=ERR_IDATA_ADDR; continue; } // address none correct
    int codret=rdata.at(1).toInt(&ok);
    if(!ok) {ret=ERR_IDATA_CRET; continue; }
    if(codret) { ret=codret; continue; } //uc Atmega162 error
    usleep(RS_DELAY);
    if(validate) {  // validate data after read for request
      ret=sendCmd(sendStrV);
      if(ret!=ERR_NONE) { UART_Flush(); continue; }
      ret=readAnswer(answer);
      if(ret!=ERR_NONE) { UART_Flush(); continue; }
      rdata.clear();
      rdata=answer.split(':');
      if(rdata.count()<2) {ret=ERR_IDATA_CNT; continue; } // no all data
      int addr=rdata.at(0).toInt(&ok);
      if(!ok) {ret=ERR_IDATA_ADDR; continue; }
      if(addr!=address) {ret=ERR_IDATA_ADDR; continue; } // address none correct
      int codret=rdata.at(1).toInt(&ok);
      if(!ok) {ret=ERR_IDATA_CRET; continue; }
      if(codret<8) { ret=codret; continue; } //uc Atmega162 error
      if(codret!=data) { ret=ERR_IDATA_DATA; continue; }
    }
    break;
  }
  return ret;
}

int TTimer::sendCmd(QString cmd)
{
  if(!UART_Send(cmd.toAscii().data(), cmd.size()+1)) return ERR_UART_TRANS ;
  return ERR_NONE;
}

int TTimer::readAnswer(QString& answer)
{
  char bf=0;
  int d,i=0;
  answer.clear();
  while(1){
    d=UART_Read(&bf,1,UART_TIMEOUT);
    if(d<1) return ERR_UART_TOUT;
    if(!bf) break;
    answer.append(bf);
    i++;
    if(i>=32) { UART_Flush(); break; }
  }
  return ERR_NONE;
}

// return 1 if controller alive
int TTimer::testAlive(void)
{
  int codret=0;
  char bf=0;
  int d,i=0;
  if(!canOperate) return ERR_SETUP_ADDR;
  QString cmd=QString("%1:%2").arg(address,2,10,QChar('0')).arg("AL");
  QString answer;
  answer.clear();
  if(!UART_Send(cmd.toAscii().data(), cmd.size()+1)) return ERR_UART_TRANS;
  while(1){
    d=UART_Read(&bf,1,UART_SHORT_TOUT);
    if(d<1) return ERR_UART_TOUT;
    if(!bf) break;
    answer.append(bf);
    i++;
    if(i>=32) { UART_Flush(); break; }
  }
  QStringList rdata;
  bool ok;
  rdata.clear();
  rdata=answer.simplified().split(':');
  if(rdata.count()<2) return ERR_IDATA_CNT;  // no all data
  int addr_c=rdata.at(0).toInt(&ok);
  if(!ok) return ERR_IDATA_ADDR;
  if(addr_c!=address) return ERR_IDATA_ADDR;  // address none correct
  codret=rdata.at(1).toInt(&ok);
  if(!ok) return ERR_IDATA_DATA;
  return ERR_NONE;
}
