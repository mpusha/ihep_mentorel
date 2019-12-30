#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

MainWindow::~MainWindow()
{
    delete ui;
}
