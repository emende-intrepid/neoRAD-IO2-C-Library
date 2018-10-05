package com.intrepidcs.neoRADIO2;

import com.intrepidcs.neoRADIO2.NativeLinked;

import java.nio.ByteBuffer;

public class DeviceSettings extends NativeLinked {

    public DeviceSettings() {
        super(null);
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        sampleRate = bb.getInt();
        channel1Config = bb.getInt();
        channel2Config = bb.getInt();
        channel3Config = bb.getInt();
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        bb.putInt(sampleRate);
        bb.putInt(channel1Config);
        bb.putInt(channel2Config);
        bb.putInt(channel3Config);
    }

    public int sampleRate;

    //TC
    public int channel1Config;

    //AOUT and DIO
    public int channel2Config;

    // ¯\_(ツ)_/¯
    public int channel3Config;

    public static final class AINEnableSetting {
        public static final int DISABLE = 0x00;
        public static final int LOWRANGE_250MV = 0x01;
        public static final int LOWRANGE_500MV = 0x02;
        public static final int LOWRANGE_1000MV = 0x03;
        public static final int LOWRANGE_2000MV = 0x04;
        public static final int LOWRANGE_4000MV = 0x05;
        public static final int LOWRANGE_5000MV = 0x06;
        public static final int HIGHRANGE_2V = 0x07;
        public static final int HIGHRANGE_4V = 0x08;
        public static final int HIGHRANGE_8V = 0x09;
        public static final int HIGHRANGE_16V = 0x0A;
        public static final int HIGHRANGE_32V = 0x0B;
        public static final int HIGHRANGE_42V = 0x0C;
    }

    public static final class AOUTEnableSetting {
        public byte enable;
        public short defaultValue;
    }

    public static final class PWRRLYEnableSetting {
        public static final int Channel_1_Enable = 0x01;
        public static final int Channel_2_Enable = 0x02;
        public static final int Channel_3_Enable = 0x03;
        public static final int Channel_4_Enable = 0x04;
        public static final int Channel_5_Enable = 0x05;
        public static final int Channel_6_Enable = 0x06;
        public static final int Channel_7_Enable = 0x07;
        public static final int Channel_8_Enable = 0x08;
    }

    public static final class TCEnableSetting {
        public static final int DISABLE = 0x00;
        public static final int TC = 0x01;
        public static final int CJ = 0x02;
        public static final int TCCJ = 0x03;
    }

}
