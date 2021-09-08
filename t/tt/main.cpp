#include <QApplication>
#include "mainwindow.h"
#include <QDebug>

int main(int argc, char *argv[])
{
  qDebug()<<"Hello";
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  
  return a.exec();
}
