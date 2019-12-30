#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings=new QSettings("myapp.ini",QSettings::IniFormat);
    readSettings();
    QLabel graphLabel1,graphLabel2;//=new QLabel();
    plot1=new TDataPlot(2,&graphLabel1,"Current PS1");
    plot2=new TDataPlot(2,&graphLabel2,"Current PS2");
    ui->grLayout1->addWidget(plot1);
    ui->grLayout2->addWidget(plot2);
    plot1->setGridVisible(true);
    plot2->setGridVisible(true);
}

MainWindow::~MainWindow()
{
    delete plot1;
    delete plot2;
    delete settings;
    delete ui;
}



void MainWindow::on_pbWrite_pressed()
{
  writeSettings();



  QFile file(outFileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox msgBox;
    msgBox.setText("Can't open file "+outFileName+" on write. May be file lock another application.");
    msgBox.exec();
    return;
  }

  QTextStream out(&file);

  out << "t,y1e,y2e,y1o,y2o\n";

  double t=0,dt=tSample/1e6; //  0.000250;
  double y1even,y1odd,y2even,y2odd;
  double T00=T0/nScan;
  double deltaSh1=(ISHEPS1-ISHBPS1)/T0,deltaSh2=(ISHEPS2-ISHBPS2)/T0;
  bool oddeven=ui->oddEvenEqCB->isChecked();
  switch(ui->cbType->currentIndex()){
    case 0: // spiral
      for(int i=0;i<nScan;i++){
        int j=i+1;
        while(t<T00*j){
          if((i%2)==0){
            y1even= ImaxPS1*sqrt((t-T00*i)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;
            if(oddeven) y1odd=y1even; else y1odd=ImaxPS1*sqrt((t-T00*i)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;

            y2even= ImaxPS2*sqrt((t-T00*i)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
            if(oddeven) y2odd=y2even; else y2odd= ImaxPS2*sqrt((t-T00*i)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
          }
          else {          
            y1even= ImaxPS1*sqrt((T00*j-t)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;
            if(oddeven) y1odd=y1even; else y1odd=ImaxPS1*sqrt((T00*j-t)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;

            y2even= ImaxPS2*sqrt((T00*j-t)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
            if(oddeven) y2odd=y2even; else y2odd= ImaxPS2*sqrt((T00*j-t)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
          }
          out.setRealNumberPrecision(5);
          out<<fixed<<t<<',';
          out.setRealNumberPrecision(1);
          out<<y1even<<','<<y2even<<','<<y1odd<<','<<y2odd<<'\n';
          t+=dt;
       }
     }
     break;
     case 1: //saw
        while(t<T0){        
          y1even= funcSaw(ImaxPS1,2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1; y1odd=y1even;
          y2even= funcSaw(ImaxPS2,2*M_PI*FrqPS2*t)+ISHBPS2+t*deltaSh2; y2odd=y2even;
          out.setRealNumberPrecision(5);
          out<<fixed<<t<<',';
          out.setRealNumberPrecision(1);
          out<<y1even<<','<<y2even<<y1odd<<','<<y2odd<<'\n';
          t+=dt;
        }
      break;
      case 2:
        while(t<T0){
          y1even= ImaxPS1*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1; y1odd=y1even;
          y2even= ImaxPS2*sin(2*M_PI*FrqPS2*t)+ISHBPS2+t*deltaSh2; y2odd=y2even;
          out.setRealNumberPrecision(5);
          out<<fixed<<t<<',';
          out.setRealNumberPrecision(1);
          out<<y1even<<','<<y2even<<y1odd<<','<<y2odd<<'\n';
          t+=dt;
       }
       break;
       default:
         while(t<T0){
           y1even= ImaxPS1*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1; y1odd=y1even;
           y2even= ImaxPS2*sin(2*M_PI*FrqPS2*t)+ISHBPS2+t*deltaSh2; y2odd=y2even;
           out.setRealNumberPrecision(5);
           out<<fixed<<t<<',';
           out.setRealNumberPrecision(1);
           out<<y1even<<','<<y2even<<y1odd<<','<<y2odd<<'\n';
           t+=dt;
         }
     }

     if(ui->retToZeroCB->isChecked()){

       double toZeroPionts=tRetToZero*1000/tSample;     
       double dY1odd,dY1even,dY2odd,dY2even;

       dY1odd=y1odd/toZeroPionts; dY2odd=y2odd/toZeroPionts;
       dY1even=y1even/toZeroPionts; dY2even=y2even/toZeroPionts;

        for(int i=0;i<(int)toZeroPionts-1;i++){
         y1even-=dY1even;y2even-=dY2even;
         y1odd-=dY1odd;y2odd-=dY2odd;
         out.setRealNumberPrecision(5);
         out<<fixed<<t<<',';
         out.setRealNumberPrecision(1);
         out<<y1even<<','<<y2even<<','<<y1odd<<','<<y2odd<<'\n';
         t+=dt;
       }
       out.setRealNumberPrecision(5);
       out<<fixed<<t<<',';
       out.setRealNumberPrecision(1);
       out<<0.0<<','<<0.0<<','<<0.0<<','<<0.0<<'\n';
       t+=dt;
     }

     file.close();
     QMessageBox msgBox;
     msgBox.setText("File "+outFileName+" write successfully!");
     msgBox.exec();
     return;
}

void MainWindow::writeSettings(void)
{
    ImaxPS1=ui->sbIMaxPS1->value();
    ImaxPS2=ui->sbIMaxPS2->value();
    FrqPS1=ui->sbFrqPS1->value();
    FrqPS2=ui->sbFrqPS2->value();
    T0=ui->sbT0->value();
    outFileName=ui->outFName->text();
    //scr1=ui->lineEditPS1->text();
    //scr2=ui->lineEditPS2->text();
    tSample=ui->sampleTime->value();
    tRetToZero=ui->reToZeroTime->value();
    nScan=ui->sbCycleScan->value();
    ISHBPS1=ui->sbIShB_PS1->value();
    ISHEPS1=ui->sbIShE_PS1->value();
    ISHBPS2=ui->sbIShB_PS2->value();
    ISHEPS2=ui->sbIShE_PS2->value();

    settings->beginGroup("setup");
    settings->setValue("ImaxPS1", ImaxPS1);
    settings->setValue("ImaxPS2", ImaxPS2);
    settings->setValue("FrqPS1", FrqPS1);
    settings->setValue("FrqPS2", FrqPS2);
    settings->setValue("T0", T0);
    settings->setValue("fileName", outFileName);
    settings->setValue("scr1", scr1);
    settings->setValue("scr2", scr2);
    settings->setValue("tsample", tSample);
    settings->setValue("tretzero",tRetToZero);
    settings->setValue("nScan",nScan);
    settings->setValue("IShBPS1",ISHBPS1);
    settings->setValue("IShEPS1",ISHEPS1);
    settings->setValue("IShBPS2",ISHBPS2);
    settings->setValue("IShEPS2",ISHEPS2);

    settings->endGroup();
}

void MainWindow::readSettings()
{
    settings->beginGroup("setup");
    ImaxPS1=settings->value("ImaxPS1", 800).toDouble();
    ImaxPS2=settings->value("ImaxPS2", 800).toDouble();
    FrqPS1=settings->value("FrqPS1", 50).toDouble();
    FrqPS2=settings->value("FrqPS2", 50).toDouble();
    T0=settings->value("T0", 1.0).toDouble();
    outFileName=settings->value("fileName", "out.csv").toString();
    scr1=settings->value("scr1", "A*sqrt(t/T0)*sin(f*t)").toString();
    scr2=settings->value("scr2", "A*sqrt(t/T0)*cos(f*t)").toString();
    tSample=settings->value("tsample", 250).toDouble();
    tRetToZero=settings->value("tretzero", 10).toDouble();
    nScan=settings->value("nScan", 1).toInt();
    ISHBPS1=settings->value("IShBPS1",0).toDouble();
    ISHEPS1=settings->value("IShEPS1",0).toDouble();\
    ISHBPS2=settings->value("IShBPS2",0).toDouble();
    ISHEPS2=settings->value("IShEPS2",0).toDouble();

    settings->endGroup();
    ui->sbIMaxPS1->setValue(ImaxPS1);
    ui->sbIMaxPS2->setValue(ImaxPS2);
    ui->sbFrqPS1->setValue(FrqPS1);
    ui->sbFrqPS2->setValue(FrqPS2);
    ui->sbT0->setValue(T0);
    ui->outFName->setText(outFileName);
    //ui->lineEditPS1->setText(scr1);
    //ui->lineEditPS2->setText(scr2);
    ui->sampleTime->setValue(tSample);
    ui->reToZeroTime->setValue(tRetToZero);
    ui->sbCycleScan->setValue(nScan);
    ui->sbIShB_PS1->setValue(ISHBPS1);
    ui->sbIShE_PS1->setValue(ISHEPS1);
    ui->sbIShB_PS2->setValue(ISHBPS2);
    ui->sbIShE_PS2->setValue(ISHEPS2);
}

double funcSaw(double A,double fi)
{
  int nPi=fi/(M_PI*2);
  double normFi=fi-nPi*2*M_PI;
  double func=0;
  if((normFi>=0)&&(normFi<M_PI_2 )){
    func=2*A/M_PI*normFi;
  }
  else if((normFi>=M_PI_2)&&(normFi<3*M_PI/2)){
    func=2*A-2*A/M_PI*normFi;
  }
  else {
    func=-4*A+2*A/M_PI*normFi;
  }
  return(func);
}

void MainWindow::on_pushButton_pressed()
{
    writeSettings();
    xGr.clear();yGr1Even.clear();yGr2Even.clear(); yGr1Odd.clear();yGr2Odd.clear();
    int ng=0;// number of graph point
    double t=0,dt=tSample/1e6; //  0.000250;
    double y1even,y1odd,y2even,y2odd;
    double T00=T0/nScan;
    double deltaSh1=(ISHEPS1-ISHBPS1)/T0,deltaSh2=(ISHEPS2-ISHBPS2)/T0;
    bool oddeven=ui->oddEvenEqCB->isChecked();
    switch(ui->cbType->currentIndex()){
      case 0: // spiral
        for(int i=0;i<nScan;i++){
          int j=i+1;
          while(t<T00*j){
            if((i%2)==0){
              y1even= ImaxPS1*sqrt((t-T00*i)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;
              if(oddeven) y1odd=y1even; else y1odd=ImaxPS1*sqrt((t-T00*i)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;

              y2even= ImaxPS2*sqrt((t-T00*i)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
              if(oddeven) y2odd=y2even; else y2odd= ImaxPS2*sqrt((t-T00*i)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
            }
            else {
              y1even= ImaxPS1*sqrt((T00*j-t)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;
              if(oddeven) y1odd=y1even; else y1odd=ImaxPS1*sqrt((T00*j-t)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1;

              y2even= ImaxPS2*sqrt((T00*j-t)/T00)*cos(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
              if(oddeven) y2odd=y2even; else y2odd= ImaxPS2*sqrt((T00*j-t)/T00)*sin(2*M_PI*FrqPS1*t)+ISHBPS2+t*deltaSh2;
            }

          ng++;
          t+=dt;
          xGr.append(t*1000);yGr1Even.append(y1even); yGr1Odd.append(y1odd);yGr2Even.append(y2even); yGr2Odd.append(y2odd);
          }
        }
      break;
      case 1: //saw
        while(t<T0){
          y1even= funcSaw(ImaxPS1,2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1; y1odd=y1even;
          y2even= funcSaw(ImaxPS2,2*M_PI*FrqPS2*t)+ISHBPS2+t*deltaSh2; y2odd=y2even;

        ng++;
        t+=dt;
        xGr.append(t*1000);yGr1Even.append(y1even); yGr1Odd.append(y1odd);yGr2Even.append(y2even); yGr2Odd.append(y2even);
        }
      break;
      case 2:
        while(t<T0){
          y1even= ImaxPS1*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1; y1odd=y1even;
          y2even= ImaxPS2*sin(2*M_PI*FrqPS2*t)+ISHBPS2+t*deltaSh2; y2odd=y2even;

        ng++;
        t+=dt;
        xGr.append(t*1000); yGr1Even.append(y1even); yGr1Odd.append(y1odd);yGr2Even.append(y2even); yGr2Odd.append(y2even);
        }
      break;
      default:
        while(t<T0){
            y1even= ImaxPS1*sin(2*M_PI*FrqPS1*t)+ISHBPS1+t*deltaSh1; y1odd=y1even;
            y2even= ImaxPS2*sin(2*M_PI*FrqPS2*t)+ISHBPS2+t*deltaSh2; y2odd=y2even;

        ng++;
        t+=dt;
        xGr.append(t*1000); yGr1Even.append(y1even); yGr1Odd.append(y1odd);yGr2Even.append(y2even); yGr2Odd.append(y2even);
        }

    }

    if(ui->retToZeroCB->isChecked()){
      double dY1odd,dY1even,dY2odd,dY2even;
      double toZeroPionts=tRetToZero*1000/tSample;
      dY1odd=y1odd/toZeroPionts; dY2odd=y2odd/toZeroPionts;
      dY1even=y1even/toZeroPionts; dY2even=y2even/toZeroPionts;
      for(int i=0;i<(int)toZeroPionts;i++){
        y1even-=dY1even;y2even-=dY2even;
        y1odd-=dY1odd;y2odd-=dY2odd;
        ng++;
        t+=dt;
        xGr.append(t*1000); yGr1Even.append(y1even); yGr1Odd.append(y1odd);yGr2Even.append(y2even); yGr2Odd.append(y2odd);
      }
      t+=dt;
      ng++;
      xGr.append(t*1000); yGr1Even.append(0); yGr1Odd.append(0);yGr2Even.append(0); yGr2Odd.append(0);
    }


    switch(ui->cbGraphView->currentIndex()){
      case 0:
         plot1->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr1Even.data());
         plot1->setDataForGraph(1,ng,qRgb(255,0,0),xGr.data(),yGr1Even.data());
         plot2->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr2Even.data());
         plot2->setDataForGraph(1,ng,qRgb(255,0,0),xGr.data(),yGr2Even.data());
      break;
      case 1:
        plot1->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr1Odd.data());
        plot1->setDataForGraph(1,ng,qRgb(255,0,0),xGr.data(),yGr1Odd.data());
        plot2->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr2Odd.data());
        plot2->setDataForGraph(1,ng,qRgb(255,0,0),xGr.data(),yGr2Odd.data());
      break;
      case 2:
        plot1->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr1Even.data());
        plot1->setDataForGraph(1,ng,qRgb(0,0,255),xGr.data(),yGr1Odd.data());
        plot2->setDataForGraph(0,ng,qRgb(255,0,0),xGr.data(),yGr2Even.data());
        plot2->setDataForGraph(1,ng,qRgb(0,0,255),xGr.data(),yGr2Odd.data());
      break;

      default:
        plot1->setDataForGraph(0,ng,qRgb(10,10,10),xGr.data(),yGr1Odd.data());

    }

    plot1->setAxis();
    plot1->replotGraph();
    plot1->repaint();
    plot2->setAxis();
    plot2->replotGraph();
    plot2->repaint();

}

void MainWindow::on_cbType_currentIndexChanged(int index)
{
    if(index==0) { ui->oddEvenEqCB->setEnabled(true); } else  {ui->oddEvenEqCB->setEnabled(false); }
   /* if(index==1) {
      ui->sbFrqPS2->setEnabled(true);
    }
    else {
      ui->sbFrqPS2->setEnabled(false);
    }*/
}

void MainWindow::on_cbGraphView_currentIndexChanged(int index)
{
    on_pushButton_pressed();
}
