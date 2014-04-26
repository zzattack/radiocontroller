#ifndef BUTTONCHECKER_H
#define	BUTTONCHECKER_H

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "basic.h"          /* For the basic definitions */
#include "hardware.h"       /* For the port mappings */

/******************************************************************************/
/* Public variables                                                           */
/******************************************************************************/

#ifdef LONG_BUTTON_TIME
typedef struct {
    uint16_t totalDownTime;
#ifdef LONG_TIMES
    uint16_t downTime;
    uint16_t upTime;
#else
    uint8_t downTime;
    uint8_t upTime;
#endif
    uint8_t state  : 1;
    uint8_t down : 1;
    uint8_t repeat : 1;
    uint8_t pressed : 1;
    uint8_t firstpressed : 1;
    uint8_t released : 1;
    uint8_t longpressed : 1;
} ButtonState;
#else
typedef struct {
#ifdef LONG_TIMES
    uint16_t downTime;
    uint16_t upTime;
#else
    uint8_t downTime;
    uint8_t upTime;
#endif
    uint8_t state  : 1;
    uint8_t down : 1;
    uint8_t repeat : 1;
    uint8_t pressed : 1;
    uint8_t firstpressed : 1;
    uint8_t released : 1;
    uint8_t longpressed : 1;
} ButtonState;
#endif

#ifndef BUTTON_BANK
#define BUTTON_BANK
#endif

#ifndef BUTTON_TIMER_HZ
#define BUTTON_TIMER_HZ 100
#endif

#if defined(NORMAL_BUTTONS) && defined(MATRIX_BUTTONS)
#error Cannot use both button modes simultaneously!
#endif

#ifdef MATRIX_BUTTONS
# define BUTTON_COUNT (BUTTON_COL_COUNT*BUTTON_ROW_COUNT)
#endif

#if defined(NORMAL_BUTTONS) || defined(MATRIX_BUTTONS)
ButtonState buttonState[BUTTON_COUNT];
#endif

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

#if defined(NORMAL_BUTTONS) || defined(MATRIX_BUTTONS)
void initButtons();
// Status codes:
// 0 = No change
// bit 0 = new press (not repeated)
// bit 1 = repeated press
// bit 2 = release
// bit 3 = long press
uint8_t checkButtons();        /* Check if a button is pressen (should be 100Hz) */
#endif

#endif	/* BUTTONCHECKER_H */

