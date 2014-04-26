/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "globals.h"

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

void initPIC();             /* I/O and Peripheral Initialization */

void checkAnalog();
bool checkTimer1();

void checkStateMachine();
void predictiveStartUpdate();
void updatePowerRelais();