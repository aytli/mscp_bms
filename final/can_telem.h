#ifndef CAN_TELEM_H
#define CAN_TELEM_H

// NOTE: The following tables are x-macros
// X macro tutorial: http://www.embedded.com/design/programming-languages-and-tools/4403953/C-language-coding-errors-with-X-macros-Part-1

//////////////////////////////
// CAN BUS DEFINES ///////////
//////////////////////////////

#define EXPAND_AS_CAN_ID_ENUM(a,b,c,d)  a##_ID  = b,
#define EXPAND_AS_CAN_LEN_ENUM(a,b,c,d) a##_LEN = c,
#define EXPAND_AS_CAN_ID_ARRAY(a,b,c,d)           b,
#define EXPAND_AS_CAN_LEN_ARRAY(a,b,c,d)          c,
#define EXPAND_AS_DATA_ADDRESS_ARRAY(a,b,c,d)     d,

// X macro table of CANbus packets
//        Packet name            ,    ID, Length
#define CAN_ID_TABLE(ENTRY)                                              \
    ENTRY(CAN_BPS_VOLTAGE1       , 0x600,  8, g_bps_voltage_page)        \
    ENTRY(CAN_BPS_VOLTAGE2       , 0x601,  8, g_bps_voltage_page+8)      \
    ENTRY(CAN_BPS_VOLTAGE3       , 0x602,  8, g_bps_voltage_page+16)     \
    ENTRY(CAN_BPS_VOLTAGE4       , 0x603,  8, g_bps_voltage_page+24)     \
    ENTRY(CAN_BPS_VOLTAGE5       , 0x604,  8, g_bps_voltage_page+32)     \
    ENTRY(CAN_BPS_VOLTAGE6       , 0x605,  8, g_bps_voltage_page+40)     \
    ENTRY(CAN_BPS_VOLTAGE7       , 0x606,  8, g_bps_voltage_page+48)     \
    ENTRY(CAN_BPS_VOLTAGE8       , 0x607,  4, g_bps_voltage_page+56)     \
    ENTRY(CAN_BPS_TEMPERATURE1   , 0x608,  8, g_bps_temperature_page)    \
    ENTRY(CAN_BPS_TEMPERATURE2   , 0x609,  8, g_bps_temperature_page+8)  \
    ENTRY(CAN_BPS_TEMPERATURE3   , 0x60A,  8, g_bps_temperature_page+16) \
    ENTRY(CAN_BPS_CURRENT        , 0x60B,  2, g_bps_current_page)        \
    ENTRY(CAN_BPS_BALANCING      , 0x60C,  4, g_bps_balancing_page)      \
    ENTRY(CAN_BPS_STATUS         , 0x60D,  1, g_bps_status_page)
#define N_CAN_ID 14

enum {CAN_ID_TABLE(EXPAND_AS_CAN_ID_ENUM)};
enum {CAN_ID_TABLE(EXPAND_AS_CAN_LEN_ENUM)};


//////////////////////////
// TELEMETRY_DEFINES /////
//////////////////////////

#define EXPAND_AS_TELEM_ID_ENUM(a,b,c,d)  a##_ID  = b,
#define EXPAND_AS_TELEM_LEN_ENUM(a,b,c,d) a##_LEN = c,
#define EXPAND_AS_TELEM_LEN_ARRAY(a,b,c,d)          c,
#define EXPAND_AS_TELEM_PAGE_ARRAY(a,b,c,d)         d,
#define EXPAND_AS_TELEM_PAGE_DECLARATIONS(a,b,c,d) static int8 d[c];

// X macro table of telemetry packets
//        Packet name            ,    ID, Length, Page array
#define TELEM_ID_TABLE(ENTRY)                                         \
    ENTRY(TELEM_BPS_VOLTAGE      ,  0x0B, 60, g_bps_voltage_page)     \
    ENTRY(TELEM_BPS_TEMPERATURE  ,  0x0D, 24, g_bps_temperature_page) \
    ENTRY(TELEM_BPS_CURRENT      ,  0x11,  2, g_bps_current_page)     \
    ENTRY(TELEM_BPS_BALANCING    ,  0x13,  4, g_bps_balancing_page)   \
    ENTRY(TELEM_BPS_STATUS       ,  0x17,  1, g_bps_status_page)
#define N_TELEM_ID 5

enum {TELEM_ID_TABLE(EXPAND_AS_TELEM_ID_ENUM)};
enum {TELEM_ID_TABLE(EXPAND_AS_TELEM_LEN_ENUM)};


//////////////////////////////
// CAN COMMAND DEFINES ///////
//////////////////////////////

#define EXPAND_AS_COMMAND_ID_ENUM(a,b)  a##_ID  = b,

// X macro table of CANbus commands
//        Packet name                  ,    ID
#define CAN_COMMAND_TABLE(ENTRY)                \
    ENTRY(COMMAND_PMS_DISCONNECT_ARRAY , 0x777) \
    ENTRY(COMMAND_ENABLE_BALANCING     , 0x888) \
    ENTRY(COMMAND_EVDC_DRIVE           , 0x501) \
    ENTRY(RESPONSE_MPPT1               , 0x771) \
    ENTRY(RESPONSE_MPPT2               , 0x772) \
    ENTRY(RESPONSE_MPPT3               , 0x773) \
    ENTRY(RESPONSE_MPPT4               , 0x774)
#define N_CAN_COMMAND 7

enum {CAN_COMMAND_TABLE(EXPAND_AS_COMMAND_ID_ENUM)};

#endif
