#ifndef HARDWARE_H
#define	HARDWARE_H

/******************************************************************************/
/* Hardware Settings                                                          */
/******************************************************************************/
#include <xc.h>
#include <stdint.h>

#define UART_DISABLE_PORT2
#define UART_DBG_PORT 1

#define _XTAL_FREQ 48000000

#define DIGITAL1 PORTAbits.RA4
#define DIGITAL2 PORTAbits.RA5
#define DIGITAL3 PORTEbits.RE0
#define DIGITAL4 PORTEbits.RE1
#define DIGITAL5 PORTEbits.RE2

#define RELAIS1 LATBbits.LATB0
#define RELAIS2 LATDbits.LATD7
#define RELAIS3 LATBbits.LATB2 // R3 switches accu/bat

#define SwitchRelais RELAIS1
#define ChargeRelais RELAIS2
#define AccessoryRelais RELAIS3

#define SWITCHACCU 0
#define SWITCHBAT 1
#define CHARGEACCU 1
#define CHARGEOFF 0

// accessory is implied by contact, the || serves as a extra check in case the accessory wire fucks up again
#define BTNAccessory (!(DIGITAL1) || BTNContact)
#define BTNContact !(DIGITAL2)
#define BTNBlink !(DIGITAL3)
#define BTNStart !(DIGITAL4)
#define BTNLockReal (!(DIGITAL5) && !Contact)
extern uint8_t analog4Value;
#define BTNPump (analog4Value > 127)



#define LED_GREEN LATC0
#define LED_RED LATD0
#define Brake LATD2

#define BUTTON_0 BTNAccessory
#define Accessory bcPressed(0)

#define BUTTON_1 BTNContact
#define Contact bcPressed(1)

#define BUTTON_2 BTNStart
#define Start bcPressed(2)

#define BUTTON_3 BTNPump
#define Pump bcPressed(3)

#define BUTTON_4 BTNLockReal
#define LockReal bcPressed(4)

#define BUTTON_5 BTNBlink
#define Blink bcPressed(5)

#define BUTTON_COUNT 5



#endif	/* HARDWARE_H */