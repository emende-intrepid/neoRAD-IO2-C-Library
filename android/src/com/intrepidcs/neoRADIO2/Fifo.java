package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class Fifo extends NativeLinked {
    public Fifo() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        pointer = bb.getInt();
        maxSz = bb.getInt();
        in = bb.getInt();
        out = bb.getInt();
        numItems = bb.getInt();
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        bb.putInt(pointer);
        bb.putInt(maxSz);
        bb.putInt(in);
        bb.putInt(out);
        bb.putInt(numItems);
    }

    public int pointer;
    public int maxSz;
    public int in;
    public int out;
    public int numItems;
}
