QT += core gui

RC_ICONS += ICON.ico

RESOURCES += \
    icon.qrc
RC_FILE += Image/icon.rc


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = zbar_gige
CONFIG += console
CONFIG -= app_bundle

DEFINES += _FILE_OFFSET_BITS=64 _LARGE_FILE

TEMPLATE = app

SOURCES += main.cpp \
    server/ScanCode.cpp \
    server/qrcodezbar.cpp \
    LZO/lzopack.cpp \
    instructions/stats.cpp \
    instructions/inirw.cpp \
    instructions/md5sum.cpp \
    instructions/base64.cpp \
    server/fragmentProcess.cpp \
    fileParameters.cpp \
    server/gigeGrab.cpp \
    server/RecvStateMachine.cpp \
    instructions/split.cpp \
    instructions/cat.cpp \
    client/DirMonitor.cpp \
    instructions/threadpool.cpp \
    server/ProcessInThread.cpp \
    server/usbGrab.cpp \
    client/ClientCbNotifier.cpp \
    client/ThreadObject.cpp \
    client/TCPMonitor.cpp \
    instructions/ActiveMQProducer.cpp \
    instructions/ActiveMQConsumer.cpp \
    instructions/JSON.cpp \
    client/qrgenerator.cpp

HEADERS += \
    server/ScanCode.h \
    server/qrcodezbar.h \
    LZO/portab.h \
    LZO/portab_a.h \
    LZO/lzo_supp.h \
    LZO/lzopack.h \
    instructions/stats.h \
    instructions/inirw.h \
    instructions/md5sum.h \
    instructions/base64.h \
    server/fragmentProcess.h \
    server/gigeGrab.h \
    client/qrgenerator.h \
    server/gigeGrab.h \
    include/DirPath.h \
    include/fileParameters.h \
    include/Errors.h \
    server/RecvStateMachine.h \
    instructions/split.h \
    instructions/cat.h \
    include/macros.h \
    client/DirMonitor.h \
    instructions/threadpool.h \
    server/ProcessInThread.h \
    server/usbGrab.h \
    client/ClientCbNotifier.h \
    client/ThreadObject.h \
    client/TCPMonitor.h \
    instructions/ActiveMQProducer.h \
    instructions/ActiveMQConsumer.h \
    instructions/JSON.h


INCLUDEPATH += \
#/usr/local/opencv320/include \
#/usr/local/opencv320/include/opencv \
#/usr/local/opencv320/include/opencv2 \
/usr/local/opencv2.4.13/include \
/usr/local/opencv2.4.13/include/opencv \
/usr/local/opencv2.4.13/include/opencv2 \
/opt/pylon5/include \
/opt/pylon5/include/pylon \
/usr/local/include \
/usr/local/libqrencode/include \
/home/montafan/3rdparty/lzo210/include \
/usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.4 \
/usr/local/apr/include/apr-1 \
#/home/montafan/software_download/jsoncpp-cmake-adjustments/include \
/usr/include \

LIBS += \
/usr/local/opencv2.4.13/lib/libopencv_core.so \
/usr/local/opencv2.4.13/lib/libopencv_highgui.so \
/usr/local/opencv2.4.13/lib/libopencv_imgproc.so \
#/usr/local/opencv320/lib/libopencv_calib3d.so \
#/usr/local/opencv320/lib/libopencv_core.so \
#/usr/local/opencv320/lib/libopencv_features2d.so \
#/usr/local/opencv320/lib/libopencv_flann.so \
#/usr/local/opencv320/lib/libopencv_highgui.so \
#/usr/local/opencv320/lib/libopencv_imgcodecs.so \
#/usr/local/opencv320/lib/libopencv_imgproc.so \
#/usr/local/opencv320/lib/libopencv_ml.so \
#/usr/local/opencv320/lib/libopencv_objdetect.so \
#/usr/local/opencv320/lib/libopencv_photo.so \
#/usr/local/opencv320/lib/libopencv_shape.so \
#/usr/local/opencv320/lib/libopencv_stitching.so \
#/usr/local/opencv320/lib/libopencv_superres.so \
#/usr/local/opencv320/lib/libopencv_video.so \
#/usr/local/opencv320/lib/libopencv_videoio.so \
#/usr/local/opencv320/lib/libopencv_videostab.so \
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
/usr/local/ActiveMQ-CPP/lib/libactivemq-cpp.a \
/usr/local/apr/lib/libapr-1.a \
#/home/montafan/software_download/jsoncpp-cmake-adjustments/libs/linux-gcc-4.9.4/libjson_linux-gcc-4.9.4_libmt.so \
/usr/lib/libjson_linux-gcc-4.9.4_libmt.so

LIBS += -L/usr/local/lib -lzbar -lcrypto -lssl

# activemq-cpp-3.9.4的头文件路径和库文件路径
#INCLUDEPATH += /usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.4
#INCLUDEPATH += /usr/local/apr/include/apr-1
# apr的头文件路径和库文件路径
#LIBS += /usr/local/ActiveMQ-CPP/lib/libactivemq-cpp.a
#LIBS += /usr/local/apr/lib/libapr-1.a

FORMS += ui/qzbar.ui
