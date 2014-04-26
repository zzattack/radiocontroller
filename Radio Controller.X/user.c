/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "globals.h"
#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void initPIC()
{
    /* Setup analog functionality and port direction */
            //76543210
    TRISA = 0b11111111;
    TRISB = 0b11111010;
    TRISC = 0b11111000;
    TRISD = 0b01111110;

    /* Initialize peripherals */

    PORTA = PORTB = PORTC = PORTD = PORTE = 0;

    /* Configure A/D */
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001011;
    ADCON2 = 0b00111110;

    /* Configure Timer 0 for timing @ 10000Hz for IR reception = 12MHz / 600 */
    T0CON = 0b10001000;
    TMR0IF = 0;
    TMR0IE = 1;

    /* Configure Timer 1 for timing @ 250Hz = 12MHz / 8 / 6000 */
    T1CON = 0b10110001;

    /* Configure EUSART */
    TXSTA = 0b00100110;
    RCSTA = 0b10000000;
    BAUDCON = 0b01000000;
    SPBRG = 155;    // 19200 baud rate

    /* Configure Timer 2 for PWM  @ 36.5kHz = 12MHz / 4 / 82 */
    PR2 = 82;
    CCPR1L = 82-22;     // 25% ON (inverted)
    CCP1CON = 0b000000000;
    TRISC = 0b11111000;
    T2CON = 0b00000101;
    CCP1CON = 0b00001100;

    ei();
}

void checkAnalog()
{

    switch(analogState) {
        case 0:
            GO_DONE = 1;
            break;
        case 1:
            analog1Value = ADRESH;
            ADCON0 = 0b00000101;
            break;
        case 2:
            GO_DONE = 1;
            break;
        case 3:
            analog2Value = ADRESH;
            ADCON0 = 0b00001001;
            break;
        case 4:
            GO_DONE = 1;
            break;
        case 5:
            analog3Value = ADRESH;
            ADCON0 = 0b00001101;
            break;
        case 6:
            GO_DONE = 1;
            break;
        case 7:
            analog4Value = ADRESH;
            ADCON0 = 0b00000001;
            break;
    }

    analogState++;
    if (analogState == 8)
        analogState = 0;
}

bool checkTimer1()
{
    if (TMR1IF) {
        TMR1IF = 0;
        TMR1H = (59535 >> 8) & 0xFF;
        TMR1L = 59535 & 0xFF;

        checkAnalog();

        predictiveStartUpdate();
        updatePowerRelais();
        
        time250Hz++;
        if (time250Hz == 20) {
            time250Hz = 0;
            checkStateMachine();
        }

        return true;
    }
    return false;
}

bool lastKnipperRecent() {
    return noKnippers < 15;
}

void checkStateMachine()
{
    switch (state) {
        case SystemOff:
            LED_RED = 1;
            LED_GREEN = 1;
            AccessoryRelais = 0;
            
            // when accessory is enabled, always go to boot state
            if (Accessory)
                ChangeState(SystemBoot);

            // when unlocking the doors, if accompanied by a knipper, also boot
            else if (lastKnipperRecent() && !Lock)
                ChangeState(SystemBoot);

            break;

        case SystemBoot:
            LED_RED = 1;
            LED_GREEN = 0;
            AccessoryRelais = 1;

            // @ time == 39: tx PWR_BUTTON_MSG
            // @ time in 240-239: tx VOL_UP_BUTTON_MSG
            // @ time >= 2160: goto SystemShutdown

            const int IDX_PWR_BUTTON_TX = 39;
            const int IDX_VOLUP_BUTTON_TX = 200;
            const int IDX_VOLUP_BUTTON_TX_END = 237;
            const int IDX_TIMEOUT = 2160;

            // if lock is activated (can only be measured when contact is off)
            // also check this was due to a knipper and not due to cars' internal timeout/relock
            if (Lock && lastKnipperRecent()) {

                // if we haven't been in this state for some time, we may not need
                // to close the screen or lower the volume
                
                if (stateTimer < IDX_PWR_BUTTON_TX)
                    // haven't powered up yet, just disconnect relais
                    ChangeState(SystemOff);

                else if (stateTimer < IDX_VOLUP_BUTTON_TX) {
                    // no volume up yet, switch to SystemShutdown but skip
                    // the part where volume is turned down
                    ChangeState(SystemShutdown);
                    stateTimer = 51; // shutdown ir is sent @ 52 (see IDX_PWR_BUTTON_TX in SystemShutdown)
                }
                else // perform full shutdown including vol down
                    ChangeState(SystemShutdown);

            } else if (stateTimer == IDX_PWR_BUTTON_TX) {
                irSendMessageIndex = PWR_BUTTON_MSG;

            } else if (stateTimer > IDX_VOLUP_BUTTON_TX) {
                if (stateTimer <= IDX_VOLUP_BUTTON_TX_END) {
                    // only every 4th tick
                    if (testbit(stateTimer, 0)) {
                        irSendMessageIndex = VOL_UP_BUTTON_MSG;
                    }
                }

                // at this point we have fully started, opened screen and raised volume

                else if (Knipper) {
                    stateTimer = IDX_VOLUP_BUTTON_TX_END + 1;
                    // this basically postpones the timeout event
                }
                else if (Accessory)
                    ChangeState(SystemOn);
                else if (stateTimer > IDX_TIMEOUT)
                    ChangeState(SystemShutdown);
            }
            break;
            
        case SystemOn:
            LED_RED = 0;
            LED_GREEN = 0;
            AccessoryRelais = 1;

            if (!Accessory) {
                shutdownPhase = contactOff;
                ChangeState(SystemShutdownWait);
            }
            break;

        case SystemShutdownWait:
            LED_RED = 0;
            LED_GREEN = 1;
            AccessoryRelais = 1;

            switch (shutdownPhase) {
                // shutdownPhase is initially always accessoryOff
                case contactOff:
                    // Lock signal was active while contact was on, but
                    // quickly deactivates after contactOff
                    if (!Lock)
                        shutdownPhase = lockInactive;
                    break;

                case lockInactive:
                    // after manually activating the lock the car is actually locked
                    if (Lock && lastKnipperRecent())
                        shutdownPhase = lockActive;
                    break;
            }

            if (Accessory)
                ChangeState(SystemOn);
            else if (stateTimer > 1440 || shutdownPhase == lockActive)
                ChangeState(SystemShutdown);

            break;

        case SystemShutdown:
            LED_RED = 0;
            LED_GREEN = 1;
            AccessoryRelais = 1;

            const int IDX_VOL_DOWN_END = 80;
            const int IDX_PWR_BUTTON_TX = IDX_VOL_DOWN_END + 2;
            const int IDX_PWR_BUTTON_TX_END = IDX_PWR_BUTTON_TX + 28;
            const int IDX_POWEROFF_RELAIS = IDX_PWR_BUTTON_TX_END + 20;

            // @ t in 0-50: tx VOL_DOWN_BUTTON_MSG
            //     accessoryOn/Lock/knipper -> ChangeState(SystemOn)
            // @ t = 55-56, tx PWR/REP button for full-shutdown of radio
            // @ t = 110, goto SystemOff

            if (Accessory || (!Lock && lastKnipperRecent())) {
                // unlock when we've so far only turned volume down:
                if (stateTimer <= IDX_VOL_DOWN_END) {
                    ChangeState(SystemOn); // user will have to manually up the volume himself
                } else if (stateTimer <= IDX_PWR_BUTTON_TX_END) {
                    // if we're currently transmitting the poweroff command, let this finish,
                    // as we cannot determine precisely when this command is "caught"
                } else if (stateTimer <= IDX_POWEROFF_RELAIS) {
                    // This covers the case where screen is currently closing, but the relais isn't off yet.
                    // We continue at SystemBoot, just before transmitting the power on IR code
                    ChangeState(SystemBoot);
                }
            }
            else if (stateTimer > IDX_POWEROFF_RELAIS) {
                ChangeState(SystemOff);
            }
            else if (stateTimer >= IDX_PWR_BUTTON_TX_END) {

            }
            else if (stateTimer >= IDX_PWR_BUTTON_TX + 1) {
                irSendMessageIndex = REP_BUTTON_MSG;
            }
            else if (stateTimer == IDX_PWR_BUTTON_TX) {
                irSendMessageIndex = PWR_BUTTON_MSG;
            }
            else if (stateTimer < IDX_VOL_DOWN_END && testbit(stateTimer, 0)) {
                irSendMessageIndex = VOL_DOWN_BUTTON_MSG;
            }
            break;
    }

    if (Knipper)
        noKnippers = 0;
    else if (noKnippers < 65535)
        noKnippers++;
    
    if (stateTimer < 65535)
        stateTimer++;

}

void predictiveStartUpdate() {
    // in this function the readings of contactOff are delayed
    // because the contact reading is inaccurate during starting

    const uint16_t CONTACTOFF_DELAY = 125;
    const uint16_t RECENTNESS_DELAY = 30*250;

    switch (predictState) {
        case Idle:
            if (Contact)
                ChangePredictState(ContactRecent);
            break;

        case ContactRecent:
        case ContactNotRecent: // hack but it's fine
            if (Start)
                ChangePredictState(Starting);
            else if (contactOffCounter > CONTACTOFF_DELAY)
                // contact now definitely off
                ChangePredictState(Idle);
            else if (predictStateTimer > RECENTNESS_DELAY)
                ChangePredictState(ContactNotRecent);
            break;

        case Starting:
            if (Contact && !Start) // finished starting
                ChangePredictState(ContactNotRecent);
            break;
    }

    if (predictStateTimer < 65535)
        predictStateTimer++;

    // delay contact counter
    if (Contact) contactOffCounter = 0;
    else if (contactOffCounter < 255) contactOffCounter++;

    if (Start) startOffCounter = 0;
    else if (startOffCounter < 255) startOffCounter++;

    if (!Pump) pumpOnCounter = 0;
    else if (pumpOnCounter < 65535) pumpOnCounter++;
}

void updatePowerRelais() {    
    // RELAIS3 ==> supply to the radio
    // RELAIS4 ==> supply to the battery
    if (startOffCounter < 125) {
        // starting
        SwitchRelais = SWITCHBAT;
        ChargeRelais = CHARGEOFF;
    }
    else if (pumpOnCounter > 1250 && Contact) {
        // running, for at least 10 seconds
        SwitchRelais = SWITCHACCU; // doesn't matter, relais off preserves power
        ChargeRelais = CHARGEACCU;
    }
    else if (predictState == ContactRecent) {
        // starting
        SwitchRelais = SWITCHBAT;
        ChargeRelais = CHARGEOFF;
    }
     
    else {
        // not starting, not running, possibly radio even turned off
        SwitchRelais = SWITCHACCU;
        ChargeRelais = CHARGEOFF;
    }

}