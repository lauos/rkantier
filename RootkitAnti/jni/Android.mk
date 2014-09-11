LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE    := native
LOCAL_SRC_FILES := native.c refinvoke.c reslove_mod.c modify_mod.c

LOCAL_LDLIBS += -L$(SYS_ROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)