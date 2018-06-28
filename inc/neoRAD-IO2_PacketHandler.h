#pragma once
#include "neoRAD-IO2.h"

#ifdef __cplusplus
extern "C" {
#endif

int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SendJumpToApp(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2GetNewData(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2FindHeaderPacket(uint8_t * data, int len);
void neoRADIO2LookForIdentResponse(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2LookForIdentHeader(neoRADIO2_DeviceInfo * devInfo);
void neoRADIO2ProcessConnectedState(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, neoRADIO2_destination * dest, uint8_t * data, int len);
int neoRADIO2SendUARTBreak(neoRADIO2_DeviceInfo * devInfo);
void neoRADIO2LookForDeviceReports(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2LookForStartHeader(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SendSettingsHeader(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2ReadSettings(neoRADIO2_DeviceInfo * deviceInfo);

int neoRADIO2GetDeviceNum(neoRADIO2frame_id id);
int neoRADIO2GetChipNum(neoRADIO2frame_id id);
neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t id);
uint8_t neoRADIO2CalcCheckSum8(uint8_t * data, int len);
uint16_t neoRADIO2CalcCheckSum16(uint16_t * data, int len);


#define getUpperNibble(x) (0xF & (x >> 4))
#define getLowerNibble(x) (0xF & (x))

#ifdef __cplusplus
}
#endif
