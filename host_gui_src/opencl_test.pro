#-------------------------------------------------
#
# Project created by QtCreator 2017-06-27T12:56:28
#
#-------------------------------------------------

QT       += core gui

CONFIG   += c++14


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencl_test
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


linux {
    INCLUDEPATH += /export/space/opt/Xilinx/SDx/2016.4/runtime/include/1_2/
    INCLUDEPATH += /export/space/opt/Xilinx/SDx/2016.4/Vivado_HLS/include/
    LIBS += -L/export/space/opt/Xilinx/SDx/2016.4/runtime/lib/x86_64 -lOpenCL
}

macx {
    LIBS += -framework OpenCL
}
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    device_list.cpp \
    deviceinfo.cpp \
    cl_hw_info.cpp \
    devicestatemachine.cpp \
    sim_task.cpp \
    pstatemachine.cpp \
    cl_util.cpp \
    cl_sw_info.cpp

HEADERS += \
        mainwindow.h \
    device_list.h \
    deviceinfo.h \
    cl_hw_info.h \
    ocl_h.h \
    devicestatemachine.h \
    sim_task.h \
    pstatemachine.h \
    cl_util.h \
    cl_sw_info.h \
    particle.h \
    trans_mat.h

FORMS += \
        mainwindow.ui \
    device_list.ui \
    deviceinfo.ui \
    sim_task.ui

DISTFILES +=
