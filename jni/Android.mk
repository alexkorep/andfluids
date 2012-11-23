LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE 	:= andfluids
LOCAL_LDLIBS    := -lEGL -lGLESv2 -lm -llog -landroid
LOCAL_CFLAGS    := -Werror -g -ggdb -O0 \
		-DANDROID_NDK \
                -DDISABLE_IMPORTGL

LOCAL_SRC_FILES := main.cpp
FILE_LIST := $(wildcard $(LOCAL_PATH)/fluids/common/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)
FILE_LIST := $(wildcard $(LOCAL_PATH)/fluids/fluids/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += GraphicsService.cpp

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
