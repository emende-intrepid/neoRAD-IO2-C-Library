package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class USBDevice extends NativeLinked {

    private int SERIAL_LENGTH = 6;

    public USBDevice() {
        super(null);
    }

    USBDevice(ByteBuffer bb){
        super(bb);
        pull();
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        device.fromNative(bb);
        for(int i = 0; i < SERIAL_LENGTH; i++){
            serial[i] = bb.get() & 0xFF;
        }
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        device.toNative(bb);
        for(int i = 0; i < SERIAL_LENGTH; i++){
            bb.put((byte)serial[i]);
        }
    }

    public ft260Device device = new ft260Device();
    public int[] serial = new int[SERIAL_LENGTH];
}
