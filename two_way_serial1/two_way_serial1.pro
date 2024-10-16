QT += core gui
QT += serialport
QT += multimedia
QT += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += debug

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Add OpenCV to INCLUDEPATH
INCLUDEPATH += /usr/include/opencv4

# Link OpenCV libraries (use pkg-config to get the correct flags)
LIBS += `pkg-config --cflags --libs opencv4`

SOURCES += \
    communication.cpp \
    csv_reader.cpp \
    key_filter.cpp \
    led_indicator.cpp \
    navigator.cpp \
    main.cpp \
    mainwindow.cpp \
    serializer.cpp \
    web_camera.cpp

HEADERS += \
    communication.h \
    csv_reader.h \
    key_filter.h \
    led_indicator.h \
    mainwindow.h \
    navigator.hpp \
    serializer.h \
    web_camera.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    app_resource.qrc
