package com.intrepidcs.neoRADIO2;

import java.nio.ByteBuffer;

public class DeviceInfo extends NativeLinked {

    public DeviceInfo() {
        super(null);
    }
    DeviceInfo(ByteBuffer bb){
        super(bb);
        pull();
    }

    @Override
    public void fromNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        usbDevice.fromNative(bb);
        for(int i = 0; i < MAX_SUPPORTED_USB_DEVICES; i++){
            for(int j = 0; j < MAX_SUPPORTED_USB_DEVICES; j++){
                ChainList[i][j] = new ChipInfo();
                ChainList[i][j].fromNative(bb);
            }
        }
        State = bb.getInt();
        lastDevice = bb.get() & 0xFF;
        lastBank = bb.get() & 0xFF;
        timeUs = bb.getLong();
        online = bb.get() & 0xFF;
        for(int i = 0; i < RX_BUFFER_SIZE; i++) {
            rxBuffer[i] = bb.get() & 0xFF;
        }
        for(int i = 0; i < TX_BUFFER_SIZE; i++) {
            txbuffer[i] = bb.get() & 0xFF;
        }
        rxFifo.fromNative(bb);
        txFifo.fromNative(bb);
        for(int i = 0; i < RX_PACKET_BUFFER_SIZE; i++){
            rxDataBuffer[i] = new Frame();
            rxDataBuffer[i].fromNative(bb);
        }
        rxDataCount = bb.get() & 0xFF;
    }

    @Override
    public void toNative(ByteBuffer bb) {
        bb.order(BYTE_ORDER);
        usbDevice.toNative(bb);
        for(int i = 0; i < MAX_SUPPORTED_USB_DEVICES; i++){
            for(int j = 0; j < MAX_SUPPORTED_USB_DEVICES; j++){
                ChainList[i][j].toNative(bb);
            }
        }
        bb.putInt(State);
        bb.put((byte)lastDevice);
        bb.put((byte)lastBank);
        bb.putLong(timeUs);
        bb.put((byte)online);
        for(int i = 0; i < RX_BUFFER_SIZE; i++){
            bb.put((byte)rxBuffer[i]);
        }
        for(int i = 0; i < TX_BUFFER_SIZE; i++){
            bb.put((byte)txbuffer[i]);
        }
        rxFifo.toNative(bb);
        txFifo.toNative(bb);
        for(int i = 0; i < RX_PACKET_BUFFER_SIZE; i++){
            rxDataBuffer[i].toNative(bb);
        }
        bb.put((byte)rxDataCount);
    }

    /**
     * Possible values for {@Link #State}
     */
    public static final class RunStates{
        public static final int Disconnected = 0;
        public static final int ConnectedWaitForAppStart = 1;
        public static final int ConnectedWaitIdentResponse = 2;
        public static final int ConncectedWaitSettings = 3;
        public static final int Connected = 4;
    }

    public static final int RX_BUFFER_SIZE = 256;
    public static final int TX_BUFFER_SIZE = 256;
    public static final int RX_PACKET_BUFFER_SIZE = (RX_BUFFER_SIZE / 6) + 1;
    public static final int MAX_SUPPORTED_USB_DEVICES = 8;

    public USBDevice usbDevice = new USBDevice();

    public ChipInfo[][] ChainList = new ChipInfo[MAX_SUPPORTED_USB_DEVICES][MAX_SUPPORTED_USB_DEVICES];

    /**
     * @see com.intrepidcs.neoRADIO2.DeviceInfo.RunStates
     */
    public int State;

    public int lastDevice;

    public int lastBank;

    public long timeUs;

    public int online;

    public int[] rxBuffer = new int[RX_BUFFER_SIZE];

    public int[] txbuffer = new int[TX_BUFFER_SIZE];

    public Fifo rxFifo = new Fifo();

    public Fifo txFifo = new Fifo();

    public Frame[] rxDataBuffer = new Frame[RX_PACKET_BUFFER_SIZE];

    public int rxDataCount;
}
