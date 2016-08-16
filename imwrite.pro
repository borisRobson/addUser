#-------------------------------------------------
#
# Project created by QtCreator 2016-08-15T11:45:39
#
#-------------------------------------------------

QT       += core gui

TARGET = addUser
TEMPLATE = app

INCLUDEPATH = /home/standby/opencv/opencv-2.4.10/include

LIBS += -L/home/standby/opencv/opencv-2.4.10/build/lib -lopencv_core -lopencv_imgproc -lopencv_video -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_flann
LIBS += -L/home/standby/opencv/opencv-2.4.10/build/lib -lopencv_features2d -lopencv_calib3d -lopencv_ml -lopencv_legacy -lopencv_photo -lopencv_gpu -lopencv_ocl -lopencv_nonfree

SOURCE_DIR = ./source
HEADER_DIR = ./headers

SOURCES += $$SOURCE_DIR/main.cpp \
    $$SOURCE_DIR/captureimage.cpp \
    $$SOURCE_DIR/detectobject.cpp

HEADERS  += $$HEADER_DIR/captureimage.h \
    $$HEADER_DIR/detectobject.h

FORMS    +=
