package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class ft260Device extends NativeLinked {

    public ft260Device() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        for(int i = 0; i < PATH_INTERFACE_LENGTH; i++){
            PathInterface0[i] = bb.get();
        }
        for(int i = 0; i < PATH_INTERFACE_LENGTH; i++){
            PathInterface1[i] = bb.get();
        }
        for(int i = 0; i < SERIAL_NUMBER_LENGTH; i++){
            SerialNumber[i] = bb.getInt();
        }
        HandleInterface0_Pointer_Address = bb.getInt();
        HandleInterface1_Pointer_Address = bb.getInt();
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        for(int i = 0; i < PATH_INTERFACE_LENGTH; i++){
            bb.put(PathInterface0[i]);
        }
        for(int i = 0; i < PATH_INTERFACE_LENGTH; i++){
            bb.put(PathInterface1[i]);
        }
        for(int i = 0; i < SERIAL_NUMBER_LENGTH; i++){
            bb.putInt(SerialNumber[i]);
        }
        bb.putInt(HandleInterface0_Pointer_Address);
        bb.putInt(HandleInterface1_Pointer_Address);
    }

    public String getSerialString(){
        String serial = "";
        for(int i = 0; i < SerialNumber.length; i++){
            serial += Integer.toString(SerialNumber[i]);
        }
        return serial;
    }

    private static final int PATH_INTERFACE_LENGTH = 256;
    private static final int SERIAL_NUMBER_LENGTH = 16;

    public byte[] PathInterface0 = new byte[PATH_INTERFACE_LENGTH];
    public byte[] PathInterface1 = new byte[PATH_INTERFACE_LENGTH];
    public int[] SerialNumber = new int[SERIAL_NUMBER_LENGTH];
    public int HandleInterface0_Pointer_Address;
    public int HandleInterface1_Pointer_Address;
}
