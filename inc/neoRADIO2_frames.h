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
    uint32_t	serialNumber;
    uint16_t	manufacture_year;
    uint8_t     manufacture_month;
    uint8_t	    manufacture_day;
    uint8_t		deviceType;
    uint8_t		deviceID;
    uint8_t		firmwareVersion_major;
    uint8_t		firmwareVersion_minor;
    uint8_t		hardware_revMinor;
    uint8_t		hardware_revMajor;
    uint8_t     current_state;
    uint8_t     checksum;
} neoRADIO2frame_identifyResponse;

typedef union _neoRADIO2frame_id {
    struct {
        unsigned chip:4;
        unsigned device:4;
    } bits;
    uint8_t byte;
} neoRADIO2frame_id;

typedef struct _neoRADIO2_destination {
    uint8_t device;
    uint8_t chip; //chip is bitmasked so 0b0001000 is the fourth chip
} PACKED neoRADIO2_destination;

typedef struct _neoRADIO2frame_header {
    uint8_t	headerAA;
    uint8_t command;
    neoRADIO2_destination dest;
    uint8_t len;
    uint8_t checksum;
} PACKED neoRADIO2frame_header;

typedef struct _neoRADIO2frame_DeviceReportHeader {
    uint8_t header55;
    uint8_t id;
    uint8_t len;
} PACKED neoRADIO2frame_DeviceReportHeader;

typedef struct _neoRADIO2_ReceiveData {
    neoRADIO2frame_DeviceReportHeader header;
    uint8_t data[36];
} PACKED  neoRADIO2_ReceiveData;

typedef struct _neoRADIO2frame_identify {
    uint8_t device_type;
    uint8_t id;
} PACKED neoRADIO2frame_identify;

typedef enum _neoRADIO2frame_commands {
    NEORADIO2_COMMAND_IDENTIFY      = 0x01,
    NEORADIO2_COMMAND_WRITEDATA     = 0x02,
    NEORADIO2_COMMAND_READDATA      = 0x03,
    NEORADIO2_COMMAND_SETSETTINGS   = 0x04,
    NEORADIO2_COMMAND_READSETTINGS  = 0x05,
    NEORADIO2_COMMAND_START         = 0x0E,
    NEORADIO2_COMMAND_ENTERBOOT     = 0x0F,
} _neoRADIO2frame_commands;

typedef enum _neoRADIO2frame_BLcommands {
    NEORADIO2_BLCOMMAND_WRITEBUFFER	=0,
    NEORADIO2_BLCOMMAND_WRITE		=1,
    NEORADIO2_BLCOMMAND_VERIFY		=2,
} neoRADIO2frame_BLommands;

typedef struct _neoRADIO2AOUTframe_data {
    uint16_t DAC11;
    uint16_t DAC12;
    uint16_t DAC23;
} PACKED neoRADIO2AOUTframe_data;

typedef struct _neoRADIO2TCframe_deviceSettings {
    uint16_t reportRate;
    uint8_t enables;
} PACKED neoRADIO2TCframe_deviceSettings;

typedef struct _neoRADIO2AOUTframe_deviceSettings {
    uint16_t reportRate;
    uint16_t DefaultValue1;
    uint16_t DefaultValue2;
    uint16_t DefaultValue3;
    uint8_t enables;
} PACKED neoRADIO2AOUTframe_deviceSettings;

typedef struct _neoRADIO2AINframe_deviceSettings {
    uint16_t reportRate;
    uint8_t InputVoltageRange;
    uint8_t enables;
} PACKED neoRADIO2AINframe_deviceSettings;

typedef struct _neoRADIO2PWRRLYframe_deviceSettings {
    uint16_t reportRate;
    uint8_t InitialRelayState;
} PACKED neoRADIO2PWRRLYframe_deviceSettings;


typedef struct _neoRADIO2frame_deviceSettings {
    union {
        neoRADIO2TCframe_deviceSettings     tc;
        neoRADIO2AINframe_deviceSettings    ain;
        neoRADIO2AOUTframe_deviceSettings   aout;
        neoRADIO2PWRRLYframe_deviceSettings pwrrly;
    } device;
} PACKED neoRADIO2frame_deviceSettings;

typedef enum _neoRADIO2TC_commands {
    NEORADIO2TC_READDATA		=0,
    NEORADIO2TC_READ_CALDATA	=1,
    NEORADIO2TC_SET_CALDATA		=2,
} neoRADIO2TC_commands;

typedef enum _neoRADIO2AOUT_commands {
    NEORADIO2AOUT_READ_DAC	    =0,
    NEORADIO2AOUT_SET_DAC       =1,
    NEORADIO2AOUT_DISABLE	    =2,
    NEORADIO2AOUT_READ_CALDATA	=3,
    NEORADIO2AOUT_SET_CALDATA	=4,
} neoRADIO2AOUT_commands;

typedef enum _neoRADIO2PWRRLY_commands {
    NEORADIO2PWRRLY_SET_RLY     =0,
    NEORADIO2PWRRLY_READ_RLY	=1,
    NEORADIO2PWRRLY_DISABLE	    =2,
} neoRADIO2PWRRLY_commands;

typedef enum _neoRADIO2states {
    NEORADIO2STATE_RUNNING          =0,
    NEORADIO2STATE_INBOOTLOADER		=1,
} neoRADIO2states;

#define DESTINATION_CHANNEL1 0x01
#define DESTINATION_CHANNEL2 0x02
#define DESTINATION_CHANNEL3 0x04
#define DESTINATION_CHANNEL4 0x08
#define DESTINATION_CHANNEL5 0x10
#define DESTINATION_CHANNEL6 0x20
#define DESTINATION_CHANNEL7 0x40
#define DESTINATION_CHANNEL8 0x80

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
