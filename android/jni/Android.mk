LOCAL_PATH := ..

include $(CLEAR_VARS)

LOCAL_MODULE := neoRADIO
LOCAL_LDLIBS := -llog

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/inc

LOCAL_SRC_FILES += libs/hidraw/hid.c \
    src/fifo.c \
    src/ft260.c \
    src/neoRAD-IO2.c \
    src/neoRAD-IO2_PacketHandler.c \
    src/neoRADIO2_jni.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := neoRADIO
LOCAL_SHARED_LIBRARIES := libc libcutils
include $(BUILD_SHARED_LIBRARY)