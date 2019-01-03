package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class ChannelName extends NativeLinked{

    private static final int INTEGER_ARRAY_LENGTH = 16;
    private static final int SHORT_ARRAY_LENGTH = 16 * 2;
    private static final int BYTE_ARRAY_LENGTH = 16 * 4;

    public ChannelName() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        length = bb.get() & 0xFF;
        charSize = bb.get() & 0xFF;

        int startPos = bb.position();
        for(int i = 0; i < INTEGER_ARRAY_LENGTH ; i++){
            u32[i] = bb.getInt();
        }

        bb.position(startPos);
        for(int i = 0; i < SHORT_ARRAY_LENGTH; i++){
            u16[i] = bb.getShort();
        }

        bb.position(startPos);
        for(int i = 0; i < BYTE_ARRAY_LENGTH; i++){
            u8[i] = bb.get();
        }
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        bb.put((byte)length);
        bb.putInt((byte)charSize);
        for(int i = 0; i < INTEGER_ARRAY_LENGTH; i++){
            bb.putInt(u32[i]);
        }

    }

    public int length;

    public int charSize;

    public int[] u32 = new int[INTEGER_ARRAY_LENGTH];

    public short[] u16 = new short[SHORT_ARRAY_LENGTH];

    public byte[] u8 = new byte[BYTE_ARRAY_LENGTH];
}
