#ifndef BASIC_H
#define	BASIC_H

#include <htc.h>            /* HiTech General Includes */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdlib.h>         /* Simple functions */
#include <string.h>         /* String operations */

#define ifOfBool(var, toset) do { if (var) {toset = true;} else {toset = false;} } while (false)
#define boolOfBit(toset, var, bit) do { if (testbit(var, bit)) {toset = true;} else {toset = false;} } while (false)
#define testbit(var, bit)  ((var) & (1 << (bit)))
#define setbit(var, bit)   ((var) |= (1 << (bit)))
#define clearbit(var, bit) ((var) &= ~(1 << (bit)))

#define BYTEMASK(v,n)   (((char*)&v)[n])
#define WORDMASK(v,n)   (((short*)&v)[n])
#define DWORDMASK(v,n)  (((long*)&v)[n]) 

/******************************************************************************/
/* User Defined Functions                                                     */
/******************************************************************************/

void align(char* str, uint8_t finalLen, char fill, bool right);

/******************************************************************************/
/* User Defined Types                                                         */
/******************************************************************************/

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorRGB;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} ColorHSV;

typedef struct {
    uint16_t downTime;
    uint16_t upTime;
    uint8_t A       : 1;
    uint8_t B       : 1;
    uint8_t button  : 1;
} RotaryState;



#endif	/* BASIC_H */

