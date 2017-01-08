/*
 * Usage:
 * 
 *  1) include uarts.h and uarts.c in project
 *  2) in hardware.h, several options are available:
 *     - Write debug output to UART1/2:
 *         #define UART_DBG_PORT 1
       - Disable UART1:
 *         #define UART_DISABLE_PORT1
 *     - Disable UART2:
 *         #define UART_DISABLE_PORT2
 *  3) during initialization, call UxInit(baud, read, write) for desired ports
 *  4) handle reads on isr, call uartIsr()
 *  5) call uartsTick1000Hz() every 1ms
 *  6) when writes to port X need to be blocked for Y ms, increment uartXblock to Y
*/

#ifndef UARTS_H
#define	UARTS_H

#include "circular_buffer.h"
#include "hardware.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef UART_DISABLE_PORT1
uint32_t uart1BaudSetting;
uint16_t uart1BlockMs = 0; // nothing comes into buffer while block>0
uint16_t uart1DelayMs = 0; // nothing goes out of buffer while delay>0
CircularBuffer cbUart1Tx;

#ifdef UART1_RX_LOC
CircularBuffer cbUart1Rx @ UART1_RX_LOC;
#else
CircularBuffer cbUart1Rx;
#endif

// UART1: RPD receiver
#define U1Rx() cbRead( &cbUart1Rx )
#define U1RxReady() !(cbIsEmpty( &cbUart1Rx ) )
#define U1TxReady() ( PIR1bits.TX1IF )
#define U1Tx(ch) do { if (!uart1BlockMs) cbWrite( &cbUart1Tx, ch ) } while (0);
void U1TxSync(char ch);
void U1puts(const char* msg);
void U1putsSync(const char* msg);
void U1Init(uint32_t baudrate, bool read, bool write);
void U1SetBaudrate(uint32_t baudrate);
void U1Disable();
#endif


#ifndef UART_DISABLE_PORT2
uint32_t uart2BaudSetting;
uint16_t uart2BlockMs = 0; // nothing comes into buffer while block>0
uint16_t uart2DelayMs = 0; // nothing goes out of buffer while delay>0
CircularBuffer cbUart2Tx;

#ifdef UART2_RX_LOC
CircularBuffer cbUart2Rx @ UART2_RX_LOC;
#else
CircularBuffer cbUart2Rx;
#endif

// UART2: EasyRadio transceiver
#define U2Rx() cbRead( &cbUart2Rx )
#define U2RxReady() !(cbIsEmpty( &cbUart2Rx ) )
#define U2TxReady() ( TXSTA2bits.TRMT )
#define U2Tx(ch) do { if (!uart2BlockMs) cbWrite( &cbUart2Tx, ch ); } while (0);
void U2TxSync(char ch);
void U2puts(const char* msg);
void U2putsSync(const char* msg);
void U2Init(uint32_t baudrate, bool read, bool write);
void U2SetBaudrate(uint32_t baudrate);
void U2Disable();
#endif


// inlining only works if the definition is done in the header file
#ifndef UART_DISABLE_PORT1
#define uart1Isr() do { \
    if (PIR1bits.RC1IF) { \
        uint8_t b = RCREG1; \
        if ((RCSTA1 & 0b00000110) == 0) { \
            *cbUart1Rx.w++ = b; \
            if (cbUart1Rx.w == cbUart1Rx.buf_end) \
                cbUart1Rx.w = cbUart1Rx.buf; \
        } \
        else { \
            RCSTA1bits.CREN = 0; \
            RCSTA1bits.CREN = 1; \
        } \
    } \
    } while (0);
#else
#define uart1Isr() do { } while (0);
#endif

#ifndef UART_DISABLE_PORT2
#define uart2Isr() do { \
    if (PIR3bits.RC2IF) { \
        char c = RCREG2; \
        if ((RCSTA2 & 0b00000110) == 0) { \
            *(cbUart2Rx.w++) = c; \
            if (cbUart2Rx.w == cbUart2Rx.buf_end) \
                cbUart2Rx.w = cbUart2Rx.buf; \
        } \
        else { \
            RCSTA2bits.CREN = 0; \
            RCSTA2bits.CREN = 1; \
        } \
    } \
    } while (0);
#else
#define uart2Isr() do { } while (0);
#endif

void uartsTick1000Hz();

#define uartsIsr() do { uart1Isr(); uart2Isr(); } while (0);

#ifndef UARTS_INTERRUPT_PRIORITY
#define UARTS_INTERRUPT_PRIORITY 1
#endif


#if defined(__18F24K80) || defined(__18LF24K80) || defined(__18LF25K80) || defined(__18F25K80)
#define TRIS_UART1TX TRISCbits.TRISC6
#define TRIS_UART1RX TRISCbits.TRISC7
#define TRIS_UART2TX TRISBbits.TRISB6
#define TRIS_UART2RX TRISBbits.TRISB7

#elif defined(__18F45K80) || defined(__18LF45K80) || defined(__18F46K80) || defined(__18LF46K80)
#define TRIS_UART1TX TRISCbits.TRISC6
#define TRIS_UART1RX TRISCbits.TRISC7
#define TRIS_UART2TX TRISDbits.TRISD6
#define TRIS_UART2RX TRISDbits.TRISD7

#elif defined(__16F88) || defined(__16LF88)
#define TRIS_UART1TX TRISCbits.TRISC6
#define TRIS_UART1RX TRISCbits.TRISC7
#define TRIS_UART2TX TRISBbits.TRISB6
#define TRIS_UART2RX TRISBbits.TRISB7

#elif defined(__18F2550) || defined(__18LF2550) || defined(__18F4550) || defined(__18LF4550) || defined(__18F4455) || defined(__18LF4455)
#define TRIS_UART1TX TRISCbits.TRISC6
#define TRIS_UART1RX TRISCbits.TRISC7

#else
#error "uart pins unknown"
#endif

#ifdef UART_DBG_PORT
#include "uarts_debug.h"

#if UART_DBG_PORT == 1
#define dbgsInit(a) U1Init(a, true, true)
#define dbgb(x) U1TxSync(x)
#define dbgs(x) U1putsSync(x)
#define dbgsval(x) U1writeVal(x)
#define dbgsval32(x) U1writeVal32(x)
#define dbgsvalx(x) U1writeValHex(x)
#define dbgsvalf(x) U1writeValFloat(x)
#define dbgsvald(x) U1writeValDouble(x)
#define dbgsx(x) U1putxSync(x)

#elif UART_DBG_PORT == 2
#define dbgsInit(a) U2Init(a, true, true)
#define dbgb(x) U2TxSync(x)
#define dbgs(x) U2putsSync(x)
#define dbgsval(x) U2writeVal(x)
#define dbgsval32(x) U2writeVal32(x)
#define dbgsvalx(x) U2writeValHex(x)
#define dbgsvalf(x) U2writeValFloat(x)
#define dbgsvald(x) U2writeValDouble(x)
#define dbgsx(x) U2putxSync(x)

#else
#define dbgsInit(a) 
#define dbgb(x)
#define dbgs(x)
#define dbgsval(x)
#define dbgsval32(x)
#define dbgsvalx(x)
#define dbgsvalf(x)
#define dbgsvald(x)
#define dbgsx(x)
#endif

#else
#define dbgsInit(a) 
#define dbgb(x)
#define dbgs(x)
#define dbgsval(x)
#define dbgsval32(x)
#define dbgsvalx(x)
#define dbgsvalf(x)
#define dbgsvald(x)
#define dbgsx(x)
#endif

#endif	/* UARTS_H */
