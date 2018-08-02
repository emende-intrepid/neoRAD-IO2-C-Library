#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _neoRADIO2AOUT_ChannelSetting{
	uint8_t enable;
	uint16_t default_value;
} neoRADIO2AOUT_ChannelSetting;

#ifdef __cplusplus
}
#endif
