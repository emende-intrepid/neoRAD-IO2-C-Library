package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class CanSettings extends NativeLinked{

    public CanSettings() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        arbID = bb.getInt();
        location = bb.get() & 0xFF;
        msgType = bb.get() & 0xFF;
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        bb.putInt(arbID);
        bb.put((byte)location);
        bb.put((byte)msgType);
    }

    public int arbID;

    public int location;

    public int msgType;
}
