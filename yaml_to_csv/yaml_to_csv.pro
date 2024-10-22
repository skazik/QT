#-------------------------------------------------
#
# Project created by QtCreator 2024-10-22T10:36:57
#
#-------------------------------------------------

QT       += core
QT       -= gui

copy_files.commands = mkdir -p $$OUT_PWD/ ; cp $$PWD/PageTree.yaml $$OUT_PWD/
copy_files.depends = $$SOURCES

QMAKE_EXTRA_TARGETS += copy_files
PRE_TARGETDEPS += copy_files

QMAKE_CXXFLAGS += -Wno-expansion-to-defined -Wno-deprecated-copy
LIBS += -lyaml-cpp

TARGET = yaml_to_csv
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

DISTFILES += \
    PageTree.yaml \
    ../build-yaml_to_csv-Desktop_Qt_5_5_0_GCC_64bit-Debug/PageTree.csv
