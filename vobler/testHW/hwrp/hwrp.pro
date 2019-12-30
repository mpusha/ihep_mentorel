#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T11:41:09
#
#-------------------------------------------------

QT       += core

QT       -= gui
VERSION = 1.00.00

#version 1.00.00 begin
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
TARGET = testHW
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
CONFIG += exception
MOC_DIR = moc
OBJECTS_DIR = obj

SOURCES += main.cpp \
    SimpleGPIO.cpp \
    spi.c \
    hw.cpp

HEADERS += \
    SimpleGPIO.h \
    spi.h \
    hw.h
