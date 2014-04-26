/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "buttonchecker.h"

#if defined(NORMAL_BUTTONS) || defined(MATRIX_BUTTONS)
void initButtons()
{
#ifdef NORMAL_BUTTONS
    for (int i = 0; i < BUTTON_COUNT; i++) {
#endif
#ifdef MATRIX_BUTTONS
    for (int i = 0; i < BUTTON_ROW_COUNT*BUTTON_COL_COUNT; i++) {
#endif

        buttonState[i].state = 1;
        buttonState[i].upTime = 255;
        buttonState[i].down = false;
        
    }
}

uint8_t checkButtons()
{
    bool result = 0;
    bool button[BUTTON_COUNT];

#ifdef NORMAL_BUTTONS

#if BUTTON_COUNT >= 1
    button[0] = BUTTON_0;
#endif
#if BUTTON_COUNT >= 2
    button[1] = BUTTON_1;
#endif
#if BUTTON_COUNT >= 3
    button[2] = BUTTON_2;
#endif
#if BUTTON_COUNT >= 4
    button[3] = BUTTON_3;
#endif
#if BUTTON_COUNT >= 5
    button[4] = BUTTON_4;
#endif
#if BUTTON_COUNT >= 6
    button[5] = BUTTON_5;
#endif
#if BUTTON_COUNT >= 7
    button[6] = BUTTON_6;
#endif
#if BUTTON_COUNT >= 8
    button[7] = BUTTON_7;
#endif
#if BUTTON_COUNT >= 9
    button[8] = BUTTON_8;
#endif
#if BUTTON_COUNT >= 10
    button[9] = BUTTON_9;
#endif
#endif

#ifdef MATRIX_BUTTONS
    for (int row = 0; row < BUTTON_ROW_COUNT; row++) {

        switch(row) {
#if BUTTON_ROW_COUNT >= 1
            case 0:
                BUTTON_TRIS_ROW0 = BUTTON_TRISV_ROW0;
                BUTTON_ROW_0 = 0;
                break;
#endif
#if BUTTON_ROW_COUNT >= 2
            case 1:
                BUTTON_TRIS_ROW1 = BUTTON_TRISV_ROW1;
                BUTTON_ROW_1 = 0;
                break;
#endif
#if BUTTON_ROW_COUNT >= 3
            case 2:
                BUTTON_TRIS_ROW2 = BUTTON_TRISV_ROW2;
                BUTTON_ROW_2 = 0;
                break;
#endif
#if BUTTON_ROW_COUNT >= 4
            case 3:
                BUTTON_TRIS_ROW3 = BUTTON_TRISV_ROW3;
                BUTTON_ROW_3 = 0;
                break;
#endif
#if BUTTON_ROW_COUNT >= 5
            case 4:
                BUTTON_TRIS_ROW4 = BUTTON_TRISV_ROW4;
                BUTTON_ROW_4 = 0;
                break;
#endif
        }

#if BUTTON_COL_COUNT >= 1
        button[row*BUTTON_COL_COUNT+0] = BUTTON_COL_0;
#endif
#if BUTTON_COL_COUNT >= 2
        button[row*BUTTON_COL_COUNT+1] = BUTTON_COL_1;
#endif
#if BUTTON_COL_COUNT >= 3
        button[row*BUTTON_COL_COUNT+2] = BUTTON_COL_2;
#endif
#if BUTTON_COL_COUNT >= 4
        button[row*BUTTON_COL_COUNT+3] = BUTTON_COL_3;
#endif
#if BUTTON_COL_COUNT >= 5
        button[row*BUTTON_COL_COUNT+4] = BUTTON_COL_4;
#endif
#endif

#ifdef NORMAL_BUTTONS
        for (int i = 0; i < BUTTON_COUNT; i++) {
#endif
#ifdef MATRIX_BUTTONS
        for (int col = 0; col < BUTTON_COL_COUNT; col++) {
            uint8_t i = row*BUTTON_COL_COUNT+col;
#endif

            // Button release
            if (button[i] == 1 && buttonState[i].state == 0 && buttonState[i].downTime >= 5) {
                buttonState[i].upTime = 0;
            }

            // Button press
            if (button[i] == 0 && buttonState[i].state == 1 && buttonState[i].upTime >= 5) {
# ifdef LONG_BUTTON_TIME
                buttonState[i].totalDownTime = 0;
# endif
                buttonState[i].downTime = 0;
                buttonState[i].repeat = 0;
            }

            // Button pressed
            if (button[i] == 0 && buttonState[i].downTime == 5 && buttonState[i].upTime >= 5) {
                buttonState[i].pressed = true;
                buttonState[i].down = true;
                if (buttonState[i].repeat) {
                    setbit(result, 1);
                } else {
                    buttonState[i].firstpressed = true;
                    setbit(result, 0);
                }
            }

            // Button released
            if (button[i] == 1 && buttonState[i].upTime == 5 && buttonState[i].downTime >= 5) {
                buttonState[i].released = true;
                buttonState[i].down = false;
                setbit(result, 2);
            }

            buttonState[i].state = button[i];

#ifdef LONG_BUTTON_TIME
            if (!buttonState[i].state && buttonState[i].totalDownTime < 65535) {
                buttonState[i].totalDownTime++;
                if (buttonState[i].totalDownTime == LONG_BUTTON_TIME) {
                    buttonState[i].longpressed = true;
                    setbit(result, 3);
                }
            }
#endif

            if (!buttonState[i].state)
                buttonState[i].downTime++;
            if (buttonState[i].downTime == (BUTTON_TIMER_HZ/2)) {
                buttonState[i].downTime = (BUTTON_TIMER_HZ/4);
                buttonState[i].repeat = 1;
            }

#ifdef LONG_TIMES
            if (buttonState[i].state && buttonState[i].upTime < 65535)
#else
            if (buttonState[i].state && buttonState[i].upTime < 255)
#endif
                buttonState[i].upTime++;
        }

#ifdef MATRIX_BUTTONS
    }
#endif
    return result;
}
#endif