/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "globals.h"
#define RFID_PRIVATE_BANK bank0

/******************************************************************************/
/* Private variables                                                          */
/******************************************************************************/

#ifdef RFID_SW_UART

bit RFID_SW_RCIF = 0;         // Interrupt
uint8_t RFID_SW_RCC = 0;      // Counter

uint8_t RFID_SW_RCREG = 0;    // Byte
uint8_t RFID_SW_RCREGC = 0;   // Copy

#endif

// Receiver
uint8_t RFID_PRIVATE_BANK rfidReceiveTimeOut = 0;

bit RFID_PRIVATE_BANK rfidReceiving = false;
bit RFID_PRIVATE_BANK rfidNewData = false;
uint8_t RFID_PRIVATE_BANK rfidCurrentIndex = 0;

uint8_t RFID_PRIVATE_BANK rfidTagData[12];
uint8_t RFID_PRIVATE_BANK rfidTagIndex;

// receiveComm()
uint8_t RFID_PRIVATE_BANK rfidData = 0;

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

#ifdef RFID_SW_UART

void rfidSwInt()
{
    if (RFID_SW_RCC == 0) {                 // If IDLE
        if (!RFID_DATA)                     // If Startbit
            RFID_SW_RCC = 1;                // Start reception
    } else if (RFID_SW_RCC == 9) {          // Stopbit
        if (RFID_DATA && !RFID_SW_RCIF) {   // If bus is high and previous byte was processed (prevent overrun of framing error)
            RFID_SW_RCREG = RFID_SW_RCREGC; // Copy
            RFID_SW_RCIF = 1;               // Byte is correct
        }
        RFID_SW_RCC = 0;                    // Reset
    } else {
        RFID_SW_RCREGC >>= 1;               // Shift
        if (RFID_DATA)                      // Read bit
            setbit(RFID_SW_RCREGC, 7);      // Set if high
        RFID_SW_RCC++;                      // Advance
    }
}

#endif

// Receiver
void rfidReceiveComm();         // Receive the next available byte
void rfidExtractTagData();


/******************************************************************************/
/* Function Implementations                                                   */
/******************************************************************************/

void rfidCheckComm()
{
# ifdef RFID_SW_UART
    if (RFID_SW_RCIF) { // New rfidData is available
        rfidReceiveComm();
    }
# else
    if (RCIF) { // New rfidData is available
        rfidReceiveComm();
    }
    if (OERR) { // Overrun error
        CREN = 0;
        CREN = 1;
    }
    if (FERR) { // Framing error
        RFID_SW_RCREG; // Read it to clear the error
    }
#endif
}

bool rfidNewCardAvailable()
{
    if (rfidNewData) {
        rfidNewData = false;
        return rfidTagIndex;
    }
    return 255;
}

void rfidUpdateTimeout() // @ 100Hz
{
    if (rfidReceiveTimeOut > 0) {
        rfidReceiveTimeOut--;
        if (rfidReceiveTimeOut == 0) {
            rfidCurrentIndex = 0;
        }
    }
}

void rfidReceiveComm()
{
# ifdef RFID_SW_UART
    rfidData = RFID_SW_RCREG;
    RFID_SW_RCIF = 0;
# else
    rfidData = RCREG;
#endif


    if (rfidCurrentIndex != 0) {
        rfidReceiveTimeOut = 100;

        if (rfidCurrentIndex == 13) {
            if (rfidData != 13)
                rfidCurrentIndex = 0;
            else
                rfidCurrentIndex++;
        } else if (rfidCurrentIndex == 14) {
            if (rfidData != 10)
                rfidCurrentIndex = 0;
            else
                rfidCurrentIndex++;
        } else if (rfidCurrentIndex == 15) {
            if (rfidData == 3) {
                rfidExtractTagData();
            }
            rfidCurrentIndex = 0;
        } else {
            rfidTagData[rfidCurrentIndex-1] = rfidData;
            rfidCurrentIndex++;
        }

    } else {
        LED_RED = !LED_RED;
        statusData[0] = rfidData;
        busTransmit(CMD_RFID_STATUS, statusData, 1);
        if (rfidData == 2) {
            rfidCurrentIndex = 1;
        }
    }
}

void rfidExtractTagData()
{
    for (uint8_t i = 0; i < RFID_TAG_COUNT; i++) {
        bool ok = true;
        for (uint8_t j = 0; j < 12; j++) {
            if (RFID_TAGS[i][j] != rfidTagData[j]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            rfidTagIndex = i;
            rfidNewData = true;
            break;
        }
    }

}