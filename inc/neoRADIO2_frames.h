#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#define PACKED 
#else
#define PACKED __attribute__((packed))
#endif

typedef enum _neoRADIO2_deviceTypes {
	NEORADIO2_DEVTYPE_TC		= 0,
	NEORADIO2_DEVTYPE_DIO		= 1,
	NEORADIO2_DEVTYPE_PWRRLY	= 2,
	NEORADIO2_DEVTYPE_AIN		= 3,
	NEORADIO2_DEVTYPE_AOUT		= 4,
	NEORADIO2_DEVTYPE_CANHUB	= 5,
	NEORADIO2_DEVTYPE_HOST  	= 0xFF,
} neoRADIO2_deviceTypes;

typedef struct _neoRADIO2frame_identifyResponse {
	uint32_t	serial_number;
	uint16_t	manufacture_year;
	uint8_t     manufacture_month;
	uint8_t	    manufacture_day;
	uint8_t		device_type;
	uint8_t		device_number;
	uint8_t		device_bank;
	uint8_t		firmwareVersion_major;
	uint8_t		firmwareVersion_minor;
	uint8_t		hardware_revMinor;
	uint8_t		hardware_revMajor;
	uint8_t     current_state;
} neoRADIO2frame_identifyResponse;

typedef struct _neoRADIO2frame_header {
	uint8_t	start_of_frame; //0xAA for host packets or 0x55 for device packets
	uint8_t command_status;
	uint8_t device;
	uint8_t bank; //bank is bitmasked so 0b0001000 is the fourth bank
	uint8_t len;
} PACKED neoRADIO2frame_header;

typedef struct _neoRADIO2frame {
	neoRADIO2frame_header header;
	uint8_t data[64];
	uint8_t crc;
} PACKED  neoRADIO2frame;

typedef struct _neoRADIO2frame_identify {
	uint8_t device_type;
	uint8_t device_number;
	uint8_t bank_number;
} PACKED neoRADIO2frame_identify;

typedef enum _neoRADIO2frame_commands {
	NEORADIO2_COMMAND_START             =   0x01,
	NEORADIO2_COMMAND_IDENTIFY          =   0x02,
	NEORADIO2_COMMAND_WRITE_DATA        =   0x03,
	NEORADIO2_COMMAND_READ_DATA         =   0x04,
	NEORADIO2_COMMAND_WRITE_SETTINGS    =   0x05,
	NEORADIO2_COMMAND_READ_SETTINGS     =   0x06,
	NEORADIO2_COMMAND_DONT_USE1         =   0x07,
	NEORADIO2_COMMAND_DONT_USE2         =   0x08,
	NEORADIO2_COMMAND_TOGGLE_LED        =   0x09,
	NEORADIO2_COMMAND_READ_PCBSN        =   0x10,

	NEORADIO2_COMMAND_READ_CAL			=   0x20,
	NEORADIO2_COMMAND_WRITE_CAL         =   0x21,
	NEORADIO2_COMMAND_WRITE_CALPOINTS   =   0x22,
	NEORADIO2_COMMAND_STORE_CAL			=   0x23,
	NEORADIO2_COMMAND_READ_CALPOINTS    =   0x24,
	NEORADIO2_COMMAND_READ_CAL_INFO     =   0x25,

	NEORADIO2_COMMAND_BL_WRITEBUFFER    =   0xFA,
	NEORADIO2_COMMAND_BL_WRITETOFLASH	=   0xFB,
	NEORADIO2_COMMAND_BL_VERIFY		    =   0xFC,
	NEORADIO2_COMMAND_ENTERBOOT         =   0xFF,
} neoRADIO2frame_commands;

typedef enum _neoRADIO2frame_deviceStatus {
	NEORADIO2_STATUS_SENSOR				=   0x00,
	NEORADIO2_STATUS_FIRMWARE			=   0x01,
	NEORADIO2_STATUS_IDENTIFY			=   0x02,
	NEORADIO2_STATUS_READ_SETTINGS		=   0x03,
	NEORADIO2_STATUS_WRITE_SETTINGS		=	0x04,
	NEORADIO2_STATUS_READ_PCBSN 		=   0x05,
	NEORADIO2_STATUS_CAL				=   0x06,
	NEORADIO2_STATUS_CAL_STORE          =   0x07,
	NEORADIO2_STATUS_CAL_INFO           =   0x08,
	NEORADIO2_STATUS_CALPOINTS          =   0x09,
	NEORADIO2_STATUS_NEED_ID			=   0xFF,
} neoRADIO2frame_deviceStatus;

typedef struct _neoRADIO2AOUTframe_data {
	uint16_t DAC11;
	uint16_t DAC12;
	uint16_t DAC23;
} PACKED neoRADIO2AOUTframe_data;

typedef struct _neoRADIO2_deviceSettings {
	uint32_t poll_rate_ms;
	uint32_t channel_1_config;
	uint32_t channel_2_Config;
	uint32_t channel_3_Config;
} PACKED neoRADIO2_deviceSettings;

typedef enum _neoRADIO2states {
	NEORADIO2STATE_RUNNING          =0,
	NEORADIO2STATE_INBOOTLOADER		=1,
} neoRADIO2states;

// Used with NEORADIO2_COMMAND_READ_CAL/NEORADIO2_COMMAND_WRITE_CAL/NEORADIO2_COMMAND_WRITE_CALPOINTS
typedef struct _neoRADIO2frame_calHeader {
	// read sets this, write needs this
	uint8_t num_of_pts;
	// read/write needs this. see cr_is_bitmask
	uint16_t channel;
	// read/write needs this. see cr_is_bitmask
	uint16_t range;
	// read sets this, write needs this. sizeof(CALIBRATION_TYPE) - TC = float, AIN = uint32
	uint8_t cal_type_size;
	// allows us to read/write without multiple calls for each channel/range.
	uint8_t cr_is_bitmask;
	// read sets this, write ignores this.
	uint8_t cal_is_valid;

	uint8_t _reserved[3];
} PACKED neoRADIO2frame_calHeader;

typedef enum _neoRADIO2CalType {
	NEORADIO2CALTYPE_NONE = 0, // Reads raw sensor value without using calibration values
	NEORADIO2CALTYPE_ENABLE, // Reads sensor value with calibration applied
	NEORADIO2CALTYPE_ENHANCED, // Same as ENABLE but with slower sample rate
} neoRADIO2CalType;

typedef enum _neoRADIO2_CANMsgType {
	NEORADIO2_CANMSGTYPE_SID_CLASSIC = 0,
	NEORADIO2_CANMSGTYPE_XID_CLASSIC = 1,
	NEORADIO2_CANMSGTYPE_SID_FD = 2,
	NEORADIO2_CANMSGTYPE_XID_FD = 3,
} neoRADIO2_CANMsgType;
typedef struct _neoRADIO2_CAN_settings
{
	uint32_t Arbid; //Arb Id
	uint8_t Location; //byte where the message starts
	uint8_t msgType; //neoRADIO2_CANMsgType
} PACKED neoRADIO2_CAN_settings;

typedef struct _neoRADIO2settings_CAN
{
	uint32_t Arbid; //Arb Id
	uint8_t Location; //byte where the message starts
	uint8_t msgType; //neoRADIO2_CANMsgType
} PACKED neoRADIO2settings_CAN;

typedef struct _neoRADIO2settings_ChannelName {
	uint8_t length;
	uint8_t charSize;
	union {
		uint32_t u32[16];
		uint16_t u16[16*2];
		uint8_t u8[16*4];
	} chars;
} PACKED neoRADIO2Settings_ChannelName;

typedef struct _neoRADIO2_settings {
	neoRADIO2_deviceSettings config;
	neoRADIO2Settings_ChannelName name1;
	neoRADIO2Settings_ChannelName name2;
	neoRADIO2Settings_ChannelName name3;
	neoRADIO2settings_CAN can;
} PACKED neoRADIO2_settings;

#define NEORADIO2_SETTINGS_PARTSIZE 32
typedef struct _neoRADIO2_SettingsPart {
	uint8_t part;
	uint8_t data[NEORADIO2_SETTINGS_PARTSIZE];
} neoRADIO2_SettingsPart;

#define NEORADIO2_DESTINATION_BANK1 0x01
#define NEORADIO2_DESTINATION_BANK2 0x02
#define NEORADIO2_DESTINATION_BANK3 0x04
#define NEORADIO2_DESTINATION_BANK4 0x08
#define NEORADIO2_DESTINATION_BANK5 0x10
#define NEORADIO2_DESTINATION_BANK6 0x20
#define NEORADIO2_DESTINATION_BANK7 0x40
#define NEORADIO2_DESTINATION_BANK8 0x80

typedef union _bytesToFloat {
	float fp;
	uint8_t b[sizeof(float)];
} bytesToFloat;

#ifdef _MSC_VER
#pragma pack(pop)
#undef PACKED 
#else
#undef PACKED
#endif


#ifdef __cplusplus
}
#endif
