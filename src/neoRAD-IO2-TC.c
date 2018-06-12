#include "neoRAD-IO2-TC.h"

uint8_t neoRADIO2TCCalcDataRate(uint16_t reportRate)
{
    if (reportRate < 10)
        return DATARATE_250SPS;
    else if (reportRate < 20)
        return DATARATE_128SPS;
    else if (reportRate < 32)
        return DATARATE_64SPS;
    else if (reportRate < 64)
        return DATARATE_32SPS;
    else if (reportRate < 128)
        return DATARATE_16SPS;
    else
        return DATARATE_8SPS;
}
