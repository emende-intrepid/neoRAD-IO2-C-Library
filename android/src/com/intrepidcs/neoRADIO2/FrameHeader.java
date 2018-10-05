package com.intrepidcs.neoRADIO2;

import com.intrepidcs.neoRADIO2.NativeLinked;

import java.nio.ByteBuffer;

public class FrameHeader extends NativeLinked {

    public FrameHeader() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        startOfFrame = bb.get() & 0xFF;
        commandStatus = bb.get() & 0xFF;
        device = bb.get() & 0xFF;
        bank = bb.get() & 0xFF;
        len = bb.get() & 0xFF;
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        bb.put((byte)startOfFrame);
        bb.put((byte)commandStatus);
        bb.put((byte)device);
        bb.put((byte)bank);
        bb.put((byte)len);
    }

    public int startOfFrame;

    public int commandStatus;

    public int device;

    public int bank;

    public int len;
}
