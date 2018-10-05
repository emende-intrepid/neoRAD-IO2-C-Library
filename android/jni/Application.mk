APP_ABI := armeabi-v7a
APP_STL := gnustl_static
APP_GNUSTL_FORCE_CPP_FEATURES := exceptions rtti
APP_CFLAGS := -D__ANDROID__ -std=gnu99
APP_CPPFLAGS := -fpermissive -fexceptions -frtti -Wno-psabi -std=c++11