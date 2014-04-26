/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "basic.h"              /* For the basic definitions */

/******************************************************************************/
/* User Defined Functions                                                     */
/******************************************************************************/

void align(char* str, uint8_t finalLen, char fill, bool right)
{
    uint8_t len = strlen(str);
    if (len < finalLen) {
        str[finalLen] = 0;
        if (right) {
            for (uint8_t i = 0; i < len; i++) {
                str[finalLen-i-1] = str[len-i-1];
            }
            for (uint8_t i = 0; i < finalLen-len; i++) {
                str[i] = fill;
            }
        } else {
            for (uint8_t i = len; i < finalLen; i++) {
                str[i] = fill;
            }
        }
    }
}
