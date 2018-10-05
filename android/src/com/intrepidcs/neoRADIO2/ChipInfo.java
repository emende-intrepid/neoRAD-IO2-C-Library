package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class ChipInfo extends NativeLinked{

    public ChipInfo() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        serialNumber = bb.getInt();
        manufactureYear = bb.getShort();
        manufactureDay = bb.get() & 0xFF;
        manufactureMonth = bb.get() & 0xFF;
        deviceType = bb.get() & 0xFF;
        firmwareVersionMajor = bb.get() & 0xFF;
        firmwareVersionMinor = bb.get() & 0xFF;
        hardwareRevMajor = bb.get() & 0xFF;
        hardwareRevMinor = bb.get() & 0xFF;
        status = bb.get() & 0xFF;
        settingsValid = bb.get() & 0xFF;
        lastReadTimeUs = bb.getLong();
        settings.fromNative(bb);
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        bb.putInt(serialNumber);
        bb.putShort(manufactureYear);
        bb.put((byte)manufactureDay);
        bb.put((byte)manufactureMonth);
        bb.put((byte)deviceType);
        bb.put((byte)firmwareVersionMajor);
        bb.put((byte)firmwareVersionMinor);
        bb.put((byte)hardwareRevMajor);
        bb.put((byte)hardwareRevMinor);
        bb.put((byte)status);
        bb.put((byte)settingsValid);
        bb.putLong(lastReadTimeUs);
        settings.toNative(bb);
    }

    public int serialNumber;

    public short manufactureYear;

    public int manufactureDay;

    public int manufactureMonth;

    public int deviceType;

    public int firmwareVersionMajor;

    public int firmwareVersionMinor;

    public int hardwareRevMajor;

    public int hardwareRevMinor;

    public int status;

    public int settingsValid;

    public long lastReadTimeUs;

    public DeviceSettings settings = new DeviceSettings();
}
