#ifndef RFIDREADER_H
#define	RFIDREADER_H

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "basic.h"          /* For the basic definitions */
#include "hardware.h"       /* For the S/W definitions */
#include "rfidtags.h"       /* For the tags */

/******************************************************************************/
/* Public variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

#ifdef RFID_SW_UART
void rfidSwInt();               /* Call this @ baud rate (9600) */
#endif

// Common
void rfidCheckComm();           /* Check USART status and reset errors if needed */

// Receiver
uint8_t rfidNewCardAvailable();    /* Returns true if new valid data is available */
void rfidUpdateTimeout();


#endif	/* RFIDREADER_H */
