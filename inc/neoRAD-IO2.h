#pragma once
#include "ft260.h"
#include "stdint.h"
#include "neoRAD-IO2-TC.h"
#include "neoRAD-IO2-AIN.h"
#include "neoRAD-IO2-AOUT.h"
#include "neoRAD-IO2-PWRRLY.h"
#include "neoRADIO2_frames.h"
#include "fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#define VENDOR_ID 0x093C
#define PRODUCT_ID 0x1300
#else
#define VENDOR_ID 0x0403
#define PRODUCT_ID 0x6030
#endif

#define NEORADIO2_RX_BUFFER_SIZE 256
#define NEORADIO2_TX_BUFFER_SIZE 256
#define NEORADIO2_RX_PACKET_BUFFER_SIZE 10
#define NEORADIO2_MAX_SUPPORTED_USB_DEVICES 8

typedef struct _neoRADIO2_ChipInfo {
    uint32_t	serialNumber;
    uint16_t	manufacture_year;
    uint8_t	    manufacture_day;
    uint8_t 	manufacture_month;
    uint8_t     deviceType;
    uint8_t		firmwareVersion_major;
    uint8_t		firmwareVersion_minor;
    uint8_t		hardwareRev_major;
    uint8_t		hardwareRev_minor;
    uint8_t     status;
    uint8_t     settingsValid;
    uint64_t    lastReadTimeus;
    neoRADIO2_deviceSettings settings;
} neoRADIO2_ChipInfo;

typedef enum _neoRADIO2_RunStates {
    neoRADIO2state_Disconnected,
    neoRADIO2state_ConnectedWaitForAppStart,
    neoRADIO2state_ConnectedWaitIdentHeader,
    neoRADIO2state_ConnectedWaitIdentResponse,
    neoRADIO2state_ConnectedWaitSettings,
    neoRADIO2state_Connected,
} neoRADIO2_RunStates;

typedef struct _neoRADIO2_USBDevice 
{
	ft260_device ft260Device;
	char serial[6];
} neoRADIO2_USBDevice;

typedef struct _neoRADIO2_DeviceInfo {
	neoRADIO2_USBDevice usbDevice;
    neoRADIO2_ChipInfo ChainList[8][8];
    neoRADIO2_RunStates State;
    uint8_t LastDevice;
    uint8_t LastBank;
    uint64_t Timeus;
    uint8_t isOnline;
    uint8_t rxbuffer[NEORADIO2_RX_BUFFER_SIZE];
    uint8_t txbuffer[NEORADIO2_TX_BUFFER_SIZE];
    fifo_t rxfifo;
    fifo_t txfifo;
    neoRADIO2frame rxDataBuffer[NEORADIO2_RX_PACKET_BUFFER_SIZE];
    uint8_t rxDataCount;
} neoRADIO2_DeviceInfo;

extern const unsigned int neoRADIO2GetDeviceNumberOfBanks[];

int neoRADIO2FindDevices(neoRADIO2_USBDevice usbDevices[], const unsigned int size);
void neoRADIO2CloseDevice(neoRADIO2_DeviceInfo * devInfo);
int neoRADIO2ConnectDevice(neoRADIO2_DeviceInfo * devInfo);
int neoRADIO2ProcessIncomingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus);
int neoRADIO2SetDeviceSettings(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank, neoRADIO2_deviceSettings * settings);
void neoRADIO2SetOnline(neoRADIO2_DeviceInfo * deviceInfo, int online);
int neoRADIO2RequestSettings(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SettingsValid(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SerialToString(char * string, uint32_t serial);
neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank);

#ifdef __cplusplus
}
#endif
