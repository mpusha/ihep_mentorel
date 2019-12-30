
#include "orbBehav.h"

extern QWaitCondition conditions;

//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------



TOrbBehav::TOrbBehav() :THw()
{
  qDebug()<<"Start constructor of class TOrbBehav";
  workDir=QCoreApplication::applicationDirPath();
  abort=false;
  for(int i=0;i<ALLREQSTATES;i++) allStates[i]=IDLE;
  phase = INITIAL_STATE;
  timerAlarm=false;
  cntErrInitial=0;
  qDebug()<<"Constructor of class TOrbBehav was finished";
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TOrbBehav::~TOrbBehav()
{
  qDebug()<<"Start destructor of class TOrbBehav";
  setAbort(true);
  wait(2000);
  if(isRunning()) {
    terminate();
    wait(2000);
  }
  qDebug()<<"Destructor of class TOrbBehav was finished";
}
// qDebug operator owerwrite for print states in debug mode
QDebug operator <<(QDebug dbg, const CPhase &t)
{
  dbg.nospace() <<"\n---------------------------------\nSTATE=";
  switch(t){
  case IDLE: dbg.space() << "IDLE" ; break;
  case INITIAL_STATE: dbg.space() << "INITIAL_STATE" ; break;
  case DEVICE_ERROR_STATE: dbg.space() << "DEVICE_ERROR_STATE"; break;
  case GET_DATA_STATE: dbg.space() << "GETINFO_STATE" ; break;
  case GET_STATUS_STATE: dbg.space() << "GET_REQ_AND_STATUS_STATE" ; break;
  case SET_DATA_STATE: dbg.space() << "UPDATE_HW_STATE" ; break;
  case UPDATE_STATUS_STATE: dbg.space() << "UPDATE_STATUS_STATE" ; break;
  default:;
  }

  return dbg.nospace() ;//<< endl;;
}
void TOrbBehav::run()
{
  CPhase deb=IDLE;//for debug only

  QEventLoop *loop;
  loop=new QEventLoop();

  int status;
  //usr_configureFPGA((char*)"orbita_project.rbf");
  //usr_rst();
  //qDebug()<<"Load FPGA HW"<<hex<<usr_getHW()<<"request HW"<<APP_FPGAVERSION;
  //u_int32_t time[16]={100,300,600,700,1000,1300,1600,1900,2200,2500,2800,3100,3400,3700,4100,4400};
  //for(u_int32_t i=0;i<16;i++)  { usr_wrTimerVal(time[i],i); }
  //int mode=0x01;
  //usr_wrMode(mode);
  //usr_hostStopPulse();
  //usr_hostStartPulse();
  msleep(1000);
  qDebug()<<"Run cycle processing of object TOrbBehav";
  emit signalStatusMsg("Begin");

  while(!abort){
    loop->processEvents();
    int ev=usr_pollingGPIOs(10);
    //usr_sysLedOff();
    if(ev) {
      if(usr_gpioIsOpen()) {
        if((usr_getHW()&0xff00)!=(APP_FPGAVERSION&0xff00)) { // all version work
        qDebug()<<"Load/Reload FPGA";
        phase = INITIAL_STATE;
        qDebug()<<"Event WDO";
        msleep(1000);
        }
      }
    }
    mutex.lock();
    if(phase==IDLE) {
      for(int i=0;i<ALLREQSTATES;i++) { // read all statese request and run if state!= READY state. high priority has low index
        if(allStates[i]!=IDLE){
          phase=allStates[i];
          allStates[i]=IDLE;
          break;
        }
      }
    }

    mutex.unlock();
    if(deb!=phase) { qDebug()<<phase;deb=phase;}
    switch(phase) {
      case IDLE: {
        msleep(10);
        break;
      }//end case READY:
// initial device on begin work
      case INITIAL_STATE: { //
        devError.clearDevError(); // clear device error
        syslog(LOG_INFO,"Initialisation of device is begin.");
        qDebug()<<"Initialisation of device is begin.";
        initialDevice();

        stErr=devError.analyseError();
        if(stErr) {
          timerAlarm=false;
          syslog(LOG_ERR,"Initialisation of device error. Try again.");
          qDebug()<<"Initialisation of device error. Try again.";
          emit signalErrMsg("Init err");
          allStates[INITIAL_STATE]=INITIAL_STATE;
          msleep(1000); //wait some time and try init another
          phase=DEVICE_ERROR_STATE;
          cntErrInitial++;
          if(cntErrInitial>5) {
            syslog(LOG_ERR,"Critical error of initialisation device. Programm terminated.");
            qDebug()<<"Critical error of initialisation device. Programm terminated.";
            emit signalErrMsg("Critical");
            abort=true;
          }
        }
        else {
          cntErrInitial=0;
          allStates[INITIAL_STATE]=IDLE;
          phase = IDLE;  
          emit signalStatusMsg("Initial");
          syslog(LOG_INFO,"Initialisation of device was finished successfully.");
          syslog(LOG_INFO,"Hw version %d",usr_getHW());
          qDebug()<<"Initialisation of device was finished successfully.";
          qDebug()<<"Hw version"<<hex<<usr_getHW();
          timerAlarm=true;
        }
        break;
      }// end case INITIAL_STATE:
      case SET_DATA_STATE:{

        break;
      }// end case SET_DATA_STATE
// processing hardware errors in device
      case DEVICE_ERROR_STATE:{
        syslog(LOG_ERR,"Found device error!");
        qDebug()<<"Found device error!";
        QStringList err=devError.readDevTextErrorList();
        for(int i=0;i<err.size();i++){
           syslog(LOG_ERR,err.at(i).toAscii());
           qDebug()<<err.at(i);
        }
        if(usr_gpioIsOpen()) {
          qDebug()<<"Version"<<hex<<usr_getHW();
          syslog(LOG_ERR,"Version %d",usr_getHW());
        }
        phase = IDLE;//UPDATE_STATUS_STATE;
        break;
      }// end case DEVICE_ERROR_STATE:
      default:;
    } // end switch

    msleep(200);
    //status=usr_rdStatus()&0xff;
    //if(status&0x80)
   //   {
   //   printf("Status=%x %x\n",status,usr_rdStatus()&0xff) ;
   // }
    /*if((status&1)&&(canReadRise==false)){
     // rise front canRead
      printf("Rise front Status=%x \n",status) ;
      canReadRise=true;
      createAdcData(mode);
    }
    if((!(status&1))&&(canReadRise==true)){
     // fall front canRead
      canReadRise=false;
      printf("Fall front Status=%x \n",status) ;
    }*/
  }//end while
  qDebug()<<"Run() processing of object TOrbBehav was finished";
  //delete this;
}

void TOrbBehav::initialDevice(void)
{
  if(readCA(workDir+"/dat/")){
    devError.setDevError(TDevErrors::INFILE_ERROR);
    return ;
  }
  if(usr_initalBBHW()) {
    devError.setDevError(TDevErrors::GPIO_ERROR);
    return ;
  }
  usr_configureFPGA((char*)QString(workDir+"/rbf/orbita_project.rbf").toStdString().c_str());
  usr_rst(); //reset FPGA
}

void TOrbBehav::wrADCFile(QString fname)
{
  //int i=0;

  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;


  QTextStream out(&file);

  //double t=0;
  //double kTr=1;
  //double d1,d2;
  out<<"x,y1,y2"<<'\n';
  for(int i=0;i<10*2;i+=2){
   /* d1=(((rd[i])&0xfff)-shADC1)*kTr;
    d2=(((rd[i+1])&0xfff)-shADC2)*kTr;
    QString tmp=QString("%1,%2,%3").arg(t,0,'f',6).arg(d1,0,'f',0).arg(d2,0,'f',0);
    out<<tmp<<'\n'; t+=0.01;*/
  }

  file.close();
}
// read coefficient,amendment (statical and dynamical), absent axis
int TOrbBehav::readCA(QString workDirPath)
{
  int axis;
  bool ok;
  for(int i=0;i<allAxis;i++) {
    absAxisR[i]=false; absAxisZ[i]=false;
    asR[i]=0; asZ[i]=0; //statical
    adR[i]=0; adZ[i]=0; // dynamical
    cR[i]=1;  cZ[i]=0.5; // coefficients
  }
// absent axis R read
  QFile fileAbsR(workDirPath+"/absR.dat");
  if(fileAbsR.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inAbsR(&fileAbsR);
    while (!inAbsR.atEnd()) {
      axis=inAbsR.readLine().toInt(&ok);
      if(!ok) continue;
      if((axis>120)||(axis<=1)) continue; // illegal number of axis/ Must be in 2-120 range
      absAxisR[axis/2-1]=true;
    }
  }
  else {
    qDebug()<<"File absR.dat is absent.";
    syslog( LOG_INFO,"File absR.dat is absent.");
  }
// absent axis Z read
  QFile fileAbsZ(workDirPath+"/absZ.dat");
  if(fileAbsZ.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inAbsZ(&fileAbsZ);
    while (!inAbsZ.atEnd()) {
      axis=inAbsZ.readLine().toInt(&ok);
      if(!ok) continue;
      if((axis>120)||(axis<=1)) continue; // illegal number of axis/ Must be in 2-120 range
      absAxisZ[axis/2-1]=true;
    }
  }
  else {
    qDebug()<<"File absZ.dat is absent.";
    syslog( LOG_INFO,"File absZ.dat is absent.");
  }

  QString sAxis,saR,saZ,scR,scZ,sdaR,sdaZ;
  floatA tmp;
// Read dynamical amendment (add in minus sign!!!)
  QFile fileADR(workDirPath+"/adR.dat");
  if(fileADR.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inADR(&fileADR);
    while (!inADR.atEnd()) {
      inADR >>sAxis>>sdaR;
      axis=sAxis.toInt(&ok);
      if(!ok) continue;
      if((axis>120)||(axis<=1)) continue; // illegal number of axis/ Must be in 2-120 range
      tmp=sdaR.toFloat(&ok);
      if(!ok) tmp=0;
      adR[axis/2-1]=tmp;
    }
  }
  else {
    qDebug()<<"File adR.dat is absent.";
    syslog( LOG_INFO,"File adR.dat is absent.");
  }

  QFile fileADZ(workDirPath+"/adZ.dat");
  if(fileADZ.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream inADZ(&fileADZ);
    while (!inADZ.atEnd()) {
      inADZ >>sAxis>>sdaR;
      axis=sAxis.toInt(&ok);
      if(!ok) continue;
      tmp=sdaZ.toFloat(&ok);
      if(!ok) tmp=0;
      adZ[axis/2-1]=tmp;
    }
  }
  else {
    qDebug()<<"File adZ.dat is absent.";
    syslog( LOG_INFO,"File adZ.dat is absent.");
  }

// Read statical amendment and PU electrode coefficeents
  QFile fileAC(workDirPath+"/acRZ.dat");
  if(!fileAC.open(QIODevice::ReadOnly | QIODevice::Text)){
    qDebug()<<"File acRZ.dat is absent.";
    syslog( LOG_INFO,"File acRZ.dat is absent.");
    return 1;
  }
  QTextStream inAC(&fileAC);
  while (!inAC.atEnd()) {
    inAC >>sAxis>>saR>>saZ>>scR>>scZ;
    axis=sAxis.toInt(&ok);
    if(!ok) continue;
    if((axis>120)||(axis<=1)) continue; // illegal number of axis/ Must be in 2-120 range
    tmp=saR.toFloat(&ok);
    if(!ok) tmp=0;
    asR[axis/2-1]=tmp;

    tmp=saZ.toFloat(&ok);
    if(!ok) tmp=0;
    asZ[axis/2-1]=tmp;

    tmp=scR.toFloat(&ok);
    if(!ok) tmp=0;
    cR[axis/2-1]=tmp;

    tmp=scZ.toFloat(&ok);
    if(!ok) tmp=0;
    cZ[axis/2-1]=tmp;
  }
  return 0;
}

void TOrbBehav::createOrbData(void)
{


}
//-----------Create ADC data fro
void TOrbBehav::createAdcData(int mode)
{
    u_int8_t regime=(mode>>4)&0x03,dir=(mode>>2)&0x01;

    int measDir;
    if(regime==2) measDir=2; else measDir=1;
    int allPoints=timePoints*commInMCycle*measDir; // allpoints for 1 ADC


    usr_rdADCData(dfAdc1,dfAdc2,allPoints);

    int n,m,shift=workAdcCh*2;
    if((regime==0) || (regime==3)) {
// regime 0 or 3 normal measure
       n=0;
       for(int i=0;i<adcCh-1;i++) {
         for(int j=0;j<timePoints;j++) {
           if(dir==0) { //R direction
             OrbR[n][j]=dfAdc1[j][i];
             OrbR[n+1][j]=dfAdc1[j+timePoints][i];
             OrbR[n+shift][j]=dfAdc2[j][i];
             OrbR[n+1+shift][j]=dfAdc2[j+timePoints][i];
             OrbZ[n][j]=0;
             OrbZ[n+1][j]=0;
             OrbZ[n+shift][j]=0;
             OrbZ[n+1+shift][j]=0;
           }
           else {
             OrbZ[n][j]=dfAdc1[j][i];
             OrbZ[n+1][j]=dfAdc1[j+timePoints][i];
             OrbZ[n+shift][j]=dfAdc2[j][i];
             OrbZ[n+1+shift][j]=dfAdc2[j+timePoints][i];
             OrbR[n][j]=0;
             OrbR[n+1][j]=0;
             OrbR[n+shift][j]=0;
             OrbR[n+1+shift][j]=0;
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
             OrbZ[n][j]=0;
             OrbZ[n+1][j]=0;
             OrbZ[n+shift][j]=0;
             OrbZ[n+1+shift][j]=0;
           }
           else {
             OrbZ[n][j]=dfAdc1[m][i];
             OrbZ[n+1][j]=dfAdc1[m+1][i];
             OrbZ[n+shift][j]=dfAdc2[m][i];
             OrbZ[n+1+shift][j]=dfAdc2[m+1][i];
             OrbR[n][j]=0;
             OrbR[n+1][j]=0;
             OrbR[n+shift][j]=0;
             OrbR[n+1+shift][j]=0;
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
       Intens[0][j]=dfAdc2[j][workAdcCh];
       Intens[1][j]=dfAdc2[j+adcCh][workAdcCh];
     }
     qDebug()<<"R";
     for(int i=0;i<60;i++)  {
       printf(" %3d ",(i+1)*2); for(int k=0;k<16;k++) printf("%6.3lf ",OrbR[i][k]); printf("\n");
     }
     qDebug()<<"Z";
     for(int i=0;i<60;i++)  {
     //  printf(" %3d ",(i+1)*2); for(int k=0;k<16;k++) printf("%6.3lf ",OrbZ[i][k]); printf("\n");
     }
     qDebug()<<"I";
     for(int i=0;i<2;i++)  {
      // printf(" %3d ",i+1); for(int k=0;k<16;k++) printf("%6.3lf ",I[i][k]); printf("\n");
     }

}

