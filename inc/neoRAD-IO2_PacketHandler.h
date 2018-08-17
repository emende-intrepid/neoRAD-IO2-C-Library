#pragma once
#include "neoRAD-IO2.h"

#ifdef __cplusplus
extern "C" {
#endif

int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SendJumpToApp(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2GetNewData(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2LookForIdentResponse(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2ProcessConnectedState(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, uint8_t device, uint8_t bank, uint8_t * data, uint8_t len);
int neoRADIO2SendUARTBreak(neoRADIO2_DeviceInfo * devInfo);
void neoRADIO2LookForDevicePackets(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2LookForStartHeader(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SendSettingsHeader(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2ReadSettings(neoRADIO2_DeviceInfo * deviceInfo);
uint8_t neoRADIO2CalcCRC8(uint8_t * data, int len);
void neoRADIO2CRC8_Init(void);
neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank);
unsigned int neoRADIO2GetReportRate(neoRADIO2_DeviceInfo * deviceInfo, int device, int bank);

#define CRC_POLYNIMIAL 0x07

#ifdef __cplusplus
}
#endif
