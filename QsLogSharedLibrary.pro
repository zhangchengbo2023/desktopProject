# This pro file will build QsLog as a shared library
include(QsLog.pri)

TARGET = QsLog
VERSION = "2.0.0"
QT -= gui
CONFIG -= console
CONFIG -= app_bundle
CONFIG += shared
TEMPLATE = lib

debug {
    DESTDIR = $$PWD/../bin/debug
}

release {
    DESTDIR = $$PWD/../bin/release
}

DEFINES += QSLOG_IS_SHARED_LIBRARY

