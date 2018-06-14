#include "neoRAD-IO2.h"
#include "neoRAD-IO2_PacketHandler.h"
#include "neoRADIO2_frames.h"
#include <string.h>
#include <stdio.h>

int neoRADIO2FindDevices(neoRADIO2_DeviceInfo * devInfo)
{
    int result = 0;

    ft260_device devices[NEORADIO2_MAX_SUPPORTED_USB_DEVICES];
    result = ft260FindDevices(devices, NEORADIO2_MAX_SUPPORTED_USB_DEVICES, VENDOR_ID, PRODUCT_ID);
    if (result >= 1)
    {
        memcpy(&devInfo->ft260Device, &devices[0], sizeof(ft260_device));
        return 0;
    }
    return -1;
}

int neoRADIO2ConnectDevice(neoRADIO2_DeviceInfo * devInfo)
{
    int result;
    uint8_t buf[64];

    FIFO_Init(&devInfo->rxfifo, devInfo->rxbuffer, NEORADIO2_RX_BUFFER_SIZE);
    FIFO_Init(&devInfo->txfifo, devInfo->txbuffer, NEORADIO2_TX_BUFFER_SIZE);
    result = ft260OpenDevice(&devInfo->ft260Device);
    devInfo->maxID.byte = 0;

    if (result == 0)
    {
        //Setup FT260 Clock
        buf[0] = SYSTEM_SETTING_REPORT_ID;
        buf[1] = SET_CLOCK;
        buf[2] = CLOCK_CTRL_48;
        result = ft260ConfigureDevice(&devInfo->ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        //GPIOA TX LED
        memset(buf, 0, sizeof(buf));
        buf[0] = SYSTEM_SETTING_REPORT_ID;
        buf[1] = 0x08;
        buf[2] = 0x04;
        result = ft260ConfigureDevice(&devInfo->ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        //GPIOG RX_LED
        memset(buf, 0, sizeof(buf));
        buf[0] = SYSTEM_SETTING_REPORT_ID;
        buf[1] = 0x09;
        buf[2] = 0x05;
        result = ft260ConfigureDevice(&devInfo->ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        //GPIOH UARTBREAK
        memset(buf, 0, sizeof(buf));
        buf[0] = GPIO_REPORT_ID;
        buf[1] = 0x00;
        buf[2] = 0x00;
        buf[3] = 0x80;
        buf[4] = 0x80;
        result = ft260ConfigureDevice(&devInfo->ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        result = ft260SetupUART(&devInfo->ft260Device, 500000, 1, 0);
    }

    if (result == 0)
    {
        result = neoRADIO2SendIdentifyPacket(devInfo);
    }

    if (result == 0)
    {
        devInfo->State = neoRADIO2state_ConnectedWaitIdentHeader;
    }

    return result;
}

//This must be called every 1 ms
int neoRADIO2ProcessIncommingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus)
{
    int result = 0;
    devInfo->Timeus += diffTimeus;
    switch (devInfo->State)
    {
        case neoRADIO2state_Disconnected:
            break;
        case neoRADIO2state_ConnectedWaitIdentHeader:
            result = neoRADIO2GetNewData(devInfo);
            neoRADIO2LookForIdentHeader(devInfo);
            break;
        case neoRADIO2state_ConnectedWaitIdentResponse:
            result = neoRADIO2GetNewData(devInfo);
            neoRADIO2LookForIdentResponse(devInfo);
            break;
        case neoRADIO2state_ConnectedWaitSettings:
            result = neoRADIO2GetNewData(devInfo);
            neoRADIO2LookForDeviceReports(devInfo);
            neoRADIO2ReadSettings(devInfo);
            break;
        case neoRADIO2state_Connected:
            result = neoRADIO2GetNewData(devInfo);
            if (devInfo->online)
                neoRADIO2ProcessConnectedState(devInfo);
            neoRADIO2LookForDeviceReports(devInfo);
            break;
        default:
            neoRADIO2CloseDevice(devInfo);
    }
    if (result != 0)
    {
        neoRADIO2CloseDevice(devInfo);
    }
    return result;
}

void neoRADIO2CloseDevice(neoRADIO2_DeviceInfo * devInfo)
{
    ft260CloseDevice(&devInfo->ft260Device);
    memset(devInfo, 0, sizeof(neoRADIO2_DeviceInfo));
    devInfo->State = neoRADIO2state_Disconnected;
}


int neoRADIO2SetDeviceSettings(neoRADIO2_DeviceInfo * deviceInfo, neoRADIO2_destination * dest, void * settings, int len)
{

    int device = dest->device;
    int chip = dest->chip;
    int i = 0;
    for (int i = 0; i < 8; i++)
    {
        if (chip >> i)
        {
            switch (deviceInfo->ChainList[device - 1][i].deviceType)
            {
                case NEORADIO2_DEVTYPE_TC:
                    memcpy(&deviceInfo->ChainList[device - 1][i].settings.device.tc, \
                         settings, sizeof(neoRADIO2TCframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_PWRRLY:
                    memcpy(&deviceInfo->ChainList[device - 1][i].settings.device.pwrrly, \
                        settings, sizeof(neoRADIO2PWRRLYframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_AIN:
                    memcpy(&deviceInfo->ChainList[device - 1][i].settings.device.ain, \
                        settings, sizeof(neoRADIO2AINframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_AOUT:
                    memcpy(&deviceInfo->ChainList[device - 1][i].settings.device.aout, \
                        settings, sizeof(neoRADIO2AOUTframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_DIO:
                case NEORADIO2_DEVTYPE_CANHUB:
                default:
                    return -1;
            }
        }
    }
    return neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_SETSETTINGS, dest, settings, sizeof(neoRADIO2frame_deviceSettings));
}

void neoRADIO2SetReadRate(neoRADIO2_DeviceInfo * deviceInfo, neoRADIO2frame_id id, uint16_t rate)
{
    deviceInfo->ChainList[neoRADIO2GetDeviceNum(id)][neoRADIO2GetChipNum(id)].settings.device.tc.reportRate = rate;
}

void neoRADIO2SetOnline(neoRADIO2_DeviceInfo * deviceInfo, int online)
{
    deviceInfo->online = online;
}

neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t id)
{
	int device = 0xF & (id >> 4);
	int chip = 0xF&(id);
	return deviceInfo->ChainList[device - 1][chip].deviceType;
}
