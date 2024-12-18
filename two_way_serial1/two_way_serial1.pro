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

#copy_files.commands = mkdir -p $$OUT_PWD/ ; cp $$PWD/PageTree.* $$OUT_PWD/
#copy_files.depends = $$SOURCES
#QMAKE_EXTRA_TARGETS += copy_files
#PRE_TARGETDEPS += copy_files

QMAKE_CXXFLAGS += -Wno-expansion-to-defined -Wno-deprecated-copy
QMAKE_CXXFLAGS += -std=c++17

# for VideoCap
INCLUDEPATH += /usr/include/opencv4 include/
LIBS += -L/usr/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_videoio
CONFIG += link_pkgconfig
PKGCONFIG += opencv4

# Add YAML-cpp library
LIBS += -lyaml-cpp

# Include the path to yaml-cpp headers if needed (adjust the path if necessary)
INCLUDEPATH += /usr/include/yaml-cpp

# Add OpenCV to INCLUDEPATH
INCLUDEPATH += /usr/include/opencv4
INCLUDEPATH += $$PWD/../common

# Link OpenCV libraries (use pkg-config to get the correct flags)
LIBS += `pkg-config --cflags --libs opencv4`

SOURCES += \
    src/communication.cpp \
    src/key_filter.cpp \
    src/led_indicator.cpp \
    src/navigator.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/page_tree.cpp \
    src/serializer.cpp \
    src/web_camera.cpp \
    src/utils.cpp \
    src/path_finder.cpp

HEADERS += \
    include/communication.h \
    include/key_filter.h \
    include/led_indicator.h \
    include/mainwindow.h \
    include/navigator.hpp \
    include/page_tree.h \
    include/serializer.h \
    include/web_camera.h \
    include/utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    app_resource.qrc

DISTFILES += \
    test_and_examples/command_script_expamle.json \
    test_and_examples/command_script_expamle.yaml \
    test_and_examples/PageTree.csv \
    test_and_examples/PageTree.yaml
