#pragma once
#include "ft260.h"
#include "stdint.h"
#include "neoRAD-IO2-TC.h"
#include "neoRADIO2_frames.h"
#include "fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define VENDOR_ID 0x093C
#define PRODUCT_ID 0x1300
#else
#define VENDOR_ID 0x0403
#define PRODUCT_ID 0x6030
#endif

#define NEORADIO2_RX_BUFFER_SIZE 256
#define NEORADIO2_TX_BUFFER_SIZE 256
#define NEORADIO2_RX_PACKET_BUFFER_SIZE 14
#define NEORADIO2_MAX_SUPPORTED_USB_DEVICES 8
#define NEORADIO2_RX_FRAME_BUFFER_SIZE 5

typedef struct _neoRADIO2_ChipInfo {
    uint32_t	serialNumber;
    uint16_t	manufacture_year;
    uint8_t	    manufacture_day;
    uint8_t 	manufacture_month;
    uint8_t     deviceID;
    uint8_t     deviceType;
    uint8_t		firmwareVersion_major;
    uint8_t		firmwareVersion_minor;
    uint8_t		hardwareRev_major;
    uint8_t		hardwareRev_minor;
    uint8_t     status;
    uint8_t     settingsValid;
    uint64_t    lastReadTimeus;
    neoRADIO2frame_deviceSettings settings;
} neoRADIO2_ChipInfo;

typedef enum _neoRADIO2_RunStates {
    neoRADIO2state_Disconnected,
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
    neoRADIO2frame_id maxID;
    uint64_t Timeus;
    int online;
    uint8_t rxbuffer[NEORADIO2_RX_BUFFER_SIZE];
    uint8_t txbuffer[NEORADIO2_TX_BUFFER_SIZE];
    fifo_t rxfifo;
    fifo_t txfifo;
    neoRADIO2_ReceiveData rxDataBuffer[NEORADIO2_RX_PACKET_BUFFER_SIZE];
    neoRADIO2frame_header rxHeaderBuffer[NEORADIO2_RX_FRAME_BUFFER_SIZE];
    int rxDataCount;
    int rxHeaderCount;
} neoRADIO2_DeviceInfo;

extern const unsigned int neoRADIO2deviceNumberChips[];

int neoRADIO2FindDevices(neoRADIO2_USBDevice usbDevices[], const unsigned int size);
void neoRADIO2CloseDevice(neoRADIO2_DeviceInfo * devInfo);
int neoRADIO2ConnectDevice(neoRADIO2_DeviceInfo * devInfo);
int neoRADIO2ProcessIncomingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus);
int neoRADIO2SetDeviceSettings(neoRADIO2_DeviceInfo * deviceInfo, neoRADIO2_destination * dest, void * settings, int len);
void neoRADIO2SetOnline(neoRADIO2_DeviceInfo * deviceInfo, int online);
int neoRADIO2RequestSettings(neoRADIO2_DeviceInfo * deviceInfo);
unsigned int neoRADIO2GetReportRate(neoRADIO2_DeviceInfo * deviceInfo, int device, int chip);
void neoRADIO2SerialToString(char * string, uint32_t serial);

#ifdef __cplusplus
}
#endif
