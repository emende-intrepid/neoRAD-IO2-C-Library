#include "neoRAD-IO2_PacketHandler.h"
#include "stdio.h"
uint8_t neoRADIO2CRC8Table[256];

int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo)
{
    uint8_t buf[3];

    buf[0] = NEORADIO2_DEVTYPE_HOST; //chip
    buf[1] = 0xFF; //id
	buf[2] = 0xFF; //id

    if (neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_IDENTIFY, 0xFF, 0xFF, buf, sizeof(buf)) == 0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitIdentResponse;
        deviceInfo->LastBank = 0;
	    deviceInfo->LastDevice = 0;
        memset(deviceInfo->ChainList, 0x00, sizeof(deviceInfo->ChainList));
        return 0;
    }
    else
    {
        return -1;
    }
}

int neoRADIO2SendJumpToApp(neoRADIO2_DeviceInfo * deviceInfo)
{
    if (neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_START, 0xFF, 0xFF, NULL, 0) == 0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitForAppStart;
        return 0;
    }
    else
    {
        return -1;
    }
}

#ifndef RADIO2_EMBEDDED_LIB
int neoRADIO2GetNewData(neoRADIO2_DeviceInfo * devInfo)
{
    uint8_t rxBuf[62];
    uint8_t txBuf[59];
    int rxLen, txLen;

    rxLen = ft260ReceiveUART(&devInfo->usbDevice.ft260Device, rxBuf);

    while (rxLen > 0)
    {
    	if (rxLen > FIFO_GetFreeSpace(&devInfo->rxfifo))
	    {
		    FIFO_Push(&devInfo->rxfifo, rxBuf, FIFO_GetFreeSpace(&devInfo->rxfifo));
    		rxLen = 0;
	    }
	    else
	    {
		    FIFO_Push(&devInfo->rxfifo, rxBuf, rxLen);
		    rxLen = ft260ReceiveUART(&devInfo->usbDevice.ft260Device, rxBuf);
	    }
    }
    if (rxLen < 0)
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
#else
int neoRADIO2GetNewData(neoRADIO2_DeviceInfo* devInfo)
{
	uint8_t rxBuf[62];
	uint8_t txBuf[59];
	int rxLen, txLen;

	rxLen = RADIO2_Interface_Pop(rxBuf, sizeof(rxBuf));

	while (rxLen > 0)
	{
		if (rxLen > FIFO_GetFreeSpace(&devInfo->rxfifo))
		{
			FIFO_Push(&devInfo->rxfifo, rxBuf, FIFO_GetFreeSpace(&devInfo->rxfifo));
			rxLen = 0;
		}
		else
		{
			FIFO_Push(&devInfo->rxfifo, rxBuf, rxLen);
			rxLen = RADIO2_Interface_Pop(rxBuf, sizeof(rxBuf));
		}
	}
	if (rxLen < 0)
	{
		return rxLen;
	}

	txLen = FIFO_GetOneShotReadSize(&devInfo->txfifo);
	if (txLen > 0)
	{
		if (txLen > sizeof(txBuf))
			txLen = sizeof(txBuf);
		txLen = RADIO2_Interface_Push(FIFO_GetReadPtr(&devInfo->txfifo), txLen);
		if (txLen < 0)
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
#endif /* RADIO2_EMBEDDED_LIB */

int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, uint8_t device, uint8_t bank, uint8_t * data, uint8_t len)
{
    unsigned int txlen = sizeof(neoRADIO2frame_header) + len + 1; //extra byte for crc
    uint8_t buf[128];
	neoRADIO2frame_header * header = (neoRADIO2frame_header *)buf;

	if (len > (sizeof(buf) - sizeof(neoRADIO2frame_header)))
        return -1;

    if (FIFO_GetFreeSpace(&devInfo->txfifo) < txlen)
        return -1;

    header->start_of_frame = 0xAA;
    header->command_status = command;
    header->device = device;
    header->bank = bank;
    header->len = len;

    for (int i = 0; i < len; i++)
    {
        buf[i + sizeof(neoRADIO2frame_header)] = data[i];
    }
	buf[txlen - 1] = neoRADIO2CalcCRC8(buf, txlen - 1);

    FIFO_Push(&devInfo->txfifo, buf, txlen);
    return 0;
}

void neoRADIO2ProcessConnectedState(neoRADIO2_DeviceInfo * deviceInfo)
{

    for (unsigned int dev = 0; dev <= deviceInfo->LastDevice; dev++)
    {
    	uint8_t txBank = 0;
        for (unsigned int bank = 0; bank < neoRADIO2GetDeviceNumberOfBanks[deviceInfo->ChainList[dev][0].deviceType]; bank++)
        {
            uint64_t reportRatems = deviceInfo->ChainList[dev][bank].settings.config.poll_rate_ms;
            uint64_t lastReportTime = deviceInfo->ChainList[dev][bank].lastReadTimeus;
            if (reportRatems != 0)
            {
                if ((deviceInfo->Timeus - lastReportTime) >= (reportRatems) * 1000)
                {
	                txBank |= (1 << bank);
                    deviceInfo->ChainList[dev][bank].lastReadTimeus = deviceInfo->Timeus;
                }
            }
        }

        if (txBank != 0)
        {
            neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_READ_DATA, dev, txBank, NULL, 0);
        }
    }
}

#ifndef RADIO2_EMBEDDED_LIB
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
#endif /* RADIO2_EMBEDDED_LIB */

void neoRADIO2LookForDevicePackets(neoRADIO2_DeviceInfo * deviceInfo)
{
    unsigned int len = 0;
    unsigned int loc = 0;
    unsigned int readsize;
    static const unsigned int minPacketSize = sizeof(neoRADIO2frame_header) + 1; //extra byte for CRC
	neoRADIO2frame_header  * header;
    uint8_t rxdata[NEORADIO2_RX_BUFFER_SIZE];

    deviceInfo->rxDataCount = 0;
    len = FIFO_GetCount(&deviceInfo->rxfifo);
    if(len > sizeof(rxdata))
        len = sizeof(rxdata);

    if (len == 0)
        return;

    if (len < minPacketSize)
        return;

    readsize = FIFO_GetOneShotReadSize(&deviceInfo->rxfifo);
    memcpy(rxdata, FIFO_GetReadPtr(&deviceInfo->rxfifo), readsize);

    if (readsize < len)
    {
        memcpy(&rxdata[readsize], deviceInfo->rxfifo.ptr, len - readsize);
    }
    while ((loc + minPacketSize) <= len)
    {
        if (rxdata[loc] == 0x55 || rxdata[loc] == 0xAA)
        {
	        header = (neoRADIO2frame_header *)&rxdata[loc];

            if ((header->len + minPacketSize) > (len - loc))
            {
                return;
            }

	        unsigned int dataloc = sizeof(neoRADIO2frame_header) + loc;
            if (neoRADIO2CalcCRC8(&rxdata[loc], sizeof(neoRADIO2frame_header) + header->len) == rxdata[dataloc + header->len])
            {
                deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.len = header->len;
	            deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.device = header->device;
	            deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.bank = header->bank;
	            deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.command_status = header->command_status;
                deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.start_of_frame = header->start_of_frame;

                for (int i = 0; i < header->len; i++)
                {
                    deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].data[i] = rxdata[dataloc + i];
                }
                deviceInfo->rxDataCount++;
                FIFO_IncrementOutPtr(&deviceInfo->rxfifo, (header->len + minPacketSize));
                loc += (header->len + minPacketSize);
            }
            else
            {
	            DEBUG_PRINTF("CRC ERROR got (0x%x), expected (%X)\n", rxdata[dataloc + header->len], neoRADIO2CalcCRC8(&rxdata[loc], sizeof(neoRADIO2frame_header) + header->len));
                loc++;
                FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
            }
        }
        else
        {
	        DEBUG_PRINTF("DROPPED BYTE (0x%X)\n", rxdata[loc]);
            loc++;
            FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
        }
    }
}

void neoRADIO2LookForIdentResponse(neoRADIO2_DeviceInfo * deviceInfo)
{
	for (int i = 0; i < deviceInfo->rxDataCount; i++)
	{
		if(deviceInfo->rxDataBuffer[i].header.start_of_frame == 0x55 && \
			deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_STATUS_IDENTIFY)
		{
			uint8_t dev = deviceInfo->rxDataBuffer[i].header.device;
			uint8_t bank = deviceInfo->rxDataBuffer[i].header.bank;

			neoRADIO2frame_identifyResponse  * response = (neoRADIO2frame_identifyResponse *)deviceInfo->rxDataBuffer[i].data;

			if (deviceInfo->LastDevice == 0 && deviceInfo->LastBank == 0)
			{
				deviceInfo->LastDevice = dev;
				deviceInfo->LastBank = bank;
			}
			deviceInfo->ChainList[dev][bank].deviceType = response->device_type;
			deviceInfo->ChainList[dev][bank].serialNumber = response->serial_number;
			deviceInfo->ChainList[dev][bank].manufacture_day = response->manufacture_day;
			deviceInfo->ChainList[dev][bank].manufacture_month = response->manufacture_month;
			deviceInfo->ChainList[dev][bank].manufacture_year = response->manufacture_year;
			deviceInfo->ChainList[dev][bank].firmwareVersion_major = response->firmwareVersion_major;
			deviceInfo->ChainList[dev][bank].firmwareVersion_minor = response->firmwareVersion_minor;
			deviceInfo->ChainList[dev][bank].hardwareRev_major = response->hardware_revMajor;
			deviceInfo->ChainList[dev][bank].hardwareRev_minor = response->hardware_revMinor;
			deviceInfo->ChainList[dev][bank].status = response->current_state;
			deviceInfo->ChainList[dev][bank].manufacture_day = response->manufacture_day;
			deviceInfo->ChainList[dev][bank].manufacture_month = response->manufacture_month;
			deviceInfo->ChainList[dev][bank].manufacture_year = response->manufacture_year;
			deviceInfo->ChainList[dev][bank].hardwareRev_major = response->hardware_revMajor;
			deviceInfo->ChainList[dev][bank].hardwareRev_minor = response->hardware_revMinor;

			if (dev == 0 && bank == 0)
			{
				deviceInfo->State = neoRADIO2state_Connected;
			}
		}
	}
}

void neoRADIO2LookForStartHeader(neoRADIO2_DeviceInfo * deviceInfo)
{
    for (int i = 0; i < deviceInfo->rxDataCount; i++)
    {
	    if(deviceInfo->rxDataBuffer[i].header.start_of_frame == 0xAA && \
			deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_COMMAND_START)
	    {
		    if (neoRADIO2IdentifyChain(deviceInfo) != 0)
		    {
			    deviceInfo->State = neoRADIO2state_Disconnected;
		    }
	    }
    }
}

void neoRADIO2StartReadSettings(neoRADIO2_DeviceInfo * deviceInfo)
{
	uint8_t buf[1] = {0};
	for (unsigned int dev = 0; dev <= deviceInfo->LastDevice; dev++)
	{
		for (int unsigned bank = 0; bank < neoRADIO2GetDeviceNumberOfBanks[deviceInfo->ChainList[dev][0].deviceType]; bank++)
		{
			deviceInfo->ChainList[dev][bank].settingsState = neoRADIO2Settings_NeedsRead;
		}
	}

	neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_READ_SETTINGS, 0, 0x1, buf, sizeof(buf));

	deviceInfo->State = neoRADIO2state_ConnectedWaitReadSettings;
}

static const uint32_t SETTINGS_NUMPARTS = sizeof(neoRADIO2_settings)/NEORADIO2_SETTINGS_PARTSIZE;
static const uint32_t SETTINGS_LASTPARTSIZE = sizeof(neoRADIO2_settings) % NEORADIO2_SETTINGS_PARTSIZE;

void neoRADIO2ReadSettings(neoRADIO2_DeviceInfo * deviceInfo)
{
	uint8_t buf[1];
    for (int i = 0; i < deviceInfo->rxDataCount; i++)
    {
        if (deviceInfo->rxDataBuffer[i].header.start_of_frame == 0x55 && \
            deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_STATUS_READ_SETTINGS)
        {
            uint8_t device = deviceInfo->rxDataBuffer[i].header.device;
            uint8_t bank = deviceInfo->rxDataBuffer[i].header.bank;

            //copy new settings
            uint8_t part = deviceInfo->rxDataBuffer[i].data[0];
            uint8_t rxlen = deviceInfo->rxDataBuffer[i].header.len - 1;
            uint8_t * newSettings = (uint8_t *) &deviceInfo->ChainList[device][bank].settings;

            for (int n = 0; n < rxlen; n++)
            {
	            newSettings[NEORADIO2_SETTINGS_PARTSIZE * part + n] = deviceInfo->rxDataBuffer[i].data[n + 1];
            }

	        if (part == SETTINGS_NUMPARTS)
	        {
		        deviceInfo->ChainList[device][bank].settingsState = neoRADIO2Settings_Valid;
		        if (bank == neoRADIO2GetDeviceNumberOfBanks[deviceInfo->ChainList[device][bank].deviceType] - 1)
		        {
					if(device != deviceInfo->LastDevice)
					{
						device++;
						bank = 0;
						part = 0;
					}
					else
					{
						deviceInfo->State = neoRADIO2state_Connected;
					}

		        }
		        else
		        {
		        	bank++;
		        	part = 0;
		        }
	        }
            else
            {
            	part++;
            }

	        buf[0] = part;
	        neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_READ_SETTINGS, device, (1 << bank), buf, sizeof(buf));
        }
    }
}

void neoRADIO2StartWriteSettings(neoRADIO2_DeviceInfo * deviceInfo)
{
	neoRADIO2_SettingsPart msg;
	for (unsigned int dev = 0; dev <= deviceInfo->LastDevice; dev++)
	{
		for (int unsigned bank = 0; bank < neoRADIO2GetDeviceNumberOfBanks[deviceInfo->ChainList[dev][0].deviceType]; bank++)
		{
			if (deviceInfo->ChainList[dev][bank].settingsState == neoRADIO2Settings_NeedsWrite)
			{
				msg.part = 0;
				uint8_t * newSettings = (uint8_t *) &deviceInfo->ChainList[dev][bank].settings;
				int txlen  = NEORADIO2_SETTINGS_PARTSIZE;
				for (int i = 0; i < txlen; i++)
				{
					msg.data[i] = newSettings[i];
				}

				deviceInfo->State = neoRADIO2state_ConnectedWaitWriteSettings;
				neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_WRITE_SETTINGS, dev, (1 << bank), (uint8_t *)&msg, sizeof(msg));
				return;
			}
		}
	}

	deviceInfo->State = neoRADIO2state_Connected;
}

void neoRADIO2WriteSettings(neoRADIO2_DeviceInfo * deviceInfo)
{
	neoRADIO2_SettingsPart msg;
	for (int i = 0; i < deviceInfo->rxDataCount; i++)
	{
		if (deviceInfo->rxDataBuffer[i].header.start_of_frame == 0xAA && \
            deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_COMMAND_WRITE_SETTINGS)
		{
			uint8_t device = deviceInfo->rxDataBuffer[i].header.device;
			uint8_t bank = neoRADIO2GetBankPos(deviceInfo->rxDataBuffer[i].header.bank);

			//copy new settings
			uint8_t part = deviceInfo->rxDataBuffer[i].data[0];


			if (part == SETTINGS_NUMPARTS)
			{
				deviceInfo->ChainList[device][bank].settingsState = neoRADIO2Settings_Valid;
				neoRADIO2StartWriteSettings(deviceInfo);
			}
			else
			{
				uint8_t * newSettings = (uint8_t *) &deviceInfo->ChainList[device][bank].settings;
				part++;
				int txlen = NEORADIO2_SETTINGS_PARTSIZE;
				if (part == SETTINGS_NUMPARTS)
					txlen = SETTINGS_LASTPARTSIZE;

				msg.part = part;
				for (int n = 0; n < txlen; n++)
				{
					msg.data[n] = newSettings[NEORADIO2_SETTINGS_PARTSIZE * part + n];
				}

				neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_WRITE_SETTINGS, device, (1 << bank), (uint8_t *)&msg, txlen + 1);
			}

		}
	}
}
uint8_t neoRADIO2CalcCRC8(uint8_t * data, int len)
{
	if (neoRADIO2CRC8Table[1] != CRC_POLYNIMIAL)
		neoRADIO2CRC8_Init();
	uint8_t crc = 0;
	for (int i = 0; i < len; i++)
	{
		crc = neoRADIO2CRC8Table[crc ^ data[i]] & 0xFF;
	}
	return crc;

}
void neoRADIO2CRC8_Init(void)
{
	uint8_t crc;

	for (int i = 0; i < 256; i++)
	{
		crc = i;
		for (int j = 0; j < 8; j++)
		{
			crc = (crc << 1) ^ ((crc & 0x80) ? CRC_POLYNIMIAL : 0);
		}
		neoRADIO2CRC8Table[i] = crc & 0xFF;
	}
}

uint8_t neoRADIO2GetBankPos(uint8_t x)
{
	switch (x)
	{
		case 1:
			return 0;
		case 2:
			return 1;
		case 4:
			return 2;
		case 8:
			return 3;
		case 16:
			return 4;
		case 32:
			return 5;
		case 64:
			return 6;
		case 128:
			return 7;
	}
	return 0;
}
