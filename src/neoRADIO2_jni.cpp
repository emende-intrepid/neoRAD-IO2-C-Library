#include <jni.h>
#include <map>
#include <mutex>
#include <android/log.h>
#include <thread>

struct jDeviceInfo_t {
    jclass classRef;
    jmethodID fromNativeID;
    jmethodID toNativeID;
};

struct jUSBDevice_t {
    jclass classRef;
    jmethodID fromNativeID;
    jmethodID toNativeID;
};

struct jDeviceSettings_t {
    jclass classRef;
    jmethodID fromNativeID;
    jmethodID toNativeID;  
};

class JavaEnvInfo
{
public:
    JavaEnvInfo(JNIEnv* e) : env(e)
    {
        jclass ref = env->FindClass("com/intrepidcs/neoRADIO2/DeviceInfo");
        jDeviceInfo.classRef = (jclass)env->NewGlobalRef(ref);
        env->DeleteLocalRef(ref);
        jDeviceInfo.fromNativeID = env->GetMethodID(jDeviceInfo.classRef, "fromNative", "(Ljava/nio/ByteBuffer;)V");
        jDeviceInfo.toNativeID = env->GetMethodID(jDeviceInfo.classRef, "toNative", "(Ljava/nio/ByteBuffer;)V");
        
        ref = env->FindClass("com/intrepidcs/neoRADIO2/USBDevice");
        jUSBDevice.classRef = (jclass)env->NewGlobalRef(ref);
        env->DeleteLocalRef(ref);
        jUSBDevice.fromNativeID = env->GetMethodID(jUSBDevice.classRef, "fromNative", "(Ljava/nio/ByteBuffer;)V");
        jUSBDevice.toNativeID = env->GetMethodID(jUSBDevice.classRef, "toNative", "(Ljava/nio/ByteBuffer;)V");

        ref = env->FindClass("com/intrepidcs/neoRADIO2/DeviceSettings");
        jDeviceSettings.classRef = (jclass)env->NewGlobalRef(ref);
        env->DeleteLocalRef(ref);
        jDeviceSettings.fromNativeID = env->GetMethodID(jDeviceSettings.classRef, "fromNative", "(Ljava/nio/ByteBuffer;)V");
        jDeviceSettings.toNativeID = env->GetMethodID(jDeviceSettings.classRef, "toNative", "(Ljava/nio/ByteBuffer;)V");
    }

    ~JavaEnvInfo()
    {
        env->DeleteGlobalRef(jDeviceInfo.classRef);
        env->DeleteGlobalRef(jUSBDevice.classRef);
        env->DeleteGlobalRef(jDeviceSettings.classRef);
    }

    jDeviceInfo_t jDeviceInfo;
    jUSBDevice_t jUSBDevice;
    jDeviceSettings_t jDeviceSettings;

    static std::mutex mtx;
    static std::map<JNIEnv*, JavaEnvInfo*> envInfos;

    static JavaEnvInfo* getEnvInfo(JNIEnv* env)
    {
        mtx.lock();

        JavaEnvInfo* ret = 0;
        std::map<JNIEnv*, JavaEnvInfo*>::iterator it(envInfos.find(env));
        if(it == envInfos.end())
        {
            std::pair<JNIEnv*, JavaEnvInfo*> put;
            put.first = env;
            put.second = ret = new JavaEnvInfo(env);
            envInfos.insert(put);
        }
        else
        {
            ret = it->second;
        }

        mtx.unlock();
        return ret;
    }

    static void clearEnvInfos()
    {
        mtx.lock();

        for(std::map<JNIEnv*, JavaEnvInfo*>::iterator it = envInfos.begin(); it != envInfos.end(); ++it)
        {
            delete it->second;
        }
        envInfos.clear();

        mtx.unlock();
    }

private:
    JNIEnv* env;
};

std::mutex JavaEnvInfo::mtx;
std::map<JNIEnv*, JavaEnvInfo*> JavaEnvInfo::envInfos;

template<class T> jobject MakeObjectFromNativeBuffer(JNIEnv* env, jclass cls, T* obj)
{
    return env->NewObject(cls, env->GetMethodID(cls, "<init>", "(Ljava/nio/ByteBuffer;)V"), env->NewDirectByteBuffer((unsigned char*)obj, sizeof(T)));
}

extern "C"
{

#define API_FUNC(X,...) Java_com_intrepidcs_neoRADIO2_libneoRADIO2_##X(JNIEnv* env, jclass thiz, __VA_ARGS__)
#define API_FUNC_NO_ARGS(X) Java_com_intrepidcs_neoRADIO2_libneoRADIO2_##X(JNIEnv* env, jclass thiz)

#include "neoRAD-IO2_PacketHandler.h"

neoRADIO2_DeviceInfo deviceInfos[8];
neoRADIO2_USBDevice devices[8];
std::chrono::time_point<std::chrono::steady_clock> currentTimes[8];
std::chrono::time_point<std::chrono::steady_clock> lastTimes[8];

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    __android_log_print(ANDROID_LOG_DEBUG,"neoRADIO2","Loading neoRADIO2 JNI Library");
    return JNI_VERSION_1_2;
}

jint API_FUNC_NO_ARGS(FindDevices)
{
    return neoRADIO2FindDevices(devices, 8);
}

void API_FUNC(CloseDevice, int deviceNum)
{
    neoRADIO2CloseDevice(&deviceInfos[deviceNum]);
}

jint API_FUNC(ConnectDevice, int deviceNum)
{
    currentTimes[deviceNum] = std::chrono::steady_clock::now();
    lastTimes[deviceNum] = std::chrono::steady_clock::now();
    memset(&deviceInfos[deviceNum], 0, sizeof(neoRADIO2_DeviceInfo));
    memcpy(&deviceInfos[deviceNum].usbDevice, &devices[deviceNum], sizeof(neoRADIO2_USBDevice));
    return neoRADIO2ConnectDevice(&deviceInfos[deviceNum]);
}

jint API_FUNC(ProcessIncomingData, int deviceNum)
{
    currentTimes[deviceNum] = std::chrono::steady_clock::now();
    long diff = std::chrono::duration_cast<std::chrono::microseconds>(currentTimes[deviceNum] - lastTimes[deviceNum]).count();
    if(diff < 1000) 
    {
        long remaining = 1000 - diff;
        std::this_thread::sleep_for(std::chrono::microseconds(remaining));
        currentTimes[deviceNum] = std::chrono::steady_clock::now();
        diff = std::chrono::duration_cast<std::chrono::microseconds>(currentTimes[deviceNum] - lastTimes[deviceNum]).count();
    }
    memcpy(&lastTimes[deviceNum], &currentTimes[deviceNum], sizeof(std::chrono::time_point<std::chrono::steady_clock>));
    int result = neoRADIO2ProcessIncomingData(&deviceInfos[deviceNum], diff);
    return result;
}

jint API_FUNC(SetDeviceSettings, int deviceNum, uint8_t device, uint8_t bank, jobject deviceSettings)
{
    JavaEnvInfo* envInfo = JavaEnvInfo::getEnvInfo(env);
    neoRADIO2_deviceSettings settings;
    jobject buffer = env->NewDirectByteBuffer((unsigned char*)&settings, sizeof(neoRADIO2_deviceSettings));
    env->CallVoidMethod(deviceSettings, envInfo->jDeviceSettings.toNativeID, buffer);
    env->DeleteLocalRef(buffer);
    
    return neoRADIO2SetDeviceSettings(&deviceInfos[deviceNum], device, bank, &settings);
}

void API_FUNC(SetOnline, int deviceNum, int online)
{
    neoRADIO2SetOnline(&deviceInfos[deviceNum], online);
}

jint API_FUNC(RequestSettings, int deviceNum)
{
    return neoRADIO2RequestSettings(&deviceInfos[deviceNum]);
}

jint API_FUNC(SettingsValid, int deviceNum)
{
    return neoRADIO2SettingsValid(&deviceInfos[deviceNum]);
}

jint API_FUNC(GetDeviceType, int deviceNum, uint8_t device, uint8_t bank)
{
    return neoRADIO2GetGetDeviceType(&deviceInfos[deviceNum], device, bank);
}

jint API_FUNC(GetDeviceState, int deviceNum)
{
    return deviceInfos[deviceNum].State;
}

jint API_FUNC(HasRxData, int deviceNum)
{
    return (deviceInfos[deviceNum].rxDataCount > 0 && deviceInfos[deviceNum].State == neoRADIO2state_Connected) ? 1 : 0;
}

void API_FUNC(DeviceInfoFromNative, int deviceNum, jobject devInfo)
{
    JavaEnvInfo* envInfo = JavaEnvInfo::getEnvInfo(env);
    jobject buffer = env->NewDirectByteBuffer((unsigned char*)&deviceInfos[deviceNum], sizeof(neoRADIO2_DeviceInfo));
    env->CallVoidMethod(devInfo, envInfo->jDeviceInfo.fromNativeID, buffer);
    env->DeleteLocalRef(buffer);
}

void API_FUNC(DeviceInfoToNative, int deviceNum, jobject devInfo)
{
    JavaEnvInfo* envInfo = JavaEnvInfo::getEnvInfo(env);
    jobject buffer = env->NewDirectByteBuffer((unsigned char*)&deviceInfos[deviceNum], sizeof(neoRADIO2_DeviceInfo));
    env->CallVoidMethod(devInfo, envInfo->jDeviceInfo.toNativeID, buffer);
    env->DeleteLocalRef(buffer);
}

}