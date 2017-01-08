#include "circular_buffer.h"

#ifdef CIRCBUF_MANUAL_ALLOC
void cbInit(CircularBuffer* cb, uint8_t* buf, uint8_t size) {
    cb->buf = buf;
    cb->r = cb->buf;
    cb->w = cb->buf;
    cb->buf_end = cb->buf + size;
}
#else 
void cbInit(CircularBuffer* cb) {
    cb->r = cb->buf;
    cb->w = cb->buf;
    cb->buf_end = cb->buf + CIRCBUF_SIZE;
}
#endif

// Write an element, overwriting oldest element if buffer is full-> App can
//   choose to avoid the overwrite by checking cbIsFull()
void cbWrite(CircularBuffer* cb, unsigned char data) {
    *(cb->w++) = data;
    if (cb->w == cb->buf_end)
        cb->w = cb->buf;
}

unsigned char cbRead(CircularBuffer* cb) {
    unsigned char data = *(cb->r++);
    if (cb->r == cb->buf_end)
        cb->r = cb->buf;
    return data;
}

void cbClear(CircularBuffer* cb) {
    cb->r = cb->buf;
    cb->w = cb->buf;
}
