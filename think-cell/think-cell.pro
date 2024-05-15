TEMPLATE = app
CONFIG += console c++11 c++14 c++17
CONFIG -= app_bundle
CONFIG -= qt

__DEBUG__ = 1

SOURCES += \
        llist.c \
        main.cpp

HEADERS += \
    Location_storage.h \
    llist.h
