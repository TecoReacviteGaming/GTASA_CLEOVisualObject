LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE := CLEO_VisualObject
LOCAL_SRC_FILES := main.cpp mod/logger.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
LOCAL_LDLIBS += -llog
LOCAL_C_INCLUDES += $(LOCAL_PATH)/rw

include $(BUILD_SHARED_LIBRARY)