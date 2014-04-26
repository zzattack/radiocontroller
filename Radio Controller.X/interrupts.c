/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#include "globals.h"
#include "user.h"          /* User funct/params */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

void interrupt isr()
{
    /* Determine which flag generated the interrupt */

    if (TMR0IF) {
        TMR0IF = 0;
        TMR0L = 64935 & 0xFF;
        TMR0H = (64935 >> 8) & 0xFF;

        // Receiver
        if (irRecState != 0) {
            if (irRecState < 400*8) {
                uint16_t byteNr = irRecState >> 3;
                uint8_t bitNr = irRecState & 7;

                if (!TSOP) {
                    setbit(irRecBytes[byteNr], bitNr);
                    irLastRecByte = byteNr;
                }
                irRecState++;

                if (byteNr > txIndex && irLastRecByte > 20) {
                    if (TXIF) {
                        TXREG = irRecBytes[txIndex];
                        txIndex++;
                    }
                }

            } else {
                if (txIndex < 400 && irLastRecByte > 20) {
                    if (TXIF) {
                        TXREG = irRecBytes[txIndex];
                        txIndex++;
                    }
                } else {
                    irRecState = 0;
                }
            }
        } else if (!TSOP) {
            irRecState = 1;
            irLastRecByte = 0;
            txIndex = 0;
            irRecBytes[0] = 128;
        } else if (irRecBytes[0] != 0) {
            for (uint16_t i = 0; i < 400; i++)
                irRecBytes[i] = 0;
        }

        // Transmitter
        if (irSendMessageIndex != 0) {
            uint8_t byteNr = irSendBitIndex >> 3;
            uint8_t bitNr = irSendBitIndex & 7;

            uint8_t byte = 0;
            uint16_t maxIndex = 0;
            switch (irSendMessageIndex) {
//                case OPEN_BUTTON_MSG:
//                    byte = OPEN_BUTTON[byteNr];
//                    maxIndex = 8*OPEN_BUTTON_SIZE;
//                    break;
                case PWR_BUTTON_MSG:
                    byte = PWR_BUTTON[byteNr];
                    maxIndex = 8*PWR_BUTTON_SIZE;
                    break;
                case REP_BUTTON_MSG:
                    byte = REP_BUTTON[byteNr];
                    maxIndex = 8*REP_BUTTON_SIZE;
                    break;
                case VOL_UP_BUTTON_MSG:
                    byte = VOL_UP_BUTTON[byteNr];
                    maxIndex = 8*VOL_UP_BUTTON_SIZE;
                    break;
                case VOL_DOWN_BUTTON_MSG:
                    byte = VOL_DOWN_BUTTON[byteNr];
                    maxIndex = 8*VOL_DOWN_BUTTON_SIZE;
                    break;
            }

            if (testbit(byte, bitNr)) {
                IR_EN = 1;
            } else {
                IR_EN = 0;
            }


            irSendBitIndex++;
            if (irSendBitIndex >= maxIndex) {
                irSendBitIndex = 0;
                irSendMessageIndex = 0;
                IR_EN = 0;
            }
        }

    }


}