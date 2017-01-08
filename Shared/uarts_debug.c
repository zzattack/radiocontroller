#include "uarts_debug.h"
#include "uarts.h"
#include <stdio.h>

#ifdef UART_DBG_PORT

#ifndef UART_DISABLE_PORT1
// debug stuff
void U1writeVal(uint16_t val) {
    char buff[10];
    sprintf(buff, "%u", val);
    U1putsSync(buff);
}
void U1writeVal32(uint32_t val) {
    char buff[10];
    sprintf(buff, "%lu", val);
    U1putsSync(buff);
}
void U1writeValHex(uint16_t val) {
    char buff[10];
    sprintf(buff, "%x", val);
    U1putsSync(buff);
}
void U1writeValFloat(float val) {
    uint8_t buff[10];
    sprintf(buff, "%f", val);
    U1putsSync(buff);
}
void U1writeValDouble(double val) {
    uint8_t buff[10];
    sprintf(buff, "%.10f", val);
    U1putsSync(buff);
}
void U1putxSync(const char* msg) {
    while (*msg != '\0') { U1writeValHex(*msg++); U1TxSync(' '); }
}

#endif

#ifndef UART_DISABLE_PORT2

void U2writeVal(uint16_t val) {
    uint8_t buff[10];
    sprintf(buff, "%u", val);
    U2putsSync(buff);
}
void U2writeVal32(uint32_t val) {
    uint8_t buff[10];
    sprintf(buff, "%u", val);
    U2putsSync(buff);
}
void U2writeValHex(uint16_t val) {
    uint8_t buff[10];
    sprintf(buff, "%x", val);
    U2putsSync(buff);
}
void U2writeValFloat(float val) {
    uint8_t buff[10];
    sprintf(buff, "%f", val);
    U2putsSync(buff);
}
void U2writeValDouble(double val) {
    uint8_t buff[10];
    sprintf(buff, "%.10f", val);
    U2putsSync(buff);
}
void U2putxSync(const char* msg) {
    while (*msg != '\0') { U2writeValHex(*msg++); U2TxSync(' '); }
}
#endif

#endif