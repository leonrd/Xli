LOCAL_PATH := $(call my-dir)
XLI_LIB_PATH := ../../../../lib/android/$(TARGET_ARCH_ABI)


# libXli.so

include $(CLEAR_VARS)
LOCAL_MODULE := Xli
LOCAL_SRC_FILES := $(XLI_LIB_PATH)/libXli.so
include $(PREBUILT_SHARED_LIBRARY)	


# libXliMain.a

include $(CLEAR_VARS)
LOCAL_MODULE := XliMain

LOCAL_SRC_FILES := $(XLI_LIB_PATH)/libXliMain.a
include $(PREBUILT_STATIC_LIBRARY)	


# libXliPlatform.so

include $(CLEAR_VARS)
LOCAL_MODULE := XliPlatform

LOCAL_SRC_FILES := $(XLI_LIB_PATH)/libXliPlatform.so
include $(PREBUILT_SHARED_LIBRARY)	


# libXliGL.so

include $(CLEAR_VARS)
LOCAL_MODULE := XliGL
LOCAL_SRC_FILES := $(XLI_LIB_PATH)/libXliGL.so
include $(PREBUILT_SHARED_LIBRARY)


# libGLWindow.so

include $(CLEAR_VARS)
LOCAL_MODULE    := GLWindow

LOCAL_SRC_FILES := \
	../../Main.cpp \

LOCAL_C_INCLUDES := ../../../include

LOCAL_CFLAGS   := -fexceptions
LOCAL_LDLIBS   := -lm -lGLESv2 -landroid -llog

LOCAL_WHOLE_STATIC_LIBRARIES := XliMain
LOCAL_SHARED_LIBRARIES := Xli XliGL XliPlatform

include $(BUILD_SHARED_LIBRARY)
