/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "bustranceiver.h"

/******************************************************************************/
/* Private variables                                                          */
/******************************************************************************/

// Receiver
#ifdef BUS_RCVR
uint16_t BUSTRCV_PRIVATE_BANK1 currentCRCIn = 0;
uint8_t BUSTRCV_PRIVATE_BANK1 packetLength = 0;
uint8_t BUSTRCV_PRIVATE_BANK1 currentIndex = 0;

uint8_t BUSTRCV_PRIVATE_BANK1 receiveTimeOut = 0;

bit BUSTRCV_PRIVATE_BANK1 escapeActive = false;
bit BUSTRCV_PRIVATE_BANK1 receiving = false;
bit BUSTRCV_PRIVATE_BANK1 newData = false;

uint8_t BUSTRCV_PRIVATE_BANK1 crcHigh = 0;

uint8_t BUSTRCV_PRIVATE_BANK1 command;
uint8_t BUSTRCV_PRIVATE_BANK1 commandDataSize;
uint8_t BUSTRCV_PRIVATE_BANK1 commandData[COMMAND_BUFFER_MAXSIZE];

# ifdef BUS_SLAVE
bool BUSTRCV_PRIVATE_BANK1 checkSlave;
# endif
#else
# ifdef BUS_SLAVE
#error Cannot use slave mode without receiver!
# endif
#endif

// Transmitter
#ifdef BUS_TRMT
uint16_t BUSTRCV_PRIVATE_BANK2 currentCRCOut = 0;
bit BUSTRCV_PRIVATE_BANK2 outputSending;
bit BUSTRCV_PRIVATE_BANK2 escaping;
uint8_t BUSTRCV_PRIVATE_BANK2 outputBuffer[OUTPUT_BUFFER_MAXSIZE];
uint8_t BUSTRCV_PRIVATE_BANK2 outputBufferSize;
uint8_t BUSTRCV_PRIVATE_BANK2 outputBufferIndex;
#endif

// Private function variables allocated in the private bank
#ifdef BUS_RCVR
// receiveComm()
uint8_t BUSTRCV_PRIVATE_BANK1 data = 0;
#endif

// updateCRC() (both)
uint8_t BUSTRCV_PRIVATE_BANK1 bt = 0;
uint8_t BUSTRCV_PRIVATE_BANK1 lsb = 0;

#ifdef BUS_TRMT
// processOutput()
uint8_t BUSTRCV_PRIVATE_BANK2 bToSend = 0;
#endif

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

// Receiver
#ifdef BUS_RCVR
void busUpdateCRCIn(uint8_t data);
void busExtractPacketData();
#endif

// Transmitter
#ifdef BUS_TRMT
void busUpdateCRCOut(uint8_t data);
#endif

/******************************************************************************/
/* Function Implementations                                                   */
/******************************************************************************/

void busCheckComm()
{
#ifdef BUS_RCVR
    if (RCIF) { // New data is available
        busReceiveComm();
    }
# ifdef BUS_SLAVE
    if (checkSlave) {
        checkSlave = false;
        if (command == CMD_CC_DISCOVER) {
            uint8_t devBuf[1];
            devBuf[0] = BUS_SLAVE;
            busTransmit(CMD_CC_DISCRESP, devBuf, 1);
#  ifdef BUS_SLAVE_NOFILTER
        }
#  else
        } else if (command >> 4 == BUS_SLAVE)
#  endif
        newData = true;
    }
# endif
    if (OERR) { // Overrun error
        CREN = 0;
        CREN = 1;
    }
    if (FERR) { // Framing error
        RCREG; // Read it to clear the error
    }
#endif
#ifdef BUS_TRMT
    if (TXIF) { // New data is available
        busProcessOutput();
    }
#endif
}

#ifdef BUS_RCVR
bool busNewDataAvailable()
{
    uint8_t result = (newData && busCommand.Valid);
    newData = false;
    return result;
}

void busUpdateTimeout() // @ BUSTRCV_TIMER_HZ Hz
{
    if (receiveTimeOut > 0) {
        receiveTimeOut--;
        if (receiveTimeOut == 0) {
            receiving = false;
            escapeActive = false;
        }
    }
}

void busReceiveComm()
{
    data = RCREG;
#ifdef BUS_INTERRUPTS
            RCIE = 0;
#endif

    if (!escapeActive && data == ESC) {
        escapeActive = true;
    } else if (!escapeActive && data == STX) {
        currentIndex = 0;
        receiveTimeOut = BUSTRCV_TIMER_HZ/4;
        receiving = true;
        command = 0;
        commandDataSize = 0;
        currentCRCIn = START_CRC;
        busUpdateCRCIn(data);
    } else if (receiving) {
        receiveTimeOut = BUSTRCV_TIMER_HZ/4;

        if (currentIndex == 0) {
            packetLength = data;
            busUpdateCRCIn(data);
            if (packetLength < 3)
                receiving = false;
        } else if (currentIndex > packetLength) {
            receiving = false;
        } else if (currentIndex == packetLength-1) {
            crcHigh = data;
            currentCRCIn ^= FINAL_CRC;
            if ((uint8_t)(currentCRCIn >> 8) != crcHigh)
                receiving = false;
        } else if (currentIndex == packetLength) {
            receiving = false;
            if ((uint8_t)(currentCRCIn & 0xFF) == data) {
                busExtractPacketData();
#ifdef BUS_SLAVE
                checkSlave = true;
#else
                newData = true;
#endif
                receiveTimeOut = 0;
            }
        } else {
            busUpdateCRCIn(data);
            if (currentIndex <= 1) {
                command = data;
            } else if (currentIndex < COMMAND_BUFFER_MAXSIZE+2) {
                commandData[currentIndex-2] = data;
                commandDataSize = currentIndex-1;
            }
        }
        currentIndex++;
        escapeActive = false;
    } else {
        escapeActive = false;
    }
#ifdef BUS_INTERRUPTS
            RCIE = 1;
#endif
}

void busUpdateCRCIn(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        bt = (data & 0x01);
        lsb = (currentCRCIn & 0x01);
        currentCRCIn >>= 1;
        if ((bt ^ lsb) == 0x01) {
                currentCRCIn ^= POLY_CRC;
        }
        data >>= 1;
    }
}

void busExtractPacketData()
{

    busCommand.Valid = false;

    busCommand.Command = command;
    for (uint8_t i = 0; i < commandDataSize; i++) {
        busCommand.CommandData[i] = commandData[i];
    }
    busCommand.CommandDataSize = commandDataSize;

    busCommand.Valid = true;

}
#endif

#ifdef BUS_TRMT
void busTransmitSync(uint8_t command, const uint8_t *commandData, uint8_t commandDataSize)
{
    busTransmit(command, commandData, commandDataSize);
    while(busOutputBusy()) {
        busCheckComm();
    }
}

void busTransmit(uint8_t command, const uint8_t *commandData, uint8_t commandDataSize)
{
    if (outputSending)
        return;
    
    currentCRCOut = START_CRC;

    outputBuffer[0] = STX;
    busUpdateCRCOut(outputBuffer[0]);

    outputBuffer[1] = commandDataSize+3;
    busUpdateCRCOut(outputBuffer[1]);

    outputBuffer[2] = command;
    busUpdateCRCOut(outputBuffer[2]);

    for (uint8_t i = 0; i < commandDataSize; i++) {
        outputBuffer[i+3] = commandData[i];
        busUpdateCRCOut(outputBuffer[i+3]);
    }

    currentCRCOut ^= FINAL_CRC;

    outputBuffer[commandDataSize+3] = currentCRCOut >> 8;
    outputBuffer[commandDataSize+4] = currentCRCOut & 0xFF;

    outputBufferIndex = 0;
    outputBufferSize = commandDataSize+5;
    escaping = true;
    outputSending = true;
#ifdef BUS_INTERRUPTS
            TXIE = 1;
#endif

}

bool busOutputBusy()
{
    return outputSending;
}

void busProcessOutput()
{
#ifdef BUS_INTERRUPTS
            TXIE = 0;
#endif
    if (outputSending) {
        if (outputBufferIndex < outputBufferSize) {
            bToSend = outputBuffer[outputBufferIndex];
            if (!escaping && (bToSend == STX || bToSend == ESC)) {
                bToSend = ESC;
                escaping = true;
            } else {
                outputBufferIndex++;
                escaping = false;
            }
            TXREG = bToSend;
#ifdef BUS_INTERRUPTS
            TXIE = 1;
#endif
        } else {
            outputSending = false;
        }
    }
}

void busUpdateCRCOut(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        bt = (data & 0x01);
        lsb = (currentCRCOut & 0x01);
        currentCRCOut >>= 1;
        if ((bt ^ lsb) == 0x01) {
                currentCRCOut ^= POLY_CRC;
        }
        data >>= 1;
    }
}
#endif
