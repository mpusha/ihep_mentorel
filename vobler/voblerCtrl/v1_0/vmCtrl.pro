#-------------------------------------------------
#
# Project created by QtCreator 2017-03-09T11:44:09
#
#-------------------------------------------------

QT       += core
VERSION = 1.00.00

#version 1.00.00 begin
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include (../config.pro)

TARGET = voblerCtrl
TEMPLATE = app console
CONFIG += exception
MOC_DIR = moc
OBJECTS_DIR = obj

SOURCES += main.cpp\
          srvBehav.cpp \
          progerror.cpp \
    spi.c \
    SimpleGPIO.cpp

HEADERS +=srvBehav.h \
          progerror.h \
          code_errors.h \
    spi.h \
    SimpleGPIO.h


DESTDIR = ../$${CURRENT_BUILD}
