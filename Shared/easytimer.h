#ifndef _EASYTIMER_H
#define _EASYTIMER_H

#include <stdint.h>

volatile uint8_t EasyTimerTick;
void tick1000HzInternal();
extern void tickTimer1000Hz();
extern void tickTimer100Hz();
extern void tickTimer10Hz();
extern void tickTimer1Hz();

#endif