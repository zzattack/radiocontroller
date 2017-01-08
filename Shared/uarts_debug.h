#ifndef UARTS_DEBUG_H
#define	UARTS_DEBUG_H

#include "hardware.h"
#include <stdint.h>

#ifdef UART_DBG_PORT

#ifndef UART_DISABLE_PORT1
void U1writeVal(uint16_t val);
void U1writeVal32(uint32_t val);
void U1writeValHex(uint16_t val);
void U1writeValFloat(float val);
void U1writeValDouble(double val);
void U1putxSync(const char* msg);
#endif

#ifndef UART_DISABLE_PORT2
void U2writeVal(uint16_t val);
void U2writeVal32(uint32_t val);
void U2writeValHex(uint16_t val);
void U2writeValFloat(float val);
void U2writeValDouble(double val);
void U2putxSync(const char* msg);
#endif

#endif


#endif	/* UARTS_DEBUG_H */

