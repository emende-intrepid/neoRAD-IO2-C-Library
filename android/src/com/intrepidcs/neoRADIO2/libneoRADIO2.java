package com.intrepidcs.neoRADIO2;

import android.util.Log;

public final class libneoRADIO2 {
    static{

        try{
            System.loadLibrary("neoRADIO");
        }
        catch (Throwable e){
            e.printStackTrace();
            Log.e("libneoRADIO", "libneoRADIO.so missing.");
        }
    }

    public static native int FindDevices();

    public static native void CloseDevice(int deviceNum);

    public static native int ConnectDevice(int deviceNum);

    public static native int ProcessIncomingData(int deviceNum);

    public static native int SetDeviceSettings(int deviceNum, byte device, byte bank, DeviceSettings deviceSettings);

    public static native void SetOnline(int deviceNum, int online);

    public static native int RequestSettings(int deviceNum);

    public static native int SettingsValid(int deviceNum);

    public static native int GetDeviceType(int deviceNum, byte device, byte bank);

    public static native int GetDeviceState(int deviceNum);

    public static native int HasRxData(int deviceNum);

    public static native void DeviceInfoFromNative(int deviceNum, DeviceInfo deviceInfo);

    public static native void DeviceInfoToNative(int deviceNum, DeviceInfo deviceInfo);
}
