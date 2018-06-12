#pragma once
#include "../libs/hidapi/hidapi/hidapi.h"
#include <stdint.h>

typedef struct _ft260_device {
    char PathInterface0[256];
    char PathInterface1[256];
    wchar_t SerialNumber[16];
    hid_device* HandleInterface0;
    hid_device* HandleInterface1;
} ft260_device;

int ft260FindDevices(ft260_device devices[], int len, int vid, int pid);
int ft260OpenDevice(ft260_device* dev);
int ft260CloseDevice(ft260_device* dev);
int ft260SetupUART(ft260_device* dev, unsigned int baud, unsigned int stop, unsigned int parity);
int ft260SetupI2C(ft260_device* dev, unsigned int freq, unsigned int address);
int ft260ConfigureDevice(ft260_device* dev, uint8_t* data, int len);
int ft260TransmitUART(ft260_device* dev, uint8_t* data, int len);
int ft260ReceiveUART(ft260_device* dev, uint8_t* data);
int ft260TransmitI2C(ft260_device* dev, uint8_t* data, int len);
int ft260ReceiveI2C(ft260_device* dev, uint8_t* data);

//Report IDs for FT260
#define CHIP_CODE_REPORT_ID			0xA0 //Feature
#define SYSTEM_SETTING_REPORT_ID	0xA1 //Feature
#define GPIO_REPORT_ID				0xB0 //Feature
#define INTERRUPT_STATUS_REPORT_ID  0xB1 //Input
#define I2C_STATUS_REPORT_ID		0xC0 //Feature
#define I2C_READRQ_REPORT_ID		0xC2 //Output
#define I2C_REPORT_REPORT_ID		0xD0 //Input-Output
#define UART_STATUS_REPORT_ID		0xE0 //Feature
#define UART_RI_REPORT_ID			0xE2 //Feature
#define UART_REPORT_REPORT_ID		0xF0 //Feature

//Configrurations for FT260 CHIP settings
#define CHIP_MODE_DCNF0				0x01
#define CHIP_MODE_DCNF1				0x02

#define SET_CLOCK					0x01 //Set Clock for FT260 Request

#define CLOCK_CTRL_12				0x00 //12MHz
#define CLOCK_CTRL_24				0x01 //24MHz
#define CLOCK_CTRL_48				0x02 //48MHz

#define SET_UART_MODE				0x03 //Request
#define UART_MODE_OFF				0x00 //Turns UART off, switches to GPIO
#define UART_MODE_RTS				0x01 //RTS and CTS active
#define UART_MODE_DTR				0x02 //DTR and DSR active
#define UART_MODE_XON_XOFF			0x03
#define	UART_MODE_FLOW_CTRL_OFF		0x04

#define EN_INTERRUPT_MODE			0x05 //Wake up if data Request
#define EN_INTERRUPT_MODE_OFF		0x00
#define EN_INTERRUPT_MODE_ON		0x01

//GPIO2 Alternate Functions
#define SET_GPIO2_MODE				0x06 //Request
#define GPIO2_MODE_GPIO				0x00 //Regular GPIO functionallity
#define GPIO2_MODE_SUSPOUT			0x01 //Suspened Pin enabled
#define GPIO2_MODE_PWREN			0x02 //Power Enable Ind pin enabled
#define GPIO2_MODE_TX_LED			0x04 //LED transaction enabled

#define UART_MODE_DCD_RI			0x07 //Request
#define UART_MODE_DCD_RI_OFF		0x00 //Switch to GPIO4, and GPIO5
#define UART_MODE_DCD_RI_ON			0x01 //Enable DCD and RI

//GPIOA alternate functions
#define SET_GPIOA_MODE				0x08 //Request
#define GPIOA_MODE_GPIO				0x00 //Regular GPIO functionallity
#define GPIOA_MODE_TX_ACTIVE		0x03 //TX_ACTIVE Pin enabled
#define GPIOA_MODE_TX_LED			0x04 //LED transaction enabled

//GPIOG alternate functions
#define SET_GPIOG_MODE				0x09 //Request
#define GPIOG_MODE_GPIO				0x00 //Regular GPIO functionallity
#define GPIOG_MODE_PWREN			0x02 //Power Enable Ind pin enabled
#define GPIOG_MODE_RX_LED			0x05 //RX_LED transaction enabled
#define GPIOG_MODE_BCD_DET			0x06 //Battery Charger detection

//Interrupt Trigger Configurations
#define SET_INTERRUPT_CONDITION		0x0A //Request
#define	INTERRUPT_CONDITION_RISING	0x00 //Rising Edge Detection
#define INTERRUPT_CONDITION_HIGH	0x01 //Detect High leveled voltage
#define INTERRUPT_CONDITION_FALLING	0x02 //Falling Edge Detection

#define INTERRUPT_CONDITION_1MS		0x01
#define INTERRUPT_CONDITION_5MS		0x02
#define	INTERRUPT_CONDITION_30MS	0x03

//Suspended Out Configuration
#define SET_SUSPEND_OUT				0x0B //Request
#define SUSPEND_OUT_HIGH			0x00
#define	SUSPEND_OUT_LOW				0x01

//Enabling UART RI wake configuration
#define ENABLE_UART_RI_WAKE			0x0C //Request
#define UART_RI_WAKE_DISABLE		0x00
#define	UART_RI_WAKE_ENABLE			0x01

//I2C reset configuration
#define I2C_RESET					0x20

//I2C clock speed configurations
#define SET_I2C_CLK_SPEED			0x22
#define	I2C_CLK_SPEED_LSB
#define	I2C_CLK_SPEED_MSB

//UART reset configuration
#define UART_RESET					0x40

//Configurations for
enum {
    UART_DATA_PLAYLOAD_4_BYTES = 240,
    UART_DATA_PLAYLOAD_8_BYTES,
    UART_DATA_PLAYLOAD_12_BYTES,
    UART_DATA_PLAYLOAD_16_BYTES,
    UART_DATA_PLAYLOAD_20_BYTES,
    UART_DATA_PLAYLOAD_24_BYTES,
    UART_DATA_PLAYLOAD_28_BYTES,
    UART_DATA_PLAYLOAD_32_BYTES,
    UART_DATA_PLAYLOAD_36_BYTES,
    UART_DATA_PLAYLOAD_40_BYTES,
    UART_DATA_PLAYLOAD_44_BYTES,
    UART_DATA_PLAYLOAD_48_BYTES,
    UART_DATA_PLAYLOAD_52_BYTES,
    UART_DATA_PLAYLOAD_56_BYTES,
    UART_DATA_PLAYLOAD_60_BYTES,
};

#define UART_CONFIG					0x41

#define UART_CONFIG_FLOW_CTRL_ON	0x03
#define UART_CONFIG_FLOW_CTRL_OFF	0x04

#define UART_CONFIG_BAUD_RATE_1
#define UART_CONFIG_BAUD_RATE_2
#define UART_CONFIG_BAUD_RATE_3

#define UART_CONFIG_DATA_BIT_7		0x07
#define UART_CONFIG_DATA_BIT_8		0x08

#define UART_CONFIG_PARITY_OFF		0x00
#define UART_CONFIG_PARITY_ODD		0x01
#define UART_CONFIG_PARITY_EVEN		0x02
#define UART_CONFIG_PARITY_HIGH		0x03
#define UART_CONFIG_PARITY_LOW		0x04

#define UART_CONFIG_STOP_BIT_1		0x00
#define UART_CONFIG_STOP_BIT_2		0x02

#define UART_CONFIG_BREAKING_OFF	0x00
#define UART_CONFIG_BREAKING_ON		0x01

//Configurations for GPIO on the FT260
#define GPIO_0						0x01
#define GPIO_1						0X02
#define GPIO_2						0x04
#define GPIO_3						0x08
#define GPIO_4						0x10
#define GPIO_5						0x20

#define GPIO_A						0x01
#define GPIO_B						0x02
#define GPIO_C						0x04
#define GPIO_D						0x08
#define GPIO_E						0x10
#define GPIO_F						0x20
#define GPIO_G						0x40
#define GPIO_H						0x80

#define GPIO_OUTPUT					0x01
#define GPIO_INPUT				    0x00