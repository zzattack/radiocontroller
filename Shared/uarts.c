#include "uarts.h"
#include <string.h>
#include <stdio.h>
#include <xc.h>

#ifndef UART_DISABLE_PORT1

void U1Init(uint32_t baudrate, bool read, bool write) {
#ifndef CIRCBUF_MANUAL_ALLOC
    // Setup circular buffers
    cbInit(&cbUart1Rx);
    cbInit(&cbUart1Tx);
#endif
    
    TRIS_UART1TX = 0;
    TRIS_UART1RX = 1;

    // setup TXSTA/RCSTA for UART1
    TXSTA1 = 0;
    TXSTA1bits.TX9 = 0;  // Selects 8-bit transmission
    TXSTA1bits.TXEN = write; // Transmit enabled

    RCSTA1bits.OERR = 0; // Clear errors
    RCSTA1bits.FERR = 0;
    RCSTA1bits.SPEN = 1; // Serial Port Enable bit
    RCSTA1bits.RX9 = 0;  // Selects 8-bit reception
    RCSTA1bits.CREN = read; // Enables continuous receive

    // setup interrupts
#ifndef UART_DISABLE_INTERRUPTS
    PIE1bits.RC1IE = read; // Enable EUSART1 receive interrupt
    IPR1bits.RC1IP = UARTS_INTERRUPT_PRIORITY;
    PIR1bits.RC1IF = 0; // Clear interrupt flag
#else
	PIE1bits.RC1IE = 0;
#endif
    
    U1SetBaudrate(baudrate);
    // U1CTS = READY;
}

void U1SetBaudrate(uint32_t baudrate) {
    while (!U1TxReady()) ClrWdt(); // wait for buffer to flush
    TXSTA1bits.SYNC = 0;    // Asynchronous
    TXSTA1bits.BRGH = 1;    // High speed
    BAUDCONbits.BRG16 = 1; // 16-Bit Baud Rate Register
    uint16_t baud = _XTAL_FREQ / (4 * (baudrate + 1));
    SPBRG = baud & 0xFF;
    SPBRGH = baud >> 8;
    uart1BaudSetting = baudrate;
}

void U1Disable() {
    RCSTA1 &= 0b01001111;  // Disable the receiver
    TXSTA1bits.TXEN = 0;   // and transmitter
    PIE1 &= 0b11001111;   // Disable both interrupts
    cbClear(&cbUart1Rx);
}

// queue data to be transmitted
void U1puts(const char* msg) {
    if (uart1BlockMs > 0) return;
    while (*msg != '\0')
        cbWrite(&cbUart1Tx, *msg++);
}

// synchronously data exchange
void U1TxSync(char ch) {
    while (!U1TxReady()) ClrWdt();
    TXREG1 = ch;
}
void U1putsSync(const char* msg) {
    while (*msg != '\0') U1TxSync(*msg++);
}

#endif

#ifndef UART_DISABLE_PORT2
void U2puts(const char* msg) {
    if (uart2BlockMs > 0) return;
    while (*msg != '\0')
        cbWrite(&cbUart2Tx, *msg++);
}

void U2TxSync(char ch) {
    while (!U2TxReady()) ClrWdt();
    TXREG2 = ch;
}
void U2putsSync(const char* msg) {
    while (*msg != '\0') U2TxSync(*msg++);
}


void U2Init(uint32_t baudrate, bool read, bool write) {
#ifndef CIRCBUF_MANUAL_ALLOC
    // Setup circular buffers
    cbInit(&cbUart2Rx);
    cbInit(&cbUart2Tx);
#endif
    
    TRIS_UART2TX = 0; // TX UART2
    TRIS_UART2RX = 1; // RX UART2

    // setup TXSTA/RCSTA for UART2
    TXSTA2 = 0;
    TXSTA2bits.TX9 = 0;  // Selects 8-bit transmission
    TXSTA2bits.TXEN = write; // Transmit enabled

    RCSTA2bits.OERR = 0; // Clear errors
    RCSTA2bits.FERR = 0;
    RCSTA2bits.SPEN = 1; // Serial Port Enable bit
    RCSTA2bits.RX9 = 0;  // Selects 8-bit reception
    RCSTA2bits.CREN = read; // Enables continuous receive

    // setup interrupts
#ifndef UART_DISABLE_INTERRUPTS
    PIE3bits.RC2IE = read; // Enable EUSART2 receive interrupt
    IPR3bits.RC2IP = UARTS_INTERRUPT_PRIORITY; 
    PIR3bits.RC2IF = 0; // Clear interrupt flag
#else
	PIE3bits.RC2IE = 0;
#endif

    U2SetBaudrate(baudrate);
}

void U2SetBaudrate(uint32_t baudrate) {
    while (!U2TxReady()) ClrWdt(); // wait for buffer to flush
    TXSTA2bits.SYNC = 0;    // Asynchronous
    TXSTA2bits.BRGH = 1;    // High speed
    BAUDCON2bits.BRG16 = 1; // 16-Bit Baud Rate Register

    uint16_t baud = _XTAL_FREQ / (4 * (baudrate + 1));
    SPBRG2 = baud & 0xFF;    // SPBRG 416 --> baud 38400
    SPBRGH2 = baud >> 8;     // SPRGB 138 --> baud 115200
    uart2BaudSetting = baudrate;
}

void U2Disable() {
    RCSTA2 &= 0b01001111;  // Disable the receiver
    TXSTA2bits.TXEN = 0;   // and transmitter
    PIE3 &= 0b11001111;   // Disable both interrupts
    cbClear(&cbUart2Rx);
}

#endif

void uartsTick1000Hz() {
#ifndef UART_DISABLE_PORT1
    if (uart1BlockMs > 0) uart1BlockMs--;
    if (uart1DelayMs > 0) uart1DelayMs--;
#endif
#ifndef UART_DISABLE_PORT2
    if (uart2BlockMs > 0) uart2BlockMs--;
    if (uart2DelayMs > 0) uart2DelayMs--;
#endif
}
