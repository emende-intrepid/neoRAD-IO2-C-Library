package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class Frame extends NativeLinked {
    public Frame() {
        super(null);
    }

    private static final int DATA_LENGTH = 64;

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        header.fromNative(bb);
        for(int i = 0; i < DATA_LENGTH; i++){
            data[i] = bb.get();
        }
        crc = bb.get() & 0xFF;
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        header.toNative(bb);
        for(int i = 0; i < DATA_LENGTH; i++){
            bb.put(data[i]);
        }
        bb.put((byte)crc);
    }

    public FrameHeader header = new FrameHeader();

    public byte[] data = new byte[DATA_LENGTH];

    public int crc;
}
