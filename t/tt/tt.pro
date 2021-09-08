#-------------------------------------------------
#
# Project created by QtCreator 2021-06-29T12:04:17
#
#-------------------------------------------------

QT       += core gui
#QT -=gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#CONFIG +=console
TEMPLATE = app

MOC_DIR = ../moc
OBJECTS_DIR = ../obj
include (../config.pro)
DESTDIR = ../../$${CURRENT_BUILD}

TARGET = test
target.file=test
target.path=/home/work/programs
INSTALLS=target


SOURCES += main.cpp mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CFLAGS_RELEASE -= -O
QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE -= -O3
#QMAKE_CXXFLAGS +=O0
