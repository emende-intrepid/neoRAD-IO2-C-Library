#pragma once
#ifndef RADIO2_EMBEDDED_LIB
#include "ft260.h"
#endif /* RADIO2_EMBEDDED_LIB */
#include "stdint.h"
#include "radio2_frames.h"
#include "fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

//#define SUPPORT_DEBUGGING

#ifdef SUPPORT_DEBUGGING
#ifdef RADIO2_EMBEDDED_LIB
#define DEBUG_PRINTF(fmt, ...) debug_printf(fmt, ...)
#else
#define DEBUG_PRINTF(fmt, ...) printf(fmt, ...)
#endif
#else
#define DEBUG_PRINTF(fmt, ...)
#endif

#if 1
#define VENDOR_ID 0x093C
#define PRODUCT_ID 0x1300
#else
#define VENDOR_ID 0x0403
#define PRODUCT_ID 0x6030
#endif

#define NEORADIO2_RX_BUFFER_SIZE 4096
#define NEORADIO2_TX_BUFFER_SIZE 4096
#define NEORADIO2_RX_PACKET_BUFFER_SIZE (NEORADIO2_RX_BUFFER_SIZE / 6) + 1
#define NEORADIO2_MAX_SUPPORTED_USB_DEVICES 8

typedef enum _neoRADIO2_SettingsStates {
	neoRADIO2Settings_NeedsRead = 0,
	neoRADIO2Settings_NeedsWrite = 1,
	neoRADIO2Settings_Valid = 2,
} neoRADIO2_SettingsStates;

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
	uint8_t     settingsState;
    uint64_t    lastReadTimeus;
	neoRADIO2_settings settings;
} PACKED neoRADIO2_ChipInfo;

typedef enum _neoRADIO2_RunStates {
    neoRADIO2state_Disconnected,
    neoRADIO2state_ConnectedWaitForAppStart,
    neoRADIO2state_ConnectedWaitIdentResponse,
	neoRADIO2state_ConnectedWaitReadSettings,
	neoRADIO2state_ConnectedWaitWriteSettings,
    neoRADIO2state_Connected,
} neoRADIO2_RunStates;

#ifndef RADIO2_EMBEDDED_LIB
typedef struct _neoRADIO2_USBDevice
{
	ft260_device ft260Device;
	char serial[6];
} PACKED neoRADIO2_USBDevice;
#endif /* RADIO2_EMBEDDED_LIB */

typedef struct _neoRADIO2_DeviceInfo {
#ifndef RADIO2_EMBEDDED_LIB
	neoRADIO2_USBDevice usbDevice;
#endif /* RADIO2_EMBEDDED_LIB */
    neoRADIO2_ChipInfo ChainList[8][8];
    neoRADIO2_RunStates State;
    uint8_t LastDevice;
    uint8_t LastBank;
    uint64_t Timeus;
    uint8_t isOnline;
    uint8_t readCount;
    uint8_t rxbuffer[NEORADIO2_RX_BUFFER_SIZE];
    uint8_t txbuffer[NEORADIO2_TX_BUFFER_SIZE];
    fifo_t rxfifo;
    fifo_t txfifo;
    neoRADIO2frame rxDataBuffer[NEORADIO2_RX_PACKET_BUFFER_SIZE];
    uint8_t rxDataCount;
} PACKED neoRADIO2_DeviceInfo;

extern const unsigned int neoRADIO2GetDeviceNumberOfBanks[];

#ifndef RADIO2_EMBEDDED_LIB
int neoRADIO2FindDevices(neoRADIO2_USBDevice usbDevices[], const unsigned int size);
int neoRADIO2SetupFT260(neoRADIO2_DeviceInfo * devInfo);
void neoRADIO2CloseDevice(neoRADIO2_DeviceInfo * devInfo);
int neoRADIO2ConnectDevice(neoRADIO2_DeviceInfo * devInfo);
#endif /* RADIO2_EMBEDDED_LIB */
int neoRADIO2ProcessIncomingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus);
int neoRADIO2SetSettings(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SetOnline(neoRADIO2_DeviceInfo * deviceInfo, int online);
int neoRADIO2RequestSettings(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SettingsValid(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SerialToString(char * string, uint32_t serial);
neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank);

#ifdef _MSC_VER
#pragma pack(pop)
#undef PACKED
#else
#undef PACKED
#endif

#ifdef __cplusplus
}
#endif
