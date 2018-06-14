#include "ft260.h"
#include "hidapi.h"
#include <string.h>
#include <stdint.h>


//TODO support more devices, We need to be able to set Serial in ft260 to do this.
int ft260FindDevices(ft260_device devices[], int len, int vid, int pid)
{
    int deviceCount = 0;
    struct hid_device_info *deviceList = NULL;
    struct hid_device_info *deviceListPtr = NULL;

    if (hid_init() != 0)
        return -1;
    deviceList = hid_enumerate(vid, pid);

    if (deviceList == NULL)
    {
	    // try one more time
	    deviceList = hid_enumerate(vid, pid);
	    if (!deviceList)
		    return -1;
    }

    deviceListPtr = deviceList;


    while (deviceList != NULL)
    {
        #ifdef __APPLE__
        int interfaceNumber = deviceList->path[strlen(deviceList->path) - 1] - 0x30;
        #else
        int interfaceNumber = deviceList->interface_number;
        #endif
        if (deviceList->serial_number == NULL) //Serial number was not set
            devices[0].SerialNumber[deviceCount] = '0';
        else
            wcscpy(devices[deviceCount].SerialNumber, deviceList->serial_number);

        if (interfaceNumber == 0)
        {
            strcpy(devices[deviceCount].PathInterface0, deviceList->path);
            if (deviceList->next != NULL)
            {
                strcpy(devices[deviceCount].PathInterface1, deviceList->next->path);
                deviceList = deviceList->next->next;
                deviceCount++;
            }

        }
        else if (interfaceNumber == 1)
        {
            strcpy(devices[deviceCount].PathInterface1, deviceList->path);
            if (deviceList->next != NULL)
            {
                strcpy(devices[deviceCount].PathInterface0, deviceList->next->path);
                deviceList = deviceList->next->next;
                deviceCount++;
            }
        }
        else
        {
            deviceList = deviceList->next;
        }
    }

    hid_free_enumeration(deviceListPtr);
    return deviceCount;
}


int ft260OpenDevice(ft260_device* dev)
{
    dev->HandleInterface0 = hid_open_path(dev->PathInterface0);
    if (dev->HandleInterface0 == NULL)
        return -1;
    dev->HandleInterface1 = hid_open_path(dev->PathInterface1);
    if (dev->HandleInterface1 == NULL)
        return -1;

    hid_set_nonblocking(dev->HandleInterface0, 1);
    hid_set_nonblocking(dev->HandleInterface1, 1);

    return 0;
}
int ft260CloseDevice(ft260_device* dev)
{
    if ( dev->HandleInterface0 != NULL)
        hid_close(dev->HandleInterface0);
    if ( dev->HandleInterface1 != NULL)
        hid_close(dev->HandleInterface1);

    hid_exit();
    return 0;
}
int ft260SetupUART(ft260_device* dev, unsigned int baud, unsigned int stop, unsigned int parity)
{
    uint8_t buf[64];

    memset(buf, 0 ,sizeof(buf));

    buf[0] = SYSTEM_SETTING_REPORT_ID;
    buf[1] = UART_CONFIG;
    buf[2] = UART_CONFIG_FLOW_CTRL_OFF;
    buf[3] = (baud & 0xFF);
    buf[4] = ((baud>>8) & 0xFF);
    buf[5] = ((baud>>16) & 0xFF);
    buf[6] = ((baud>>24) & 0xFF);
    buf[7] = UART_CONFIG_DATA_BIT_8;
    switch(parity)
    {
        case 0:
            buf[8] = UART_CONFIG_PARITY_OFF;
            break;
        case 1:
            buf[8] = UART_CONFIG_PARITY_ODD;
            break;
        case 2:
            buf[8] = UART_CONFIG_PARITY_EVEN;
            break;
        default:
            buf[8] = UART_CONFIG_PARITY_OFF;
    }
    switch(stop)
    {
        case 1:
            buf[9] = UART_CONFIG_STOP_BIT_1;
            break;
        case 2:
            buf[9] = UART_CONFIG_STOP_BIT_2;
            break;
        default:
            buf[9] = UART_CONFIG_STOP_BIT_1;
    }
    buf[10] = UART_CONFIG_BREAKING_OFF;

    return ft260ConfigureDevice(dev, buf, sizeof(buf));
}
int ft260SetupI2C(ft260_device* dev, unsigned int freq, unsigned int address)
{
    //TODO
    return -1;
}
int ft260ConfigureDevice(ft260_device* dev, uint8_t* data, int len)
{
    int result = 0;

    if (len > 64)
        return -1;

    if (dev->HandleInterface1 == NULL)
        return -1;

    result = hid_send_feature_report(dev->HandleInterface1, (unsigned char *)data, len);
    if (result != 64)
        return -1;

    return 0;
}

int ft260TransmitUART(ft260_device* dev, uint8_t* data, int len)
{
    uint8_t buf[64];
    int writelen = 0;

    if (dev->HandleInterface1 == NULL)
    {
        return -1;
    }

    if (len > 59)
        return -2;
    buf[0] = 240 + (len / 4);
    buf[1] = len;

    for (int i = 0; i < len; i++)
    {
        buf[i+2] = data[i];
    }

    writelen = hid_write(dev->HandleInterface1, (unsigned char *)buf, (len + 2));
#ifdef _WIN32
    if (writelen > len)
    {
        writelen = len + 2;
    }
#endif

    return writelen - 2;
}
int ft260ReceiveUART(ft260_device* dev, uint8_t* data)
{
    uint8_t buf[64] = {0};
    int len = 0;

    if (dev->HandleInterface1 == NULL)
    {
        return -1;
    }

    len = hid_read(dev->HandleInterface1, (unsigned char*)buf, 64);
    if (len > 0)
    {
        len = buf[1];
        for (int i = 0; i < len; i++)
        {
            data[i] = buf[i+2];
        }
    }
    return len;
}
int ft260TransmitI2C(ft260_device* dev, uint8_t* data, int len)
{
    //TODO
    return -1;
}
int ft260ReceiveI2C(ft260_device* dev, uint8_t* data)
{
    //TODO
    return -1;
}