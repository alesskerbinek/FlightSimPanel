#ifndef APPLICATION_DEFINES_H
#define APPLICATION_DEFINES_H

#include <stdint.h>
#include <stddef.h>
#include <Arduino.h>

// --------------------------------------------------------------------------

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 0
#endif

#ifndef SW_VERSION_MAJOR
#define SW_VERSION_MAJOR	0
#endif

#ifndef SW_VERSION_MINOR
#define	SW_VERSION_MINOR	9
#endif

// --------------------------------------------------------------------------

#define PROJECT_NAME    "NavCom"

// Inputs
#define ROTARY_COUNT    2
#define BUTTON_COUNT    7

// Outputs
#define DIGIT_COUNT     12

// --------------------------------------------------------------------------

// Connection status LED
#define PIN_CONN 0

// NavCom Button pins
#define PIN_SW1 5
#define PIN_SW2 17
#define PIN_SW3 23
#define PIN_SW4 19
#define PIN_SW5 18

// NavCom Rotary Knob input pins
#define ROT1_B 39
#define ROT1_A 36
#define ROT2_B 35
#define ROT2_A 34

// NavCom Digit Select bus pins
#define DIG_SEL_A 16
#define DIG_SEL_B 4
#define DIG_SEL_C 2
#define DIG_SEL_D 15

// NavCom Digit Data bus pins
#define SEG_A 32
#define SEG_B 33
#define SEG_C 25
#define SEG_D 26
#define SEG_E 27
#define SEG_F 14
#define SEG_G 12
#define SEG_DP 13

// NavCom 7-segment digit addresses
#define DIG_X 4
#define DIG_Y 12
#define DIG_1 5
#define DIG_2 6
#define DIG_3 3
#define DIG_4 1
#define DIG_5 2
#define DIG_6 0
#define DIG_7 13
#define DIG_8 14
#define DIG_9 11
#define DIG_10 9
#define DIG_11 10
#define DIG_12 8

// 7-segment characters
//   A
// F   B
//   G
// E   C
//   D   DP
#define CH_0 0xC0
#define CH_1 0xF9
#define CH_2 0xA4
#define CH_3 0xB0
#define CH_4 0x99
#define CH_5 0x92
#define CH_6 0x82
#define CH_7 0xF8
#define CH_8 0x80
#define CH_9 0x90
#define CH_A 0x88
#define CH_B 0x83
#define CH_C 0xC6
#define CH_D 0xA1
#define CH_E 0x86
#define CH_F 0x8E
#define CH_G 0x82
#define CH_I 0xF9
#define CH_L 0xC7
#define CH_N 0xAB
#define CH_O 0xC0
#define CH_S 0x92
#define CH_T 0x87
#define CH_U 0xC1
#define CH_V 0xC1 //0x8D
#define CH_Y 0x91
#define CH_Z 0xA4
#define CH_SPACE 0xFF

#define DECIMAL_POINT   0x7F

// --------------------------------------------------------------------------

//! Settings structure
typedef  struct {
    uint32_t uiSerialNumber;    // 4 bytes
    uint32_t uiActiveValue;     // 4 bytes
    uint32_t uiStandbyValue;    // 4 bytes
    uint8_t  uiUnitType;        // 1 byte
    char     acWiFiSSID[33];    // 33 bytes
    char     acWiFiPass[33];    // 33 bytes
    uint8_t  auiFree0;          // FREE SPACE
    uint16_t uiUdpListenPort;   // 2 bytes
    uint16_t uiUdpRemotePort;   // 2 bytes
    uint32_t uiUdpRemoteIP;     // 4 bytes
    uint16_t uiOBS;             // 2 bytes
    uint8_t  auiFree[166];      // FREE SPACE (sum of structure = 256 bytes)
} settings_t;

//! Unit types
typedef  enum {
    utUndefined = 0,
    utCom1,
    utCom2,
    utVOR1,
    utVOR2,
    utADF1,
    utADF2,
    utXPNDR
} UnitTypes;

//! Unit modes
typedef  enum {
    xmUndefined = 0,
    // General
    gmOff,
    // XPNDR
    xmSby,
    xmOn,
    xmAlt,
    // ADF
    amFrq,
    amFlT,
    amElT,
} UnitModes;

// --------------------------------------------------------------------------

#endif
