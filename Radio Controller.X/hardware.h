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

#define RELAIS1 LATB0
#define RELAIS2 LATD7
#define RELAIS3 LATB2 // R3 switches accu/bat

#define SwitchRelais RELAIS1
#define ChargeRelais RELAIS2
#define AccessoryRelais RELAIS3

#define SWITCHACCU 0
#define SWITCHBAT 1
#define CHARGEACCU 1
#define CHARGEOFF 0

/* Input Names */
#define TSOP RD1

// accessory is implied by contact, the || serves as a extra check in case the accessory wire fucks up again
#define Accessory (!(DIGITAL1) || Contact)

#define Contact !(DIGITAL2)
#define Knipper !(DIGITAL3)
#define Start !(DIGITAL4)
#define LockReal (!(DIGITAL5) && !Contact)
#define Pump (analog4Value > 127)
#define Brake LATD2

/* Output Names */
#define CCP1 RC2
#define CCP2 RC1

#define LED_GREEN LATC0
#define LED_RED LATD0

#define IR_PWM LATC2
#define IR_EN LATC1



#endif	/* HARDWARE_H */