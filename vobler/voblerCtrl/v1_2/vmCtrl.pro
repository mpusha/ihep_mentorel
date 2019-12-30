#-------------------------------------------------
#
# Project created by QtCreator 2017-03-09T11:44:09
#
#-------------------------------------------------

QT       += core

VERSION = 1.20.00

#version 1.00.00 begin
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include (../config.pro)

TARGET = voblerCtrl
TEMPLATE = app
CONFIG += console app
CONFIG -= app_bundle
CONFIG += exception
MOC_DIR = moc
OBJECTS_DIR = obj

SOURCES += main.cpp\
          srvBehav.cpp \
          progerror.cpp \
          spi.c \
          SimpleGPIO.cpp \
          hw.cpp \
    fpga.c

HEADERS +=srvBehav.h \
          progerror.h \
          code_errors.h \
          spi.h \
          SimpleGPIO.h \
          hw.h \
    fpga.h
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

DESTDIR = ../$${CURRENT_BUILD}
