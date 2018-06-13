#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t neoRADIO2TCCalcDataRate(uint16_t reportRate);

#define NEORADIO2TC_ENABLEREAD_DISABLE  0x00
#define NEORADIO2TC_ENABLEREAD_TC     0x01
#define NEORADIO2TC_ENABLEREAD_CJ     0x02
#define NEORADIO2TC_ENABLEREAD_TCCJ   0x03

#define DATARATE_8SPS	0
#define DATARATE_16SPS	1
#define DATARATE_32SPS	2
#define DATARATE_64SPS	3
#define DATARATE_128SPS	4
#define DATARATE_250SPS	5
#define DATARATE_475SPS	6
#define DATARATE_860SPS	7


#ifdef __cplusplus
}
#endif
