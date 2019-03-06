TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = CFEM

SOURCES += \
    main.cpp \
    FEMSolver.cpp \
    PhyElement.cpp \
    PhyElementBar.cpp \
    PhyElementFrame.cpp \
    PhyElementTruss.cpp \
    PhyGlobal.cpp \
    PhyNode.cpp \
    PhyElementBeam.cpp

HEADERS += \
    CFEMTypes_Global.h \
    FEMSolver.h \
    PhyDof.h \
    PhyElement.h \
    PhyElementBar.h \
    PhyElementFrame.h \
    PhyElementTruss.h \
    PhyGlobal.h \
    PhyNode.h \
    PhyElementBeam.h
