LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE 	:= andfluids
#LOCAL_SRC_FILES := Main.cpp
LOCAL_SRC_FILES := plasma.cpp
LOCAL_LDLIBS    := -lm -llog -landroid
LOCAL_CFLAGS    := -Werror -g -ggdb -O0

FILE_LIST := $(wildcard $(LOCAL_PATH)/fluids/common/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)
FILE_LIST := $(wildcard $(LOCAL_PATH)/fluids/fluids/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

#LOCAL_SRC_FILES := fluids/common/geomx.cpp
#LOCAL_SRC_FILES += fluids/common/mdebug.cpp
#LOCAL_SRC_FILES += fluids/common/mtime.cpp
#LOCAL_SRC_FILES += fluids/common/point_set.cpp
#LOCAL_SRC_FILES += fluids/common/vector.cpp
#LOCAL_SRC_FILES += plasma.c

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
