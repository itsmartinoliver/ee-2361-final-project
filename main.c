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

// ---------------- TUNABLE CONSTANTS ---------------- //
// Transformation matrix
const float x[6] = { 0.05,  0.05,  0.00,
                     0.00,  0.05,  0.05};

// Offset vector
const signed int p[3] = {2000, 3300, 2000};

const unsigned int SENSOR_PULSE_DELAY = 20000; // Delay between sonic pulses from distance sensors in microseconds
// --------------------------------------------------- //

void setup(void);

volatile signed int motorVector[2] = {0, 0}; // Index 0 left motor
volatile signed int distanceVector[3] = {0, 0, 0}; // Values go from ~2500 (very close) to ~4500 (somewhat far)
volatile unsigned long int T3Cycles = 0;

int main(void) {
    setup();
    
    while (1) {
        motorVector[0] = (x[0]*(distanceVector[0]-p[0])) +
                         (x[1]*(distanceVector[1]-p[1])) +
                         (x[2]*(distanceVector[2]-p[2]));
        
        motorVector[1] = (x[3]*(distanceVector[0]-p[0])) +
                         (x[4]*(distanceVector[1]-p[1])) +
                         (x[5]*(distanceVector[2]-p[2]));
    }
    return;
}

void setup(void) {
    AD1PCFG = 0xFFFF; // Sets all pins to digital I/O
    TRISB = 0x0100; // RB8 is the only input (combined distance sensor echo pins)
    
    // Code modified from PPS Quick Lesson manual
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    RPINR7bits.IC1R = 8;  // Use Pin RP8 = "8", for Input Capture 1 (Table 10-2)
    RPOR3bits.RP6R = 19;  // Use Pin RP6 for Output Compare 2 = "19" (Table 10-3)
    RPOR3bits.RP7R = 18;  // Use Pin RP7 for Output Compare 1 = "18" (Table 10-3)
    RPOR5bits.RP10R = 20;  // Use RPOR5 for RP10 (Output Compare 3)
    RPOR5bits.RP11R = 21;  // Use RPOR5 for RP11 (Output Compare 4)
    RPOR6bits.RP12R = 22;  // Use RPOR6 for RP12 (Output Compare 5)
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    
    PORTBbits.RB9 = 1; // Turn on status LED to indicate program start
    
    // Timer 3
        T3CONbits.TCKPS = 0b01; // PRE 1:8
        TMR3 = 0;
        _T3IF = 0;
        PR3 = 99; // Timer resets at 99 because OCxRS = 100 should correspond to 100%
        IEC0bits.T3IE = 1; // Enable Timer 3 interrupt
        T3CONbits.TON = 1; // Restart Timer 3
    
    // OC1 & OC2 for motor PWM
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
    
    // OC3, OC4, OC5 for distance sensors
        OC3CON = 0; // Disable OC3 for now
        OC3R = 89; // Trigger at the end of the clock period so that T3Cycles has time to update
        OC3RS = 99; // 10 us pulse (TRIG)
        OC3CONbits.OCTSEL = 1; // Use Timer 3 for compare source
        OC3CONbits.OCM = 0b101; // Continuous Pulse Mode

        OC4CON = 0; // Disable OC4 for now
        OC4R = 89; // Trigger at the end of the clock period so that T3Cycles has time to update
        OC4RS = 99; // 10 us pulse (TRIG)
        OC4CONbits.OCTSEL = 1; // Use Timer 3 for compare source
        OC4CONbits.OCM = 0b101; // Continuous Pulse Mode

        OC5CON = 0; // Disable OC5 for now
        OC5R = 89; // Trigger at the end of the clock period so that T3Cycles has time to update
        OC5RS = 99; // 10 us pulse (TRIG)
        OC5CONbits.OCTSEL = 1; // Use Timer 3 for compare source
        OC5CONbits.OCM = 0b101; // Continuous Pulse Mode
        
    // Input capture for distance sensor ECHO
        IEC0bits.IC1IE = 1; // Enable IC1 interrupt
        IC1CON = 0; // Turn off and reset internal state of IC1
        IC1CONbits.ICTMR = 0; // Use Timer 3 for capture source
        IC1CONbits.ICM = 0b010; // Turn on and capture every FALLING edge
    
    return;
}

void __attribute__((interrupt, auto_psv)) _T3Interrupt() {
    _T3IF = 0;
    T3Cycles++;
    
    // First: Set output compare enable bits for TRIG
    unsigned long int norm = (100*T3Cycles) % (3*SENSOR_PULSE_DELAY); // This is the approximate current time being compared with the total delay period
    OC3CONbits.OCM = 0b101 * (norm == 0);
    OC4CONbits.OCM = 0b101 * (norm == SENSOR_PULSE_DELAY);
    OC5CONbits.OCM = 0b101 * (norm == 2*SENSOR_PULSE_DELAY);
    
    // Update output compare values and xPHASE pins
    unsigned char sL = (motorVector[0] >> 15) & 0b1; // Use only sign
    unsigned char sR = (motorVector[1] >> 15) & 0b1; // Use only sign
    OC1RS = ((!sL) * motorVector[0]) + (sL * (~motorVector[0] + 1)); // Use only magnitude
    OC2RS = ((!sR) * motorVector[1]) + (sR * (~motorVector[1] + 1)); // Use only magnitude
    PORTBbits.RB13 = sL; // APHASE
    PORTBbits.RB14 = sR; // BPHASE
    
    return;
}

void __attribute__((__interrupt__, __auto_psv__)) _IC1Interrupt(void)
{
    _IC1IF = 0;
    
    signed long int t = IC1BUF + (100*T3Cycles); // Capture time t
    signed long int rt = t % (3*SENSOR_PULSE_DELAY); // Find relative time rt
    
    // Find index of distance sensor from which the signal was received
    int i = (rt > SENSOR_PULSE_DELAY && rt <= 2*SENSOR_PULSE_DELAY) +
            (2*(rt > 2*SENSOR_PULSE_DELAY && rt <= 3*SENSOR_PULSE_DELAY));
    
    // Set the appropriate distanceVector index based on when the ECHO was received
    distanceVector[i] = rt - (SENSOR_PULSE_DELAY*i);
    
    PORTBbits.RB9 = 0; // Turn off status LED to indicate first distance measurement accomplished
    return;
}
