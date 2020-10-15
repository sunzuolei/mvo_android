LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:= on
OPENCV_INSTALL_MODULES:= on
OPENCV_LIB_TYPE := SHARED
OPENCV_MK_PATH := C:\Users\Pascal\Desktop\Apk\OpenCV-4.1.2-android-sdk\sdk\native\jni\OpenCV.mk
include ${OPENCV_MK_PATH}

LOCAL_SRC_FILES  := visodo.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := visodo

include $(BUILD_SHARED_LIBRARY)
