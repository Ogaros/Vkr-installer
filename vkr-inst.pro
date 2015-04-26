#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T15:47:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vkr-inst
TEMPLATE = app

LIBS += D:\Projects\vkr-installer\sqlite3.dll

SOURCES += main.cpp\
        mainwindow.cpp \
    DBManager.cpp

HEADERS  += mainwindow.h \
    sqlite3.h \
    DBManager.h

FORMS    += mainwindow.ui

CONFIG   += c++11
