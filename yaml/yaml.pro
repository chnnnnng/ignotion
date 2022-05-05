QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QSSH_ROOT = $$PWD
DESTDIR   = $$PWD/lib

TEMPLATE = lib
DEFINES += YAML_LIBRARY

#TARGET  = $$qtLibraryTarget(YAML)

CONFIG += c++11

include ($${PWD}/yaml.pri)
