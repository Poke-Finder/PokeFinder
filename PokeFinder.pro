#-------------------------------------------------
#
# Project created by QtCreator 2017-07-28T23:02:16
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PokeFinder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Objects/EncounterSlot.cpp \
    Objects/LCRNG.cpp \
    Objects/MTRNG.cpp \
    Objects/NatureLock.cpp \
    Objects/Utilities.cpp

HEADERS += \
        mainwindow.h \
    Objects/Encounter.hpp \
    Objects/EncounterSlot.hpp \
    Objects/LCRNG.hpp \
    Objects/Lead.hpp \
    Objects/Method.hpp \
    Objects/MTRNG.hpp \
    Objects/NatureLock.hpp \
    Objects/Utilities.hpp

FORMS += \
        mainwindow.ui

linux:LIBS += \
       -lboost_system\

win32:INCLUDEPATH += C:/bod windowsost/include/boost-1_64