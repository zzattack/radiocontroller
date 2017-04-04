/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"
#include "easytimer.h"
#include "buttonchecker.h"
#include "uarts.h"
#include <stdlib.h>


void initPIC();             /* I/O and Peripheral Initialization */
void checkAnalog();
void checkStateMachine();
void predictiveStartUpdate();
void lockFix();
void updatePowerRelais();

typedef enum { SystemOff, SystemBoot, SystemOn, SystemShutdownWait, SystemShutdown } SystemState;
typedef enum { contactOff, lockInactive, lockActive } ShutdownPhase ;
typedef enum { Idle, ContactRecent, ContactNotRecent, Starting } PredictState;

const char* SystemStateNames[] = { "SystemOff", "SystemBoot", "SystemOn", "SystemShutdownWait", "SystemShutdown" };
const char* ShutdownPhaseNames[] = { "ContactOff", "LockInactive", "LockActive" };
const char* PredictStateNames[] = { "Idle", "ContactRecent", "ContactNotRecent", "Starting" };

SystemState sysState = SystemOff;
PredictState predictState = Idle;
ShutdownPhase shutdownPhase = contactOff;

void ChangeSysState(SystemState state, const char* reason);
void ChangePredictState(PredictState state, const char* reason);
void ChasngeShutdownPhase(ShutdownPhase phase, const char* reason);

/* A/D settings */
uint8_t analogState = 0;
uint8_t analog1Value = 0;
uint8_t analog2Value = 0;
uint8_t analog3Value = 0;
uint8_t analog4Value = 0;

/* State Machines */
uint16_t stateTimer = 0;
uint16_t predictTimer = 0;
uint16_t noBlink = 65535;
bool blinkWhileWait = 0;
uint8_t contactOffCounter = 255;
uint8_t startOffCounter = 255;
uint16_t pumpOnCounter = 0;
uint16_t timeSinceUnlock = 65535;
uint16_t timeUnlockFakeRemaining = 65535;
bool Lock = 0;
bool LockRealPrev = 0;


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main() {
    /* Initialize I/O and Peripherals for application */
    initPIC();
    U1Init(115200, false, true);
    bcInit();
    
    RCONbits.IPEN = 1; // priority levels
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    dbgs("Radiocontroller initialized\n");
    dbgs("Last reset cause: \n");
    if (!RCONbits.RI) {
        dbgs("Reset instruction\n");
        RCONbits.RI = 1;
    }
    if (!RCONbits.TO) {
        dbgs("watchdog\n");
    }
    if (!RCONbits.PD) {
        dbgs("SLEEP instruction\n");
    }
    if (!RCONbits.POR) {
        dbgs("Power on reset\n");
        RCONbits.POR = 1;
    }
    if (RCONbits.BOR) {
        dbgs("Brown-out reset\n");
        RCONbits.BOR = 1;
    }
    
    LED_GREEN = 1;
    LED_RED = 1;
    while(1) {
        ClrWdt();
        
        if (EasyTimerTick) {
            EasyTimerTick--;
            tick1000HzInternal();
        }
        predictiveStartUpdate();
        updatePowerRelais();
        lockFix();
        checkStateMachine();        
    }
}


void initPIC() {
    /* Setup analog functionality and port direction */
            //76543210
    TRISA = 0b11111111;
    TRISB = 0b11111010;
    TRISC = 0b11111000;
    TRISD = 0b01111010;

    PORTA = PORTB = PORTC = PORTD = PORTE = 0;

    /* Configure A/D */
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001011;
    ADCON2 = 0b00111110;
    
    // Prescaler 1:4; Postscaler 1:12; TMR2 Preload = 250; Actual Interrupt Time : 1.000 ms
    // timer2, all 1/10/100/1000 Hz timer functions derive from this
    PR2		 = 250;
    T2CON	 = 0b01100101;
    TMR2IE   = 1;
    TMR2IP   = 1;
}

void ChangeSysState(SystemState s, const char* reason) {
    dbgs("[SYS] state change from ");
    dbgs(SystemStateNames[sysState]);
    sysState = s; 
    dbgs(" to ");
    dbgs(SystemStateNames[(uint8_t)sysState]);
    if (reason != NULL) {
        dbgs(" because ");
        dbgs(reason);
    }
    dbgs("\n");
    stateTimer = 0;
}
void ChangePredictState(PredictState s, const char* reason) {
    dbgs("[PRE] state change from ");
    dbgs(PredictStateNames[(uint8_t)predictState]);
    predictState = s; 
    dbgs(" to ");
    dbgs(PredictStateNames[(uint8_t)predictState]);
    if (reason != NULL) {
        dbgs(" because ");
        dbgs(reason);
    }
    dbgs("\n");
    predictTimer = 0;
}

void checkAnalog() {
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

bool lastBlinkRecent() {
    return noBlink < 50;
}

void checkStateMachine() {
    switch (sysState) {
        case SystemOff:
            LED_RED = 1;
            LED_GREEN = 1;
            AccessoryRelais = 0;
            Brake = 0;
            
            // when accessory is enabled, always go to boot state
            if (Accessory)
                ChangeSysState(SystemBoot, "accessory on");
            
            // when unlocking the doors, if accompanied by a blink, also boot
            else if (lastBlinkRecent() && !Lock)
                ChangeSysState(SystemBoot, "blink + unlock");
            
            break;

        case SystemBoot:
            LED_RED = 1;
            LED_GREEN = 0;
            AccessoryRelais = 1;

            const int IDX_TIMEOUT = 18000; // 3mins
            Brake = stateTimer > 200 ? 1 : 0; // bogus brake signal, just usually GND for pioneer

            if (Accessory) // full on
                ChangeSysState(SystemOn, "accessory on");
            else if (Lock && lastBlinkRecent()) // lock while booting, just shut off
                ChangeSysState(SystemShutdown, "lock while booting");            
            else if (Blink) // blink while booting, postpone timeout
                stateTimer = 0;
            else if (stateTimer > IDX_TIMEOUT) // timeout, shut off
                ChangeSysState(SystemShutdown, "no accessory before 3min timeout");
            break;
            
        case SystemOn:
            LED_RED = 0;
            LED_GREEN = 0;
            AccessoryRelais = 1;
            Brake = 1;

            if (!Accessory) {
                blinkWhileWait = false;
                shutdownPhase = contactOff;
                ChangeSysState(SystemShutdownWait, "accessory off");
            }
            break;

        case SystemShutdownWait:
            // this is where we wait for car to be locked before going to full shutdown
            LED_RED = 0;
            LED_GREEN = 1;
            AccessoryRelais = 1;
            Brake = 1;

            // we either wait for lock to become inactive, then active + blink, or timeout
            blinkWhileWait |= Blink;
            switch (shutdownPhase) {
                // shutdownPhase is initially always contactOff
                case contactOff:
                    // Lock signal was active while contact was on, but
                    // quickly deactivates after contactOff
                    if (!Lock) shutdownPhase = lockInactive;
                    break;
                case lockInactive:
                    if (Lock && blinkWhileWait) shutdownPhase = lockActive;
                    break;
            }
            if (Accessory)
                ChangeSysState(SystemOn, "accessory on during shutdown");
            // after locking the car or 3 minute timeout
            else if (stateTimer > 18000)
                ChangeSysState(SystemShutdown, "3 min timeout waiting for lock");
            else if (shutdownPhase == lockActive)
                ChangeSysState(SystemShutdown, "lock made active");
            break;

        case SystemShutdown:
            // this is where we'll lower volume before shutting down completely
            LED_RED = 0;
            LED_GREEN = 1;
            AccessoryRelais = 1;
            Brake = 1;
            if (stateTimer >= 10)
                ChangeSysState(SystemOff, "timer expired (always)");
            break;
    }

}

void predictiveStartUpdate() {
    // This is called at 250Hz.
    // In this function the readings of contactOff are delayed
    // because the contact reading is inaccurate during starting
    const uint16_t CONTACTOFF_DELAY = 50; // hold for half second
    const uint16_t RECENTNESS_DELAY = 3000; // if predicting a start, but doesn't happen within 30 seconds, revert

    switch (predictState) {
        case Idle:
            if (Contact) {
                ChangePredictState(ContactRecent, "contact on");
                contactOffCounter = 0;
            }
            break;

        case ContactRecent:
        case ContactNotRecent: // hack but it's fine
            if (Start)
                ChangePredictState(Starting, "start on");
            else if (!Contact && contactOffCounter > CONTACTOFF_DELAY)
                // contact now definitely off
                ChangePredictState(Idle, "contactOffCounter > CONTACTOFF_DELAY");
            else if (predictTimer > RECENTNESS_DELAY && predictState == ContactRecent)
                ChangePredictState(ContactNotRecent, "predictTimer > RECENTNESS_DELAY");
            break;

        case Starting:
            if (Contact && !Start) // finished starting
                ChangePredictState(ContactNotRecent, "start finished");
            break;
    }
}

void updatePowerRelais() {    
    // RELAIS3 ==> supply to the radio
    // RELAIS4 ==> supply to the battery
    if (startOffCounter < 100) {
        // starting
        SwitchRelais = SWITCHBAT;
        ChargeRelais = CHARGEOFF;
    }
    else if (pumpOnCounter > 1000 && Contact) {
        // car is running for at least 10s, so we can charge battery
        SwitchRelais = SWITCHACCU; // doesn't matter, relais off saves some power
        ChargeRelais = CHARGEACCU; 
    }
    else if (predictState == ContactRecent) {
        // likely to start soon, switch over to battery as precaution
        SwitchRelais = SWITCHBAT;
        ChargeRelais = CHARGEOFF;
    }
    else {
        // not starting, not running, possibly radio even turned off
        SwitchRelais = SWITCHACCU;
        ChargeRelais = CHARGEOFF;
    }
}


void lockFix() {
    // sometimes right after unlocking, the car re-locks itself,
    // causing the startup sequence to never happen.
    // when the instant re-lock is detected, we force the lock
    // to be ignored for 120 seconds

    if (!LockReal && LockRealPrev) { // just unlocked
        timeSinceUnlock = 0;
        Lock = false;
    }
    else if (LockReal) {
        if (!LockRealPrev) { // just locked
            // if re-locked within a 2s of unlocking, "ignore" for 120s
            if (timeSinceUnlock < 200)
                timeUnlockFakeRemaining = 12000; // 2 min
            else
                timeUnlockFakeRemaining = 0;
        }
        if (timeUnlockFakeRemaining > 0) {
            if (Blink && timeUnlockFakeRemaining < 1420) {
                // knipper during lock fake means either 1) the doors are unlocked
                // again, after which !LockReal is in sync again (picked up above)
                // or 2) the doors are now actually locked (picked up here) so
                // we should stop faking the unlock
                timeUnlockFakeRemaining = 0;
                Lock = true;
            }
            else {
                timeUnlockFakeRemaining--;
                Lock = false;
            }
        }
        else
            Lock = true;
    }
    LockRealPrev = LockReal;
    if (timeSinceUnlock < 65535)
        timeSinceUnlock++;
}


void interrupt high_priority isr() {
    uartsIsr();
    if (TMR2IF) {
        TMR2IF = 0;
        EasyTimerTick++;
    }
}
void interrupt low_priority isr_low() {
    dbgs("low priority interrupt, should never fire!\n");
}

extern void tickTimer1000Hz() {
    if (bcCheck()) {
        if (bcTick(0)) {
            dbgs("[BTN] "); dbgs("accessory: "); dbgs(bcPressed(0) ? "on" : "off"); dbgs("\n");
        }
        if (bcTick(1)) {
            dbgs("[BTN] "); dbgs("contact: "); dbgs(bcPressed(1) ? "on" : "off"); dbgs("\n");
        }
        if (bcTick(2)) {
            dbgs("[BTN] "); dbgs("start: "); dbgs(bcPressed(2) ? "on" : "off"); dbgs("\n");
        }
        if (bcTick(3)) {
            dbgs("[BTN] "); dbgs("pump: "); dbgs(bcPressed(3) ? "on" : "off"); dbgs("\n");
        }
        if (bcTick(4)) {
            dbgs("[BTN] "); dbgs("lockreal: "); dbgs(bcPressed(4) ? "on" : "off"); dbgs("\n");
        }
        if (bcTick(5)) {
            dbgs("[BTN] "); dbgs("blink: "); dbgs(bcPressed(5) ? "on" : "off"); dbgs("\n");
        }        

        if (bcPressed(5)) noBlink = 0;        
    }
}
extern void tickTimer100Hz() {
    if (stateTimer < 65535) stateTimer++;
    if (predictTimer < 65535) predictTimer++;
    if (noBlink < 65535) noBlink++;   
    
    // delay contact counter
    if (Contact) contactOffCounter = 0;
    else if (contactOffCounter < 255) contactOffCounter++;

    if (Start) startOffCounter = 0;
    else if (startOffCounter < 255) startOffCounter++;

    if (!Pump) pumpOnCounter = 0;
    else if (pumpOnCounter < 65535) pumpOnCounter++;
    
}
extern void tickTimer10Hz() { 
    checkAnalog();
}
extern void tickTimer1Hz() {
    
}