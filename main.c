// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)

// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

#include "xc.h"

void setup(void);

volatile signed char wheelVelocityLeft = 10;
volatile signed char wheelVelocityRight = 10;

int main(void) {
    setup();
    
    while (1) {
        // Do nothing
    }
    return;
}

void setup(void) {
    AD1PCFG = 0xFFFF; // Sets all pins to digital I/O
    TRISB = 0x0100; // RB8 is the only input (combined distance sensor echo pins)
    
    // Code taken from PPS Quick Lesson manual
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 8;  // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    RPOR3bits.RP6R = 19;  // Use Pin RP6 for Output Compare 2 = "19" (Table 10-3)
    RPOR3bits.RP7R = 18;  // Use Pin RP7 for Output Compare 1 = "18" (Table 10-3)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    // AENBL & BENBL PWM Timer 3
    T3CONbits.TCKPS = 0b01; // PRE 1:8
    TMR3 = 0;
    _T3IF = 0;
    PR3 = 99; // Timer resets at 99 because OCxRS = 100 should correspond to 100%
    IEC0bits.T3IE = 1; // Enable Timer 3 interrupt
    T3CONbits.TON = 1; // Restart Timer 3
    
    OC1CON = 0; // Disable OC1 for now
    OC1R = 0;
    OC1RS = 0;
    OC1CONbits.OCTSEL = 1; // Use Timer 3 for compare source
    OC1CONbits.OCM = 0b110; // Output compare PWM w/o faults
    
    OC2CON = 0; // Disable OC2 for now
    OC2R = 0;
    OC2RS = 0;
    OC2CONbits.OCTSEL = 1; // Use Timer 3 for compare source
    OC2CONbits.OCM = 0b110; // Output compare PWM w/o faults
    
    return;
}

void __attribute__((interrupt, auto_psv)) _T3Interrupt() { // <- TODO: Handle negaitve values of wheelVelocity
    _T3IF = 0;
    OC1RS = wheelVelocityLeft;
    OC2RS = wheelVelocityRight;
    //PORTBbits.RB14 = wheelVelocityLeft & 0x80; // Only want sign bit (not yet working)
    //PORTBbits.RB13 = wheelVelocityRight & 0x80; // Only want sign bit (not yet working)
    return;
}
