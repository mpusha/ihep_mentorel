/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue 23. Oct 15:36:16 2018
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *grLayout1;
    QDoubleSpinBox *sbT0;
    QLabel *label_7;
    QCheckBox *retToZeroCB;
    QDoubleSpinBox *sampleTime;
    QLabel *label_6;
    QLabel *label_5;
    QLabel *label_4;
    QSpinBox *reToZeroTime;
    QSpinBox *sbFrqPS1;
    QPushButton *pbWrite;
    QPushButton *pushButton;
    QLabel *label_3;
    QLineEdit *outFName;
    QSpinBox *sbIMaxPS1;
    QComboBox *cbType;
    QLabel *label_8;
    QSpinBox *sbCycleScan;
    QLabel *label_9;
    QComboBox *cbGraphView;
    QLabel *label_10;
    QSpinBox *sbFrqPS2;
    QLabel *label_11;
    QSpinBox *sbIMaxPS2;
    QLabel *label_12;
    QSpinBox *sbIShB_PS1;
    QSpinBox *sbIShE_PS1;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QSpinBox *sbIShE_PS2;
    QLabel *label_16;
    QSpinBox *sbIShB_PS2;
    QLabel *label_17;
    QLabel *label_18;
    QCheckBox *oddEvenEqCB;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *grLayout2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(774, 738);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayoutWidget_2 = new QWidget(centralWidget);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(10, 220, 701, 211));
        grLayout1 = new QVBoxLayout(verticalLayoutWidget_2);
        grLayout1->setSpacing(6);
        grLayout1->setContentsMargins(11, 11, 11, 11);
        grLayout1->setObjectName(QString::fromUtf8("grLayout1"));
        grLayout1->setContentsMargins(0, 0, 0, 0);
        sbT0 = new QDoubleSpinBox(centralWidget);
        sbT0->setObjectName(QString::fromUtf8("sbT0"));
        sbT0->setGeometry(QRect(630, 30, 62, 22));
        sbT0->setMaximum(2);
        sbT0->setSingleStep(0.05);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(530, 10, 111, 20));
        retToZeroCB = new QCheckBox(centralWidget);
        retToZeroCB->setObjectName(QString::fromUtf8("retToZeroCB"));
        retToZeroCB->setGeometry(QRect(20, 190, 211, 17));
        sampleTime = new QDoubleSpinBox(centralWidget);
        sampleTime->setObjectName(QString::fromUtf8("sampleTime"));
        sampleTime->setGeometry(QRect(540, 30, 62, 22));
        sampleTime->setDecimals(0);
        sampleTime->setMinimum(50);
        sampleTime->setMaximum(500);
        sampleTime->setSingleStep(50);
        sampleTime->setValue(250);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(480, 160, 111, 20));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(630, 10, 31, 16));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(70, 60, 31, 16));
        reToZeroTime = new QSpinBox(centralWidget);
        reToZeroTime->setObjectName(QString::fromUtf8("reToZeroTime"));
        reToZeroTime->setGeometry(QRect(240, 180, 51, 22));
        reToZeroTime->setMinimum(1);
        reToZeroTime->setMaximum(1000);
        reToZeroTime->setSingleStep(10);
        reToZeroTime->setValue(10);
        sbFrqPS1 = new QSpinBox(centralWidget);
        sbFrqPS1->setObjectName(QString::fromUtf8("sbFrqPS1"));
        sbFrqPS1->setGeometry(QRect(110, 60, 61, 22));
        sbFrqPS1->setMinimum(0);
        sbFrqPS1->setMaximum(2000);
        pbWrite = new QPushButton(centralWidget);
        pbWrite->setObjectName(QString::fromUtf8("pbWrite"));
        pbWrite->setGeometry(QRect(690, 190, 61, 23));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 650, 75, 23));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 90, 61, 16));
        outFName = new QLineEdit(centralWidget);
        outFName->setObjectName(QString::fromUtf8("outFName"));
        outFName->setGeometry(QRect(450, 190, 231, 20));
        sbIMaxPS1 = new QSpinBox(centralWidget);
        sbIMaxPS1->setObjectName(QString::fromUtf8("sbIMaxPS1"));
        sbIMaxPS1->setGeometry(QRect(30, 110, 61, 22));
        sbIMaxPS1->setMinimum(-1000);
        sbIMaxPS1->setMaximum(1000);
        cbType = new QComboBox(centralWidget);
        cbType->setObjectName(QString::fromUtf8("cbType"));
        cbType->setGeometry(QRect(90, 20, 191, 22));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 20, 71, 20));
        sbCycleScan = new QSpinBox(centralWidget);
        sbCycleScan->setObjectName(QString::fromUtf8("sbCycleScan"));
        sbCycleScan->setGeometry(QRect(430, 60, 61, 22));
        sbCycleScan->setMinimum(1);
        sbCycleScan->setMaximum(33);
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(310, 60, 111, 16));
        cbGraphView = new QComboBox(centralWidget);
        cbGraphView->setObjectName(QString::fromUtf8("cbGraphView"));
        cbGraphView->setGeometry(QRect(220, 650, 161, 22));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(140, 650, 61, 16));
        sbFrqPS2 = new QSpinBox(centralWidget);
        sbFrqPS2->setObjectName(QString::fromUtf8("sbFrqPS2"));
        sbFrqPS2->setEnabled(true);
        sbFrqPS2->setGeometry(QRect(220, 60, 61, 22));
        sbFrqPS2->setMinimum(1);
        sbFrqPS2->setMaximum(2000);
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(180, 60, 31, 16));
        sbIMaxPS2 = new QSpinBox(centralWidget);
        sbIMaxPS2->setObjectName(QString::fromUtf8("sbIMaxPS2"));
        sbIMaxPS2->setGeometry(QRect(410, 110, 61, 22));
        sbIMaxPS2->setMinimum(-1000);
        sbIMaxPS2->setMaximum(1000);
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(410, 90, 61, 16));
        sbIShB_PS1 = new QSpinBox(centralWidget);
        sbIShB_PS1->setObjectName(QString::fromUtf8("sbIShB_PS1"));
        sbIShB_PS1->setGeometry(QRect(160, 110, 61, 22));
        sbIShB_PS1->setMinimum(-1000);
        sbIShB_PS1->setMaximum(1000);
        sbIShE_PS1 = new QSpinBox(centralWidget);
        sbIShE_PS1->setObjectName(QString::fromUtf8("sbIShE_PS1"));
        sbIShE_PS1->setGeometry(QRect(290, 110, 61, 22));
        sbIShE_PS1->setMinimum(-1000);
        sbIShE_PS1->setMaximum(1000);
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(100, 110, 61, 16));
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(240, 110, 51, 16));
        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(220, 90, 31, 16));
        sbIShE_PS2 = new QSpinBox(centralWidget);
        sbIShE_PS2->setObjectName(QString::fromUtf8("sbIShE_PS2"));
        sbIShE_PS2->setGeometry(QRect(690, 110, 61, 22));
        sbIShE_PS2->setMinimum(-1000);
        sbIShE_PS2->setMaximum(1000);
        label_16 = new QLabel(centralWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(610, 90, 31, 16));
        sbIShB_PS2 = new QSpinBox(centralWidget);
        sbIShB_PS2->setObjectName(QString::fromUtf8("sbIShB_PS2"));
        sbIShB_PS2->setGeometry(QRect(540, 110, 61, 22));
        sbIShB_PS2->setMinimum(-1000);
        sbIShB_PS2->setMaximum(1000);
        label_17 = new QLabel(centralWidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(480, 110, 61, 16));
        label_18 = new QLabel(centralWidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(630, 110, 51, 16));
        oddEvenEqCB = new QCheckBox(centralWidget);
        oddEvenEqCB->setObjectName(QString::fromUtf8("oddEvenEqCB"));
        oddEvenEqCB->setGeometry(QRect(20, 150, 211, 21));
        oddEvenEqCB->setCheckable(true);
        oddEvenEqCB->setChecked(true);
        verticalLayoutWidget_3 = new QWidget(centralWidget);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(10, 430, 701, 211));
        grLayout2 = new QVBoxLayout(verticalLayoutWidget_3);
        grLayout2->setSpacing(6);
        grLayout2->setContentsMargins(11, 11, 11, 11);
        grLayout2->setObjectName(QString::fromUtf8("grLayout2"));
        grLayout2->setContentsMargins(0, 0, 0, 0);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 774, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Sample time, us", 0, QApplication::UnicodeUTF8));
        retToZeroCB->setText(QApplication::translate("MainWindow", "Linear return to 0 current from time, ms", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Output file name", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "T0,s", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "f1,Hz", 0, QApplication::UnicodeUTF8));
        pbWrite->setText(QApplication::translate("MainWindow", "Write", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Refresh", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Imax PS1,A", 0, QApplication::UnicodeUTF8));
        cbType->clear();
        cbType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "\320\241\320\277\320\270\321\200\320\260\320\273\321\214\320\275\320\276\320\265  \321\201\320\272\320\260\320\275\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "\320\237\320\270\320\273\320\276\320\276\320\261\321\200\320\260\320\267\320\275\321\213\320\271 \321\202\320\276\320\272", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "\320\223\320\260\321\200\320\274\320\276\320\275\320\270\321\207\320\265\321\201\320\272\320\270\320\271 \321\202\320\276\320\272 Sin(x) for all", 0, QApplication::UnicodeUTF8)
        );
        label_8->setText(QApplication::translate("MainWindow", "Current form", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Number of scan cycle", 0, QApplication::UnicodeUTF8));
        cbGraphView->clear();
        cbGraphView->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Current in even cycle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Current in odd cycle", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Current in both cycle", 0, QApplication::UnicodeUTF8)
        );
        label_10->setText(QApplication::translate("MainWindow", "Graph view", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "f2,Hz", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Imax PS2,A", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "Shift begin", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Shift end", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "PS1", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainWindow", "PS2", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainWindow", "Shift begin", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("MainWindow", "Shift end", 0, QApplication::UnicodeUTF8));
        oddEvenEqCB->setText(QApplication::translate("MainWindow", "Even cycle is equivalent of odd cycle", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
