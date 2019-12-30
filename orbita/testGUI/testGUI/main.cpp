#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setCursor(QCursor(Qt::ArrowCursor));
    w.show();

    return a.exec();
}
