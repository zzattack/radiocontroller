#ifndef BUSSERIAL_H
#define	BUSSERIAL_H

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "basic.h"          /* For the basic definitions */
#include "busdefs.h"
#include "hardware.h"       /* For the S/W definitions */

/******************************************************************************/
/* Global Settings                                                            */
/******************************************************************************/

#ifndef COMMAND_BUFFER_MAXSIZE
#define COMMAND_BUFFER_MAXSIZE 16
#endif

#ifndef OUTPUT_BUFFER_MAXSIZE
#define OUTPUT_BUFFER_MAXSIZE 24
#endif

typedef struct {
    uint8_t Command;

    uint8_t CommandDataSize;
    uint8_t CommandData[COMMAND_BUFFER_MAXSIZE];

    uint8_t Valid : 1;
} BusCommand;

#endif	/* BUSSERIAL_H */

