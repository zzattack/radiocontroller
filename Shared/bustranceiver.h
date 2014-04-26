#ifndef BUSRECEIVER_H
#define	BUSRECEIVER_H

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "busserial.h"

/******************************************************************************/
/* Public variables                                                           */
/******************************************************************************/

#ifndef BUSTRCV_TIMER_HZ
#define BUSTRCV_TIMER_HZ 100
#endif
#ifndef BUSTRCV_PRIVATE_BANK1
#define BUSTRCV_PRIVATE_BANK1 bank0
#endif
#ifndef BUSTRCV_PRIVATE_BANK2
#define BUSTRCV_PRIVATE_BANK2 bank1
#endif

#ifdef BUS_RCVR
BusCommand BUSTRCV_PRIVATE_BANK1 busCommand;
#endif

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

// Common
void busCheckComm();            /* Check USART status and reset errors if needed */

// Receiver
#ifdef BUS_RCVR
void busReceiveComm();             // Receive the next available byte
bool busNewDataAvailable();     /* Returns true if new valid data is available */
void busUpdateTimeout();
#endif

// Transmitter
#ifdef BUS_TRMT
void busProcessOutput();
void busTransmitSync(uint8_t command, const uint8_t *commandData, uint8_t commandDataSize);
void busTransmit(uint8_t command, const uint8_t *commandData, uint8_t commandDataSize);
bool busOutputBusy();
#endif

#endif	/* BUSRECEIVER_H */
