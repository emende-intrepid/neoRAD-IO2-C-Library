#include "neoRAD-IO2.h"
#include "neoRAD-IO2_PacketHandler.h"
#include <string.h>
#include <stdio.h>

/*
* Function: neoRADIO2FindDevices
* ----------------------------
*   Finds neoRAD-IO2 USB Devices
*
*   usbDevices: Array of neoRADIO2_USBDevice which will be filled in by function
*   size: size of usbDevices
*
*   returns: Number of devices found. 0 if no devices found.
*/
const unsigned int neoRADIO2GetDeviceNumberOfBanks[] = {8, 8, 1, 8, 8, 1};

int neoRADIO2FindDevices(neoRADIO2_USBDevice usbDevices[], const unsigned int size)
{
    int result = 0;
    ft260_device  devices[NEORADIO2_MAX_SUPPORTED_USB_DEVICES];
    result = ft260FindDevices(devices, NEORADIO2_MAX_SUPPORTED_USB_DEVICES, VENDOR_ID, PRODUCT_ID);
    if (result >= 0)
    {
		for (int i = 0; i < result; i++)
        {
			memcpy(&usbDevices[i].ft260Device, &devices[i], sizeof(ft260_device));
            for (int j = 0; j < 6; j++)
            {
                usbDevices[i].serial[j] = (char)devices->SerialNumber[j];
            }
        }
        return result;
    }
    return -1;
}


int neoRADIO2ConnectDevice(neoRADIO2_DeviceInfo * devInfo)
{
    int result;
    uint8_t buf[64];

    FIFO_Init(&devInfo->rxfifo, devInfo->rxbuffer, NEORADIO2_RX_BUFFER_SIZE);
    FIFO_Init(&devInfo->txfifo, devInfo->txbuffer, NEORADIO2_TX_BUFFER_SIZE);
    result = ft260OpenDevice(&devInfo->usbDevice.ft260Device);
    devInfo->LastBank = 0;

    if (result == 0)
    {
        //Setup FT260 Clock
        buf[0] = SYSTEM_SETTING_REPORT_ID;
        buf[1] = SET_CLOCK;
        buf[2] = CLOCK_CTRL_48;
		result = ft260ConfigureDevice(&devInfo->usbDevice.ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        //GPIOA TX LED
        memset(buf, 0, sizeof(buf));
        buf[0] = SYSTEM_SETTING_REPORT_ID;
        buf[1] = 0x08;
        buf[2] = 0x04;
		result = ft260ConfigureDevice(&devInfo->usbDevice.ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        //GPIOG RX_LED
        memset(buf, 0, sizeof(buf));
        buf[0] = SYSTEM_SETTING_REPORT_ID;
        buf[1] = 0x09;
        buf[2] = 0x05;
        result = ft260ConfigureDevice(&devInfo->usbDevice.ft260Device, buf, sizeof(buf));
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
		result = ft260ConfigureDevice(&devInfo->usbDevice.ft260Device, buf, sizeof(buf));
    }
    if (result == 0)
    {
        result = ft260SetupUART(&devInfo->usbDevice.ft260Device, 250000, 1, 0);
    }

    if (result == 0)
    {
        result = neoRADIO2SendJumpToApp(devInfo);
    }
    return result;
}

//This must be called every 1 ms
int neoRADIO2ProcessIncomingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus)
{
    int result = 0;
    devInfo->Timeus += diffTimeus;
    if (devInfo->State != neoRADIO2state_Disconnected)
    {
        result = neoRADIO2GetNewData(devInfo);
        if (result != 0)
            return result;
        neoRADIO2LookForDevicePackets(devInfo);
    }
    switch (devInfo->State)
    {
        case neoRADIO2state_Disconnected:
            break;
        case neoRADIO2state_ConnectedWaitForAppStart:
            neoRADIO2LookForStartHeader(devInfo);
            break;
        case neoRADIO2state_ConnectedWaitIdentHeader:
            neoRADIO2LookForIdentHeader(devInfo);
            break;
        case neoRADIO2state_ConnectedWaitIdentResponse:
            neoRADIO2LookForIdentResponse(devInfo);
            break;
        case neoRADIO2state_ConnectedWaitSettings:
            neoRADIO2ReadSettings(devInfo);
            break;
        case neoRADIO2state_Connected:
            if (devInfo->isOnline)
                neoRADIO2ProcessConnectedState(devInfo);
            break;
        default:
            neoRADIO2CloseDevice(devInfo);
    }
    return result;
}

void neoRADIO2CloseDevice(neoRADIO2_DeviceInfo * devInfo)
{
    ft260CloseDevice(&devInfo->usbDevice.ft260Device);
    memset(devInfo, 0, sizeof(neoRADIO2_DeviceInfo));
    devInfo->State = neoRADIO2state_Disconnected;
}


int neoRADIO2SetDeviceSettings(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank, neoRADIO2_deviceSettings * settings)
{
    for (int i = 0; i < 8; i++)
    {
        if (0x1 & bank >> i)
        {
            memcpy(&deviceInfo->ChainList[device][i].settings, settings, sizeof(neoRADIO2_deviceSettings));
            deviceInfo->ChainList[device][i].settingsValid = 0;
        }
    }

    return neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_WRITE_SETTINGS, device, bank, (uint8_t *)settings, sizeof(neoRADIO2_deviceSettings));
}

void neoRADIO2SetOnline(neoRADIO2_DeviceInfo * deviceInfo, int online)
{
    deviceInfo->isOnline = online;
}

int neoRADIO2RequestSettings(neoRADIO2_DeviceInfo * deviceInfo)
{
    if (deviceInfo->State != neoRADIO2state_Connected)
    {
        return -1;
    }

    neoRADIO2SendSettingsHeader(deviceInfo);
    return 0;
}

int neoRADIO2SettingsValid(neoRADIO2_DeviceInfo * deviceInfo)
{
    for (unsigned int dev = 0; dev < deviceInfo->LastDevice; dev++)
    {
        for (unsigned int bank = 0; bank < neoRADIO2GetDeviceNumberOfBanks[deviceInfo->ChainList[dev][0].deviceType]; bank++)
        {
            if (deviceInfo->ChainList[dev][bank].settingsValid == 0)
                if(deviceInfo->ChainList[dev][bank].status != NEORADIO2STATE_INBOOTLOADER) //bootloader cannot read settigns
                    return 0;
        }
    }
    return 1;
}



neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank)
{
	return deviceInfo->ChainList[device][bank].deviceType;
}

void neoRADIO2SerialToString(char * string, uint32_t serial)
{
	for (int i = 5; i >= 0; i--)
	{
		string[i] = serial % 36;
		serial /= 36;

		if (string[i] < 9)
			string[i] += 0x30;
		else if (string[i] < 35)
			string[i] += 0x37;
		else
			string[i] = ' ';
	}
}