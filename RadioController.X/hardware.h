#ifndef HARDWARE_H
#define	HARDWARE_H


/******************************************************************************/
/* Hardware Settings                                                          */
/******************************************************************************/

#define _XTAL_FREQ 48000000

/* General Names */
#define ANALOG1 RA0
#define ANALOG2 RA1
#define ANALOG3 RA2
#define ANALOG4 RA3

#define DIGITAL1 RA4
#define DIGITAL2 RA5
#define DIGITAL3 RE0
#define DIGITAL4 RE1
#define DIGITAL5 RE2
#define DIGITALXX REXX

#define RELAIS1 LATB0
#define RELAIS2 LATD7
#define RELAIS3 ERROR // R3 switches accu/bat
#define RELAIS4 ERROR // R4 switches accu/nothing
#define R3ACCU = 0
#define R3BAT = 1
#define R4ACCU = 0
#define R4OFF = 1

/* Input Names */
#define TSOP RD1

#define Accessory !(DIGITAL1)
#define Knipper !(DIGITAL3)
#define Lock (!(DIGITAL5) && !Contact)
#define Contact !(ERROR)
#define Start !(ERROR)
#define Accu !(ERROR)
#define Pump !(ERROR)

/* Output Names */
#define CCP1 RC2
#define CCP2 RC1

#define LED_GREEN LATC0
#define LED_RED LATD0

#define IR_PWM LATC2
#define IR_EN LATC1

#define AccessoryRelais RELAIS1


#endif	/* HARDWARE_H */