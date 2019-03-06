QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtFEM
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += "../CFEM"

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        ../CFEM/FEMSolver.cpp \
        ../CFEM/PhyElement.cpp \
        ../CFEM/PhyElementBar.cpp \
        ../CFEM/PhyElementFrame.cpp \
        ../CFEM/PhyElementTruss.cpp \
        ../CFEM/PhyGlobal.cpp \
        ../CFEM/PhyNode.cpp \
        ../CFEM/PhyElementBeam.cpp \
    node.cpp \
    element.cpp \
    femview.cpp

HEADERS += \
        mainwindow.h \
    node.h \
    element.h \
    femview.h

FORMS += \
        mainwindow.ui

RC_ICONS = ME517.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
