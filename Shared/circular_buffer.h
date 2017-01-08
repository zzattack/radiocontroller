/*
 * Usage:
 * 
 *  1) include circular_buffer.h and circular_buffer.c in project
 *  2) in hardware.h, define buffer size, defaults to 128:
 *       #define CIRCBUF_SIZE 128
 *  3) call cbInit() for every buffer upon creation
 *  4) call cbWrite() whenever needed - write pointer is volatile
 *  5) read only from main program loop - read pointer is non-volatile
*/

#ifndef CIRCULARBUFFER_H
#define	CIRCULARBUFFER_H

#include <stdint.h>
#include "hardware.h"

#ifndef CIRCBUF_SIZE
#define CIRCBUF_SIZE 128 // set me in hardware.h
#endif

typedef struct {
#ifdef CIRCBUF_MANUAL_ALLOC
    uint8_t* buf;
#else
    uint8_t buf[CIRCBUF_SIZE];
#endif
    uint8_t* buf_end;
    volatile uint8_t* w; // head
    uint8_t* r; // tail
} CircularBuffer;


#ifdef CIRCBUF_MANUAL_ALLOC
void cbInit(CircularBuffer* cb, uint8_t* buf, uint8_t size);
#else 
void cbInit(CircularBuffer* cb);
#endif

#define cbIsEmpty(cb) ((cb)->w==(cb)->r)
void cbWrite(CircularBuffer* cb, unsigned char data);
unsigned char cbRead(CircularBuffer* cb);
void cbClear(CircularBuffer* cb);


#endif	/* CIRCULARBUFFER_H */