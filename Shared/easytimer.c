#include "easytimer.h"

void tick1000HzInternal() {
    static uint8_t timer100hz = 0;
    static uint8_t timer10hz = 0;
    static uint16_t timer1hz = 0;

    tickTimer1000Hz();
    timer100hz++;
    if (timer100hz >= 10) {
        tickTimer100Hz();
        timer100hz = 0;
        timer1hz++;
        timer10hz++;

        if (timer10hz >= 10) {
            tickTimer10Hz();
            timer10hz = 0;
        }
        if (timer1hz >= 100) {
            tickTimer1Hz();
            timer1hz = 0;
        }
    }
}

