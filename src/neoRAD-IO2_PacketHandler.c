#include "neoRAD-IO2_PacketHandler.h"

int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo)
{
    uint8_t buf[2];
    neoRADIO2_destination dest;

    buf[0] = NEORADIO2_DEVTYPE_HOST; //chip
    buf[1] = 0x00; //id

    dest.device = 0xFF;
    dest.chip = 0xFF;

    if (neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_IDENTIFY, &dest, buf, 2) ==0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitIdentHeader;
        deviceInfo->maxID.byte = 0;
        memset(deviceInfo->ChainList, 0x00, sizeof(deviceInfo->ChainList));
        return 0;
    }
    else
    {
        return -1;
    }
}

int neoRADIO2GetNewData(neoRADIO2_DeviceInfo * devInfo)
{
    uint8_t rxBuf[62];
    uint8_t txBuf[59];
    int rxLen, txLen;

    rxLen = ft260ReceiveUART(&devInfo->usbDevice.ft260Device, rxBuf);
    if(rxLen > 0)
    {
        FIFO_Push(&devInfo->rxfifo, rxBuf, rxLen);
    }
    else if (rxLen < 0)
    {
        return rxLen;
    }

    txLen = FIFO_GetOneShotReadSize(&devInfo->txfifo);
    if(txLen > 0)
    {
        if(txLen > sizeof(txBuf))
            txLen = sizeof(txBuf);
        txLen = ft260TransmitUART(&devInfo->usbDevice.ft260Device, FIFO_GetReadPtr(&devInfo->txfifo), txLen);
        if(txLen < 0)
        {
            return -1;
        }
        else
        {
            FIFO_IncrementOutPtr(&devInfo->txfifo, txLen);
        }
    }

    return 0;
}

/*
Returns location of header packet in array
result is negative if not found
-1 is not found anywhere in the array
-2 we found a possible header but we did not have all the bits
 */
int neoRADIO2FindHeaderPacket(uint8_t * data, int len)
{
    int loc = 0;
    int headerfound = 0;
    neoRADIO2frame_header * header;

    while (headerfound == 0)
    {
        if (loc == len)
        {
            return -1;
        }
        if (data[loc] == 0xAA)
        {
            if ((len - loc) > sizeof(neoRADIO2frame_header))
            {
                header = (neoRADIO2frame_header *)&data[loc];
                if (header->command == NEORADIO2_COMMAND_IDENTIFY && \
                    header->dest.device == 0xFF && header->dest.chip == 0xFF && header->len == 0x02)
                {
                    if (header->checksum == neoRADIO2CalcCheckSum8((uint8_t *) header, sizeof(neoRADIO2frame_header) - 1))
                    {
                        headerfound = 1;
                    }
                }
            }
            else
            {
                //We may have Found the Packet but we do not have all of the bytes
                return -2;
            }
        }
        loc++;
    }
    return loc;
}


void neoRADIO2LookForIdentHeader(neoRADIO2_DeviceInfo * deviceInfo)
{
    int result;
    int readsize = FIFO_GetOneShotReadSize(&deviceInfo->rxfifo);

    result = neoRADIO2FindHeaderPacket(FIFO_GetReadPtr(&deviceInfo->rxfifo), readsize);

    if (result >= 0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitIdentResponse;
        FIFO_IncrementOutPtr(&deviceInfo->rxfifo, sizeof(neoRADIO2frame_header));
    }
    else if (result == -1)
    {
        FIFO_IncrementOutPtr(&deviceInfo->rxfifo, readsize);
        return;
    }

    return;
}

int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, neoRADIO2_destination * dest, uint8_t * data, int len)
{
    unsigned int txlen = sizeof(neoRADIO2frame_header) + len;
    uint8_t buf[62];

    if (len > (59 - sizeof(neoRADIO2frame_header)))
        return -1;

    if (FIFO_GetFreeSpace(&devInfo->txfifo) < txlen)
        return -1;

    buf[0] = 0xAA; //aa
    buf[1] = command; //command
    buf[2] = dest->device; //destination
    buf[3] = dest->chip; //destination
    buf[4] = len; //length
    buf[5] = neoRADIO2CalcCheckSum8(buf, 5);

    for (int i = 0; i < len; i++)
    {
        buf[i + sizeof(neoRADIO2frame_header)] = data[i];
    }

    FIFO_Push(&devInfo->txfifo, buf, txlen);
    return 0;
}

void neoRADIO2ProcessConnectedState(neoRADIO2_DeviceInfo * deviceInfo)
{
    for (unsigned int dev = 0; dev < deviceInfo->maxID.bits.device; dev++)
    {
        neoRADIO2_destination dest = {0};
        dest.device = dev + 1;


        for (unsigned int chip = 0; chip < neoRADIO2deviceNumberChips[deviceInfo->ChainList[dev][0].deviceType]; chip++)
        {
            uint64_t reportRatems = neoRADIO2GetReportRate(deviceInfo, dev, chip);
            uint64_t lastReporttime = deviceInfo->ChainList[dev][chip].lastReadTimeus;
            if (reportRatems != 0)
            {
                if ((deviceInfo->Timeus - lastReporttime) >= (reportRatems) * 1000)
                {
                    dest.chip |= (1 << chip);
                    deviceInfo->ChainList[dev][chip].lastReadTimeus = deviceInfo->Timeus;
                }
            }
        }

        if (dest.chip != 0)
        {
            neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_READDATA, &dest, NULL, 0);
        }
    }
}

int neoRADIO2SendUARTBreak(neoRADIO2_DeviceInfo * devInfo)
{
    int result;
    uint8_t buf[64];

    memset(buf, 0, sizeof(buf));
    buf[0] = GPIO_REPORT_ID;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x80;
    result = ft260ConfigureDevice(&devInfo->usbDevice.ft260Device, buf, sizeof(buf));

    if (result != 0)
        return -1;

    buf[3] = 0x80;
    return ft260ConfigureDevice(&devInfo->usbDevice.ft260Device, buf, sizeof(buf));

}
void neoRADIO2LookForDeviceReports(neoRADIO2_DeviceInfo * deviceInfo)
{
    unsigned int len = 0;
    unsigned int loc = 0;
    unsigned int readsize;
    neoRADIO2frame_DeviceReportHeader * report;
    neoRADIO2frame_header * header;
    uint8_t rxdata[128];

    deviceInfo->rxDataCount = 0;
    deviceInfo->rxHeaderCount = 0;
    len = FIFO_GetCount(&deviceInfo->rxfifo);
    if(len > sizeof(rxdata))
    {
        len = sizeof(rxdata);
    }

    if (len == 0)
        return;

    if (len < sizeof(neoRADIO2frame_DeviceReportHeader))
        return;

    if (len > 64)
        len = 64;

    readsize = FIFO_GetOneShotReadSize(&deviceInfo->rxfifo);
    memcpy(rxdata, FIFO_GetReadPtr(&deviceInfo->rxfifo), readsize);

    if (readsize < len)
    {
        memcpy(&rxdata[readsize], deviceInfo->rxfifo.ptr, len - readsize);
    }
    while ((loc + sizeof(neoRADIO2frame_DeviceReportHeader)) < len)
    {
        if (rxdata[loc] == 0x55)
        {
            report = (neoRADIO2frame_DeviceReportHeader*)&rxdata[loc];

            if ((report->len + sizeof(neoRADIO2frame_DeviceReportHeader)) > (len - loc))
            {
                return;
            }

            int dataloc = sizeof(neoRADIO2frame_DeviceReportHeader) + loc;
            if (neoRADIO2CalcCheckSum8(&rxdata[dataloc], report->len - 1) == rxdata[dataloc + report->len - 1])
            {
                deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.len = report->len;
                deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.id = report->id;
                deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.header55 = report->header55;

                for (int i = 0; i < report->len; i++)
                {
                    deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].data[i] = rxdata[dataloc + i];
                }
                deviceInfo->rxDataCount++;
                FIFO_IncrementOutPtr(&deviceInfo->rxfifo, (report->len + sizeof(neoRADIO2frame_DeviceReportHeader)));
                loc += (report->len + sizeof(neoRADIO2frame_DeviceReportHeader));
            }
            else
            {
                loc++;
                FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
            }
        }
        else if (rxdata[loc] == 0xAA)
        {
            if ((len - loc) >= sizeof(neoRADIO2frame_header))
            {
                header = (neoRADIO2frame_header *)&rxdata[loc];
                if (header->checksum == neoRADIO2CalcCheckSum8((uint8_t *) header, sizeof(neoRADIO2frame_header) - 1))
                {
                    if ((len - loc) >= sizeof(neoRADIO2frame_header) + header->len)
                    {
                        memcpy(&deviceInfo->rxHeaderBuffer[deviceInfo->rxHeaderCount++], header, sizeof(neoRADIO2frame_header));
                        FIFO_IncrementOutPtr(&deviceInfo->rxfifo, (sizeof(neoRADIO2frame_header) + header->len));
                        loc += (sizeof(neoRADIO2frame_header) + header->len);
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    loc++;
                    FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            loc++;
            FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
        }
    }
}

void neoRADIO2LookForIdentResponse(neoRADIO2_DeviceInfo * deviceInfo)
{
    neoRADIO2frame_identify maxID;
    neoRADIO2frame_identifyResponse response;
    uint8_t device, chip;
    if (deviceInfo->maxID.byte == 0)
    {
        if (FIFO_GetCount(&deviceInfo->rxfifo) >= sizeof(neoRADIO2frame_identify))
        {
            FIFO_Pop(&deviceInfo->rxfifo, (uint8_t *) &maxID, sizeof(maxID));
            deviceInfo->maxID.byte = maxID.id;
        }
        else
        {
            return;
        }
    }
    else
    {
        while (FIFO_GetCount(&deviceInfo->rxfifo) >= sizeof(neoRADIO2frame_identifyResponse))
        {
            FIFO_Pop(&deviceInfo->rxfifo, (uint8_t *)&response, sizeof(response));
            if (response.checksum == neoRADIO2CalcCheckSum8((uint8_t *)&response, (sizeof(response) - 1)))
            {
                chip = 0x0F & response.deviceID;
                device = (0x0F & ((response.deviceID >> 4) - 1));
                deviceInfo->ChainList[device][chip].deviceID = response.deviceID;
                deviceInfo->ChainList[device][chip].deviceType = response.deviceType;
                deviceInfo->ChainList[device][chip].serialNumber = response.serialNumber;
                deviceInfo->ChainList[device][chip].manufacture_day = response.manufacture_day;
                deviceInfo->ChainList[device][chip].manufacture_month = response.manufacture_month;
                deviceInfo->ChainList[device][chip].manufacture_year = response.manufacture_year;
                deviceInfo->ChainList[device][chip].firmwareVersion_major = response.firmwareVersion_major;
                deviceInfo->ChainList[device][chip].firmwareVersion_minor = response.firmwareVersion_minor;
                deviceInfo->ChainList[device][chip].hardwareRev_major = response.hardware_revMajor;
                deviceInfo->ChainList[device][chip].hardwareRev_minor = response.hardware_revMinor;
                deviceInfo->ChainList[device][chip].status = response.current_state;

                if (response.deviceID == 0x10)
                {
                    deviceInfo->State = neoRADIO2state_Connected;
                }
            }
        }
    }
}


void neoRADIO2SendSettingsHeader(neoRADIO2_DeviceInfo * deviceInfo)
{
    for (unsigned int dev = 0; dev < deviceInfo->maxID.bits.device; dev++)
    {
        neoRADIO2_destination dest;
        dest.device = dev + 1;
        dest.chip = 0xFF;
        neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_READSETTINGS, &dest, NULL, 0);
    }

    for (unsigned int dev = 0; dev < deviceInfo->maxID.bits.device; dev++)
    {
        for (int unsigned chip = 0; chip < neoRADIO2deviceNumberChips[deviceInfo->ChainList[dev][0].deviceType]; chip++)
        {
            deviceInfo->ChainList[dev][chip].settingsValid = 0;
        }
    }
    deviceInfo->State = neoRADIO2state_ConnectedWaitSettings;
}


void neoRADIO2ReadSettings(neoRADIO2_DeviceInfo * deviceInfo)
{
    if (deviceInfo->rxDataCount > 0)
    {
        for (int i = 0; i < deviceInfo->rxDataCount; i++)
        {
            int device = getUpperNibble(deviceInfo->rxDataBuffer[i].header.id) - 1;
            int chip = getLowerNibble(deviceInfo->rxDataBuffer[i].header.id);

            deviceInfo->ChainList[device][chip].settingsValid = 1;
            switch(neoRADIO2GetGetDeviceType(deviceInfo, deviceInfo->rxDataBuffer[i].header.id))
            {
                case NEORADIO2_DEVTYPE_TC:
                    memcpy(&deviceInfo->ChainList[device][chip].settings.device.tc, \
                     deviceInfo->rxDataBuffer[i].data, sizeof(neoRADIO2TCframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_PWRRLY:
                    memcpy(&deviceInfo->ChainList[device][chip].settings.device.pwrrly, \
                    deviceInfo->rxDataBuffer[i].data, sizeof(neoRADIO2PWRRLYframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_AIN:
                    memcpy(&deviceInfo->ChainList[device][chip].settings.device.ain, \
                    deviceInfo->rxDataBuffer[i].data, sizeof(neoRADIO2AINframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_AOUT:
                    memcpy(&deviceInfo->ChainList[device][chip].settings.device.aout, \
                    deviceInfo->rxDataBuffer[i].data, sizeof(neoRADIO2AOUTframe_deviceSettings));
                    break;
                case NEORADIO2_DEVTYPE_DIO:
                case NEORADIO2_DEVTYPE_CANHUB:
                default:
                    return;
            }
        }
        for (unsigned int dev = 0; dev < deviceInfo->maxID.bits.device; dev++)
        {
            for (int unsigned chip = 0; chip < neoRADIO2deviceNumberChips[deviceInfo->ChainList[dev][0].deviceType]; chip++)
            {
                if (deviceInfo->ChainList[dev][chip].settingsValid == 0)
                    if(deviceInfo->ChainList[dev][chip].status != NEORADIO2STATE_INBOOTLOADER)
                        return;
            }
        }
        deviceInfo->rxDataCount = 0;
        deviceInfo->State = neoRADIO2state_Connected;
    }

}

uint8_t neoRADIO2CalcCheckSum8(uint8_t * data, int len)
{
    uint8_t sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}
uint16_t neoRADIO2CalcCheckSum16(uint16_t * data, int len)
{
    uint16_t sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}
unsigned int neoRADIO2GetReportRate(neoRADIO2_DeviceInfo * deviceInfo, int device, int chip)
{
    switch (deviceInfo->ChainList[device][chip].deviceType)
    {
        case NEORADIO2_DEVTYPE_TC:
            return deviceInfo->ChainList[device][chip].settings.device.tc.reportRate;
        case NEORADIO2_DEVTYPE_PWRRLY:
            return deviceInfo->ChainList[device][chip].settings.device.pwrrly.reportRate;
        case NEORADIO2_DEVTYPE_AIN:
            return deviceInfo->ChainList[device][chip].settings.device.ain.reportRate;
        case NEORADIO2_DEVTYPE_AOUT:
            return deviceInfo->ChainList[device][chip].settings.device.aout.reportRate;
        case NEORADIO2_DEVTYPE_DIO:
        case NEORADIO2_DEVTYPE_CANHUB:
        default:
            return 0;
    }
}

int neoRADIO2GetDeviceNum(neoRADIO2frame_id id)
{
    return (id.bits.device - 1);
}
int neoRADIO2GetChipNum(neoRADIO2frame_id id)
{
    return id.bits.chip;
}

