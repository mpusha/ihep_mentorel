#-------------------------------------------------
#
# Project created by QtCreator 2017-03-09T11:44:09
#
#-------------------------------------------------

QT       += core

VERSION = 1.00.00

#version 1.00.00 under U70 control system RF amplitude of booster 
# based by ver 1.50.00 for vobler 

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include (../config.pro)

TARGET = wd
target.file=wd
target.path=/home/work/programs
# add Ciphers aes128-ctr,aes192-ctr,aes256-ctr,aes128-cbc   in file sshd_config

INSTALLS=target

TEMPLATE = app
CONFIG += console app
CONFIG -= app_bundle
CONFIG += exception

MOC_DIR = moc
OBJECTS_DIR = obj


SOURCES += wd.c
          

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
