QT += core
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = zbar_gige
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ScanCode.cpp \
    qrcodezbar.cpp \
    qrgenerator.cpp \
    LZO/lzopack.cpp \
    Instuctions/stats.cpp \
    Instuctions/inirw.cpp \
    Instuctions/md5sum.cpp \
    Instuctions/base64.cpp \
    server/fragmentProcess.cpp \
    gigeGrab.cpp \
    fileParameters.cpp

HEADERS += \
    ScanCode.h \
    qrcodezbar.h \
    qrgenerator.h \
    LZO/portab.h \
    LZO/portab_a.h \
    LZO/lzo_supp.h \
    LZO/lzopack.h \
    Instuctions/stats.h \
    Instuctions/inirw.h \
    Instuctions/md5sum.h \
    Instuctions/base64.h \
    server/fragmentProcess.h \
    gigeGrab.h \
    fileParameters.h \
    DirPath.h





INCLUDEPATH += /usr/local/opencv320/include \
/usr/local/opencv320/include/opencv \
/usr/local/opencv320/include/opencv2 \
/opt/pylon5/include \
/opt/pylon5/include/pylon \
/usr/local/include \
/usr/local/libqrencode/include \
/home/montafan/3rdparty/lzo210/include \

LIBS += /usr/local/opencv320/lib/libopencv_calib3d.so \
/usr/local/opencv320/lib/libopencv_core.so \
/usr/local/opencv320/lib/libopencv_features2d.so \
/usr/local/opencv320/lib/libopencv_flann.so \
/usr/local/opencv320/lib/libopencv_highgui.so \
/usr/local/opencv320/lib/libopencv_imgcodecs.so \
/usr/local/opencv320/lib/libopencv_imgproc.so \
/usr/local/opencv320/lib/libopencv_ml.so \
/usr/local/opencv320/lib/libopencv_objdetect.so \
/usr/local/opencv320/lib/libopencv_photo.so \
/usr/local/opencv320/lib/libopencv_shape.so \
/usr/local/opencv320/lib/libopencv_stitching.so \
/usr/local/opencv320/lib/libopencv_superres.so \
/usr/local/opencv320/lib/libopencv_video.so \
/usr/local/opencv320/lib/libopencv_videoio.so \
/usr/local/opencv320/lib/libopencv_videostab.so \
/opt/pylon5/lib64/libbxapi.so \
/opt/pylon5/lib64/libFirmwareUpdate_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libGCBase_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libGenApi_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libgxapi.so \
/opt/pylon5/lib64/liblog4cpp_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libLog_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libMathParser_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libNodeMapData_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/libpylonbase.so \
/opt/pylon5/lib64/libpylonc.so \
/opt/pylon5/lib64/libpylon_TL_bcon.so \
/opt/pylon5/lib64/libpylon_TL_camemu.so \
/opt/pylon5/lib64/libpylon_TL_gige.so \
/opt/pylon5/lib64/libpylon_TL_gtc.so \
/opt/pylon5/lib64/libpylon_TL_usb.so \
/opt/pylon5/lib64/libpylonutility.so \
/opt/pylon5/lib64/libuxapi.so \
/opt/pylon5/lib64/libXmlParser_gcc_v3_0_Basler_pylon_v5_0.so \
/opt/pylon5/lib64/pylon-libusb-1.0.so \
#/usr/local/lib/libzbargtk.so \
#/usr/local/lib/libzbarqt.so \
#/usr/local/lib/libzbar.so \
/usr/local/libqrencode/lib/libqrencode.so \
/home/montafan/3rdparty/lzo210/lib/liblzo2.so \

LIBS += -L/usr/local/lib -lzbar


FORMS += qzbar.ui
