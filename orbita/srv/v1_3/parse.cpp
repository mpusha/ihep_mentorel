#include "parse.h"


TParse::TParse() : QObject(), size(4096)
{
  dev=new THw(32767);
  dev->start(QThread::NormalPriority);
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
    else if(szLine1=="starton"){
      dev->usr_hostStartOn();
    }
    else if(szLine1=="startoff"){
      dev->usr_hostStartOff();
    }
    else if(szLine1=="stopon"){
      dev->usr_hostStopOn();
    }
    else if(szLine1=="stopoff"){
      dev->usr_hostStopOff();
    }
    else if(szLine1=="readhw"){
      qDebug()<<"HW"<<dev->usr_getHW();
    }
    else if(szLine1=="testdevice"){
        qDebug()<<"START tests set of orbita device!";
        qDebug()<<"HW="<<hex<<dev->usr_getHW();
        qDebug()<<"Wait Reset pulse ";
        while(!(dev->usr_rdStatus()&1)) dev->m_sleep(10);
        qDebug()<<"Status="<<dev->usr_rdStatus();
        bool err=false;
        u_int32_t addr=0;
        dev->usr_wrMode(0x5a);
        if((0xff&dev->usr_rdMode())!=0x5a) err=true;
        if(err) qDebug()<<"Write mode register test wrong." <<dev->usr_rdMode(); else  qDebug()<<"Write mode register test OK." ;
        err=false;
        int tmp;
        for(u_int32_t i=0;i<16;i++) { dev->usr_wrTimerVal(i*100+i,i);}
        for(u_int32_t i=0;i<16;i++) { if((tmp=dev->usr_rdTimerVal(i)/10)!=i*100+i) { err=true; addr=i;}}
        //dev->usr_wrTimerVal(32000,15);
        if(err) qDebug()<<"Timer test wrong. addr" <<addr<<"Read val"<<tmp; else  qDebug()<<"Timer test OK." ;
        int sz=4096;
        QVector <u_int32_t> dw,cdw,dr;
        dw.resize(sz);cdw.resize(sz);dr.resize(sz);
        for(int i=0;i<sz;i++) cdw[i]=dw[i]=rand()%65535;
        dev->usr_wrAddress(0);
        dev->usr_wrMem(dw.data(),sz);
        dev->usr_wrAddress(0);
        dev->usr_rdMem(dr.data(),sz);
        err=false;
        addr=0;
        for(int i=1;i<sz;i++) {
            if(cdw[i]!=(dr[i]&0xffff)) {
                err=true;
                addr=i;
                 qDebug()<<i<<cdw[i]<<(dr[i]&0xffff);
                break;
            }
        }
        if(err) qDebug()<<"ADC memory test wrong."<<addr ; else  qDebug()<<"ADC memory  test OK." ;
//        dev->usr_rdADCBlockOnePoint(0);
 /*        dev->usr_wrTimerVal(10,0);   dev->usr_wrTimerVal(300,1);   dev->usr_wrTimerVal(600,2);  dev->usr_wrTimerVal(900,3);
         dev->usr_wrTimerVal(1200,4); dev->usr_wrTimerVal(1500,5); dev->usr_wrTimerVal(1800,6); dev->usr_wrTimerVal(2100,7);
         dev->usr_wrTimerVal(2400,8); dev->usr_wrTimerVal(2700,9); dev->usr_wrTimerVal(3000,10); dev->usr_wrTimerVal(3300,11);
         dev->usr_wrTimerVal(3600,12); dev->usr_wrTimerVal(3900,13); dev->usr_wrTimerVal(4200,14); dev->usr_wrTimerVal(4500,15);
         */

        /*dev->usr_wrTimerVal(10,0);   dev->usr_wrTimerVal(500,1);   dev->usr_wrTimerVal(1000,2);  dev->usr_wrTimerVal(1500,3);
        dev->usr_wrTimerVal(2000,4); dev->usr_wrTimerVal(2500,5); dev->usr_wrTimerVal(3000,6); dev->usr_wrTimerVal(3500,7);
        dev->usr_wrTimerVal(4000,8); dev->usr_wrTimerVal(4500,9); dev->usr_wrTimerVal(5000,10); dev->usr_wrTimerVal(5500,11);
        dev->usr_wrTimerVal(6000,12); dev->usr_wrTimerVal(6500,13); dev->usr_wrTimerVal(7000,14); dev->usr_wrTimerVal(4500,15);*/
        dw.clear();cdw.clear();dr.clear();
        dev->start(QThread::NormalPriority);
    }
    else if(szLine1=="measure"){
        u_int32_t time[16]={100,300,600,700,1000,1300,1600,1900,2200,2500,2800,3100,3400,3700,4100,4400};
        for(u_int32_t i=0;i<16;i++)  { dev->usr_wrTimerVal(time[i],i); }
        int mode=0x21;
        dev->usr_wrMode(mode);
        dev->usr_hostStopPulse();
        dev->usr_hostStartPulse();
        int status=0;
        bool canReadRise=true;

//while(1){ dev->m_sleep(100); printf("Rise front Status=%x \n",dev->usr_rdStatus()&0xff) ;}
        while(1){
          dev->m_sleep(10);
          status=dev->usr_rdStatus()&0xff;
          if((status&1)&&(canReadRise==false)){
 // rise front canRead
             printf("Rise front Status=%x \n",status) ;
             canReadRise=true;
             //printAdcData(0);
             createOrbData(mode);
          }
          if((!(status&1))&&(canReadRise==true)){
 // fall front canRead
           canReadRise=false;
           printf("Fall front Status=%x \n",status) ;
          }
        }

    }
    else if(szLine1=="readregs"){

    }
    else if(szLine1=="readdac"){


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
        qDebug()<<"forever";
        qDebug()<<"Ctrl-C for exit.";
        while(1) {
            dev->m_sleep(2000);
            dev->usr_sysLedOff();
            dev->m_sleep(2000);
            dev->usr_sysLedOn();
            qDebug()<<"Status"<<dev->usr_rdStatus();
         }
     }
    else if(szLine1=="writefileindac"){

    }
    else if(szLine1=="readadctofile"){
      dev->usr_rdADCFile(szLine.section(':',1,1).simplified());
    }
    else if(szLine1=="writemode"){
      QString tmp=szLine.section(':',1,1).simplified();
      qDebug()<<"size"<<tmp.toFloat();
    }
    else if(szLine1=="writefpga"){
      QString tmp=szLine.section(':',1,1).simplified();
      qDebug()<<"File name for load"<<tmp.toAscii();
      { qDebug()<<"Loading status"<<dev->configureFPGA(tmp.toLocal8Bit().data());}
    }
    else if(szLine1=="readstatus"){
    }
    else{
      if(szLine.length())
        qDebug()<<"Unknown command!";
      statusScript=false;
    }
    if(statusScript) qDebug()<<"Execute command OK.";

  } while(!szLine.isNull());


  qDebug()<<"Finish";
  parseFile.close();
  return true;
}

void TParse::printAdcData(int count)
{
  //  floatA chADC1,chADC2;
  //  int timePoints=16;
   // chADC1.resize(size);
    //chADC2.resize(size);
    //::std::vector <floatA >  dfAdc1(timePoints,chADC1),dfAdc2(timePoints,chADC2);
   // dfAdc1.resize(256); dfAdc2.resize(256);
   floatA chADC1,chADC2;
   int timePoints=16;
   chADC1.resize(16);
   chADC2.resize(16);
   QVector <floatA >  dfAdc1(timePoints*2,chADC1),dfAdc2(timePoints*2,chADC2);
  // dfAdc1.resize(256); dfAdc2.resize(256);

    dev->usr_rdADCBlockOnePoint(dfAdc1,dfAdc2,1);
    printf("ADC1\n");
    printf(" tP   CH1    CH2    CH3    CH4    CH5    CH6    CH7    CH8    CH9    CH10   CH11   CH12   CH13   CH14   CH15   I\n");
  //  for(int i=0;i<16;i++)  {
  //    printf(" %2d ",i); for(int k=0;k<32;k++) printf("%6.3lf ",dfAdc1[i][k]); printf("\n");
  //  }
     printf("\nADC2\n");
     printf(" tP   CH1    CH2    CH3    CH4    CH5    CH6    CH7    CH8    CH9    CH10   CH11   CH12   CH13   CH14   CH15   I\n");
    for(int i=0;i<32;i++)  {
       printf(" %2d ",i); for(int k=0;k<16;k++) printf("%6.3lf ",dfAdc2[i][k]); printf("\n");
    }
   printf("All write words= %4d\n",dev-> usr_rdAllWords());
    dfAdc1.clear(); dfAdc2.clear();
}

void TParse::createOrbData(int mode)
{
    u_int8_t regime=(mode>>4)&0x03,dir=(mode>>2)&0x01;
    qDebug()<<"reg/dir"<<regime<<dir;
    floatA chADC1,chADC2;
    int commInMCycle=2; // all count of commutation Com1/Com2  in one measure cycle
    int measDir; // all count of commutation  R/Z in one measure cycle
    if(regime==2) measDir=2; else measDir=1;
    int timePoints=16;
    int allADCs=2;
    int adcCh=16,workAdcCh=15;
    int allPoints=timePoints*commInMCycle*measDir; //    all mode size 512 for 1 ADC
    int allWords=adcCh*allPoints;
    qDebug()<<"allwp"<<allWords<<allPoints;
    chADC1.resize(adcCh);
    chADC2.resize(adcCh);

    QVector <floatA >  dfAdc1(allPoints,chADC1),dfAdc2(allPoints,chADC2);

    dev->usr_rdADCData(dfAdc1,dfAdc2,allPoints);

    floatA  orbOneTPointR, orbOneTPointZ;
    orbOneTPointR.resize(timePoints);
    orbOneTPointZ.resize(timePoints);

    QVector <floatA >  OrbR(commInMCycle*allADCs*workAdcCh,orbOneTPointR), OrbZ(commInMCycle*allADCs*workAdcCh,orbOneTPointZ);

    floatA IOnComm(timePoints); // intensivnost
    QVector <floatA > I(commInMCycle,IOnComm);

     qDebug()<<"alloc";
     int n,m,shift=workAdcCh*2;
     if((regime==0) || (regime==3)) {
// regime 0 or 3 normal measure
       n=0;
       for(int i=0;i<adcCh-1;i++) {
         for(int j=0;j<timePoints;j++) {//qDebug()<<"sds"<<i<<j<<n;
           if(dir==0) { //R direction
             OrbR[n][j]=dfAdc1[j][i];
             OrbR[n+1][j]=dfAdc1[j+timePoints][i];
             //  if((n==28)||(n==30)) qDebug()<<"ON62"<<n<<i<<j<<dfAdc2[j][i]<<dfAdc2[j+timePoints][i];
             OrbR[n+shift][j]=dfAdc2[j][i];
             OrbR[n+1+shift][j]=dfAdc2[j+timePoints][i];
           }
           else {
             OrbZ[n][j]=dfAdc1[j][i];
             OrbZ[n+1][j]=dfAdc1[j+timePoints][i];
             OrbZ[n+shift][j]=dfAdc2[j][i];
             OrbZ[n+1+shift][j]=dfAdc2[j+timePoints][i];
           }
         }
         n+=2;
       }
     }
// regime 1 fast measure
     if(regime==1) {
       n=0;
       for(int i=0;i<adcCh-1;i++) {
         m=0;
         for(int j=0;j<timePoints;j++) {
           if(dir==0) {
             OrbR[n][j]=dfAdc1[m][i];
             OrbR[n+1][j]=dfAdc1[m+1][i];
             OrbR[n+shift][j]=dfAdc2[m][i];
             OrbR[n+1+shift][j]=dfAdc2[m+1][i];
           }
           else {
             OrbZ[n][j]=dfAdc1[m][i];
             OrbZ[n+1][j]=dfAdc1[m+1][i];
             OrbZ[n+shift][j]=dfAdc2[m][i];
             OrbZ[n+1+shift][j]=dfAdc2[m+1][i];
           }
           m+=2;
         }
         n+=2;
       }
     }
// regime 2 ultra fast measure
     n=0;
     if(regime==2) {
       for(int i=0;i<adcCh-1;i++) {
         m=0;
         for(int j=0;j<timePoints;j++) {//qDebug()<<"sds"<<i<<j<<n;
           OrbR[n][j]=dfAdc1[m][i];
           OrbR[n+1][j]=dfAdc1[m+1][i];
           OrbR[n+shift][j]=dfAdc2[m][i];
           OrbR[n+1+shift][j]=dfAdc2[m+1][i];

           OrbZ[n][j]=dfAdc1[m+2][i];
           OrbZ[n+1][j]=dfAdc1[m+3][i];
           OrbZ[n+shift][j]=dfAdc2[m+2][i];
           OrbZ[n+1+shift][j]=dfAdc2[m+3][i];
           m+=4;
         }
         n+=2;
       }
     }
// intensivnost for all
     for(int j=0;j<timePoints;j++) {
       I[0][j]=dfAdc2[j][workAdcCh];
       I[1][j]=dfAdc2[j+adcCh][workAdcCh];
     }
     qDebug()<<"R";
     for(int i=0;i<60;i++)  {
       printf(" %3d ",(i+1)*2); for(int k=0;k<16;k++) printf("%6.3lf ",OrbR[i][k]); printf("\n");
     }
     qDebug()<<"Z";
     for(int i=0;i<60;i++)  {
       printf(" %3d ",(i+1)*2); for(int k=0;k<16;k++) printf("%6.3lf ",OrbZ[i][k]); printf("\n");
     }
     qDebug()<<"I";
     for(int i=0;i<2;i++)  {
       printf(" %3d ",i+1); for(int k=0;k<16;k++) printf("%6.3lf ",I[i][k]); printf("\n");
     }

     OrbR.clear();OrbZ.clear(); I.clear();
     orbOneTPointR.clear(); orbOneTPointZ.clear(); IOnComm.clear();
     dfAdc1.clear(); dfAdc2.clear();
     chADC1.clear(); chADC2.clear();
}
