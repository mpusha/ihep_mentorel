#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QScriptEngine>
#include <QScriptValue>

#include "data_plot.h"

namespace Ui {
class MainWindow;
}
double funcSaw(double A,double fi);
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readSettings(void);
    void writeSettings(void);
    TDataPlot *plot1,*plot2;

private slots:
    void on_pbWrite_pressed();

    void on_pushButton_pressed();

    void on_cbType_currentIndexChanged(int index);

    void on_cbGraphView_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    double T0,FrqPS1,FrqPS2,ImaxPS1,ImaxPS2,tSample,tRetToZero,ISHBPS1,ISHEPS1,ISHBPS2,ISHEPS2;
    int nScan;
    QString outFileName;
    QString scr1,scr2;
    QVector <double> xGr,yGr1Odd,yGr1Even,yGr2Odd,yGr2Even; // for plot graph
};

#endif // MAINWINDOW_H
