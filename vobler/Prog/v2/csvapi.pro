#-------------------------------------------------
#
# Project created by QtCreator 2017-02-28T08:33:04
#
#-------------------------------------------------

QT       += core gui script

TARGET = csvapi
TEMPLATE = app
include( qwt.pri )

SOURCES += main.cpp\
        mainwindow.cpp data_plot.cpp

HEADERS  += mainwindow.h data_plot.h

FORMS    += mainwindow.ui
