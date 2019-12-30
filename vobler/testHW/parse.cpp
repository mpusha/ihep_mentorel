#include "parse.h"


TParse::TParse() : QObject()
{
  dev=new THw(32767);
}
TParse::~TParse()
{
  if(dev) delete dev;
}

bool TParse::parseFile(QString fname)
{

  QFile parseFile(fname);
  qDebug()<<"Begin processing script file"<<fname;
  if(!parseFile.open(QIODevice::ReadOnly)) {
    qDebug()<<tr("Error opening script file").arg (parseFile.fileName ());
    return false;
  }
  QTextStream stream (&parseFile);
  stream.setCodec(QTextCodec::codecForName("UTF-8"));
  QTextStream out(&parseFile);
  QString szLine,szLine1;

  bool statusScript;
  do {
    do{
      szLine = out.readLine().toLower();
    } while(szLine[0]=='#');
    if(!szLine.isNull()) qDebug()<<"Try execute command"<<szLine.simplified();
    statusScript=true;
    szLine1=szLine.section(':',0,0);
    if(szLine1=="initial"){
      if(dev->usr_initalBBHW()){
        qDebug()<<"Error initial board";
        break;
      }
    }
    else if(szLine1=="reset"){
      dev->usr_rst();
    }
    else if(szLine1=="powerdownon"){
      dev->usr_pdOn();
    }
    else if(szLine1=="powerdownoff"){
      dev->usr_pdOff();
    }
    else if(szLine1=="clearon"){
      dev->usr_clrOn();
    }
    else if(szLine1=="clearoff"){
      dev->usr_clrOff();
    }
    else if(szLine1=="memtest"){
      dev->usr_DacMemTest();
    }
    else if(szLine1=="writedac"){
      int data=szLine.section(':',1,1).simplified().toInt();
      int size=szLine.section(':',2,2).simplified().toInt();
      int sample=szLine.section(':',3,3).simplified().toInt();
      int mode=szLine.section(':',4,4).simplified().toInt();
      if(mode==0){
        dev->usr_WrDACConst(data);
      }
      else if(mode==1){ //write SAW Wf
        dev->usr_WrDACSaw();
      }
      else if(mode==2){
        dev->usr_WrDACRnd();
      }
      else
        dev->usr_DacMemTest(0); // infinite write for test SPI speed
      dev->usr_WrStrobe(size);dev->usr_WrSample(sample);
      qDebug()<<"count points"<<size<<"sample"<<sample;
    }
    else if(szLine1=="readadc"){
      dev->usr_RdAdcMem(szLine.section(':',1,1).simplified().toInt());
    }
    else if(szLine1=="readregs"){
      dev->usr_RdRegs(szLine.section(':',1,1).simplified().toInt());
    }
    else if(szLine1=="readdac"){

      dev->usr_RdDacMem(szLine.section(':',1,1).simplified().toInt());
    }
    else if(szLine1=="waitstart"){

    }
    else if(szLine1=="waitcanrdwr"){
      dev->usr_waitCanRead();
    }
    else if(szLine1=="wait"){
      dev->m_sleep(szLine.section(':',1,1).simplified().toInt());
    }
    else if(szLine1=="waitforever"){
        qDebug()<<"fore";
      dev->usr_waitForever();
    }
    else if(szLine1=="writefileindac"){
      dev->usr_WrDACFile(szLine.section(':',1,1).simplified());
    }
    else if(szLine1=="readadctofile"){
      dev->usr_RdADCFile(szLine.section(':',1,1).simplified());
    }
    else if(szLine1=="writemode"){
      QString tmp=szLine.section(':',1,1).simplified();
      qDebug()<<"size"<<tmp.toFloat();
    }
    else if(szLine1=="readstatus"){
    }
    else{
      if(szLine.length())
        qDebug()<<"Unknown command!";
      statusScript=false;
    }
    if(statusScript) qDebug()<<"Execute command OK.";
;
  } while(!szLine.isNull());


  qDebug()<<"Finish";
  parseFile.close();
  return true;
}

