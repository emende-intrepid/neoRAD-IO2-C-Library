package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class Settings extends NativeLinked {

    public Settings() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        config.fromNative(bb);
        name1.fromNative(bb);
        name2.fromNative(bb);
        name3.fromNative(bb);
        can.fromNative(bb);
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        config.toNative(bb);
        name1.toNative(bb);
        name2.toNative(bb);
        name3.toNative(bb);
        can.toNative(bb);
    }

    public DeviceSettings config = new DeviceSettings();

    public ChannelName name1 = new ChannelName();

    public ChannelName name2 = new ChannelName();

    public ChannelName name3 = new ChannelName();

    public CanSettings can = new CanSettings();
}
