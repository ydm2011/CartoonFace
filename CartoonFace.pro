
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
functions.cpp \
    face_api_test.cpp
INCLUDEPATH += /usr/local/opencv/include \
/usr/local/opencv/include/opencv \
/usr/local/opencv/include/opencv2

LIBS += /usr/local/opencv/lib/libopencv_highgui.so \
/usr/local/opencv/lib/libopencv_calib3d.so \
/usr/local/opencv/lib/libopencv_contrib.so \
/usr/local/opencv/lib/libopencv_core.so \
/usr/local/opencv/lib/libopencv_features2d.so \
/usr/local/opencv/lib/libopencv_flann.so \
/usr/local/opencv/lib/libopencv_gpu.so \
/usr/local/opencv/lib/libopencv_imgproc.so \
/usr/local/opencv/lib/libopencv_legacy.so \
/usr/local/opencv/lib/libopencv_ml.so \
/usr/local/opencv/lib/libopencv_nonfree.so \
/usr/local/opencv/lib/libopencv_objdetect.so \
/usr/local/opencv/lib/libopencv_photo.so \
/usr/local/opencv/lib/libopencv_stitching.so \
/usr/local/opencv/lib/libopencv_video.so \
/usr/local/opencv/lib/libopencv_videostab.so \
/usr/local/libjpeg/lib/libjpeg.so \
/usr/local/libtiff/lib/libtiff.so \
/usr/local/libpng/lib/libpng.so \
/home/daoming/faceapi/faceapi/face-api-0506/lib/libface360api.a

HEADERS += \
functions.h \
    face_api.h \
    face_common.h \
    face_cartoon_api.h


