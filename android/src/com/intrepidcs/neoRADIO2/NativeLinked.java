package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public abstract class NativeLinked {

    protected final ByteBuffer unmanaged;
    public static final ByteOrder BYTE_ORDER = ByteOrder.LITTLE_ENDIAN;

    public NativeLinked(ByteBuffer bb) {
        unmanaged = bb;
    }

    void pull(){
        if(isLinked()){
            unmanaged.position(0);
            fromNative(unmanaged);
        }
    }

    void push(){
        if(isLinked()){
            unmanaged.position(0);
            toNative(unmanaged);
        }
    }

    boolean isLinked(){
        return unmanaged != null;
    }

    public abstract void fromNative(ByteBuffer bb);
    public abstract void toNative(ByteBuffer bb);
}
