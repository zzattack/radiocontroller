#ifndef BUSDEFS_H
#define	BUSDEFS_H

#define STX 0x0F
#define ESC 0xF0

#define START_CRC 0xFFFF
#define FINAL_CRC 0xFFFF
#define POLY_CRC 0x8408

// Max 16 module types!
#define CC_MODULE 1
#define GAS_MODULE 2
#define DASH_MODULE 3
#define BUTTON_MODULE 4
#define BLINK_MODULE 5
#define PC_MODULE 6
#define LCD_MODULE 7
#define MODULE_COUNT 8

// Commands: 16*Device ID + Index. Index > 7 <=> Message originating from device

// CC_MODULE Receives:
#define CMD_CC_GETSTATUS    (16*CC_MODULE)
#define CMD_CC_DISCRESP     (16*CC_MODULE + 1)      // Discover response from device 'Data[0]'
// CC_MODULE Sends:
#define CMD_CC_STATUS       (16*CC_MODULE + 8)
#define CMD_CC_DISCOVER     (16*CC_MODULE + 9)      // All Devices should respond with CMD_CC_DISCRESP

// GAS_MODULE Receives:
#define CMD_GAS_GETSTATUS   (16*GAS_MODULE)
#define CMD_GAS_SETCC       (16*GAS_MODULE + 1)
#define CMD_GAS_SETLIMIT    (16*GAS_MODULE + 2)
// GAS_MODULE Sends:
#define CMD_GAS_STATUS      (16*GAS_MODULE + 8)

// DASH_MODULE Receives:
#define CMD_DASH_GETSTATUS  (16*DASH_MODULE)
#define CMD_DASH_SETLEDS    (16*DASH_MODULE + 1)
#define CMD_DASH_SETLED     (16*DASH_MODULE + 2)    // Not used atm
// DASH_MODULE Sends:
#define CMD_DASH_STATUS     (16*DASH_MODULE + 8)

// BUTTON_MODULE Receives:
#define CMD_BUTTON_GETSTATUS (16*BUTTON_MODULE)
// BUTTON_MODULE Sends:
#define CMD_BUTTON_STATUS   (16*BUTTON_MODULE + 8)

// BLINK_MODULE Receives:
#define CMD_BLINK_GETSTATUS (16*BLINK_MODULE)
#define CMD_BLINK_SETSTATE  (16*BLINK_MODULE + 1)
// BLINK_MODULE Sends:
#define CMD_BLINK_STATUS    (16*BLINK_MODULE + 8)

// PC_MODULE Receives:
#define CMD_PC_GETSTATUS   (16*PC_MODULE)
// LCD_MODULE Sends:
#define CMD_PC_STATUS      (16*PC_MODULE + 8)

// LCD_MODULE Receives:
#define CMD_LCD_GETSTATUS   (16*LCD_MODULE)
#define CMD_LCD_NEXT        (16*LCD_MODULE + 1)
// LCD_MODULE Sends:
#define CMD_LCD_STATUS      (16*LCD_MODULE + 8)

#endif	/* BUSDEFS_H */
