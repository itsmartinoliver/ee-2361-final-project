//The boring solution
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
#define MOTOR_SPEED 90
#define DISTANCE_THRESHOLD 200

// ---------------- TUNABLE CONSTANTS ---------------- //
// Offset vector
const signed int p[3] = {2000, 3300, 2000};

const unsigned int SENSOR_PULSE_DELAY = 20000; // Delay between sonic pulses from distance sensors in microseconds
// --------------------------------------------------- //

void setup(void);

volatile signed int motorVector[2] = {0, 0}; // Index 0 left motor
volatile signed int distanceVector[3] = {0, 0, 0}; // Values go from ~2500 (very close) to ~4500 (somewhat far)
volatile unsigned long int T3Cycles = 0;
volatile int count = 0;

void setMotorSpeed(int speed, int motorSelect) {
    //speed should be only between 0 and 99
    //motorSelect should be 0 for left motor, 1 for right motor
    //more specifically if the motor is attached to OC1R, motorSelect should be 0.
    if(motorSelect == 0) {
        OC1RS = speed;
    }
    else {
        OC2RS = speed;
    }
}

//BASIC MOVES (Show me ya moves!)
void rotateCCW90(void) {
    setMotorSpeed(0,0); //set left motor to no speed
    setMotorSpeed(MOTOR_SPEED,1); //set right motor to high speed
    T1CONbits.TON = 1; //turn on the timer
    while(count != 2);
        //wait for some time to turn equal to a certain number of TMR1 interrupts
        //count being 2 is arbitrary at the moment, we can test it
        
    T1CONbits.TON = 0; //turn off the timer
    TMR1 = 0; //reset value of TMR1
    count = 0; //reset count for future movements
    //turn motors to low and return
    setMotorSpeed(0,0); //set left motor to no speed
    setMotorSpeed(0,1); //set right motor to no speed
    
}


void rotateCW90(void) {
    setMotorSpeed(MOTOR_SPEED,1); //set left motor to high
    setMotorSpeed(0,1); //set right motor to no speed
    T1CONbits.TON = 1; //turn on the timer
    while(count != 2);
        //wait for some time to turn equal to a certain number of TMR1 interrupts
        //count being 2 is arbitrary at the moment, we can test it
        
    T1CONbits.TON = 0; //turn off the timer
    TMR1 = 0; //reset value of TMR1
    count = 0; //reset count for future movements
    //turn motors to low and return
    setMotorSpeed(0,0); //set left motor to no speed
    setMotorSpeed(0,1); //set right motor to no speed
    
}

void nudgeForward(void) {
    //This function is not for continuous driving but is to ensure that we get out of a junction after rotating
    //set both motor speeds high
    setMotorSpeed(MOTOR_SPEED,0);
    setMotorSpeed(MOTOR_SPEED,1);
    
     T1CONbits.TON = 1; //turn on the timer
    while(count != 2);
        //wait for some time to turn equal to a certain number of TMR1 interrupts
        //count being 2 is arbitrary at the moment, we can test it
        
    T1CONbits.TON = 0; //turn off the timer
    TMR1 = 0; //reset value of TMR1
    count = 0; //reset count for future movements
    //turn motors to low and return
    setMotorSpeed(0,0); //set left motor to no speed
    setMotorSpeed(0,1); //set right motor to no speed
    
}

void stopMoving(void) {
    //This function both stops the motor and ensures we take no more input from PID
    setMotorSpeed(0,0); //set left motor to no speed
    setMotorSpeed(0,1); //set right motor to no speed
    //FIXME: ADD STOP PID
}

void turnaround(void) {
    rotateCCW90();
    rotateCCW90();
}





void sensorResponse(void) {
    //This function will decide what to do based on its current input from its sensors
    if(distanceVector[0]-p[0] > DISTANCE_THRESHOLD && distanceVector[1]-p[1] < DISTANCE_THRESHOLD && distanceVector[2]-p[2] < DISTANCE_THRESHOLD) { 
        //left corner case
        //only option is to go left
        rotateCCW90();
        nudgeForward();
    }
    else if(distanceVector[0]-p[0] > DISTANCE_THRESHOLD && distanceVector[1]-p[1] < DISTANCE_THRESHOLD && distanceVector[2]-p[2] < DISTANCE_THRESHOLD) {
        //right corner case
        //only option is to go right
        rotateCW90();
        nudgeForward();
    }
    else if(distanceVector[0]-p[0] > DISTANCE_THRESHOLD && distanceVector[1]-p[1] > DISTANCE_THRESHOLD && distanceVector[2]-p[2] < DISTANCE_THRESHOLD) {
        //left option case
        //take the left option
        rotateCCW90();
    }
    else if(distanceVector[0]-p[0] < DISTANCE_THRESHOLD && distanceVector[1]-p[1] > DISTANCE_THRESHOLD && distanceVector[2]-p[2] > DISTANCE_THRESHOLD) {
        //right option case
        //keep going forward
        nudgeForward();
    }
    else if(distanceVector[0]-p[0] > DISTANCE_THRESHOLD && distanceVector[1]-p[1] > DISTANCE_THRESHOLD && distanceVector[2]-p[2] > DISTANCE_THRESHOLD) {
        //+ junction case
        //pick left path
        rotateCCW90();
        nudgeForward();
    }    
    else if(distanceVector[0]-p[0] > DISTANCE_THRESHOLD && distanceVector[1]-p[1] < DISTANCE_THRESHOLD && distanceVector[2]-p[2] > DISTANCE_THRESHOLD) {
        //T junction case
        //pick left path
        rotateCCW90();
        nudgeForward();
    }
    else if(distanceVector[0]-p[0] < DISTANCE_THRESHOLD && distanceVector[1]-p[1] < DISTANCE_THRESHOLD && distanceVector[2]-p[2] < DISTANCE_THRESHOLD) {
        //Dead End Case
        turnaround();
    }
    else {
        //hall case
        //keep  moving forward
        nudgeForward();
    }
      
}

void updateSensor(int time, int relativeTime) {
    //updates the correct sensor based on the time that it was called
    
    // Find index of distance sensor from which the signal was received
    int i = (relativeTime > SENSOR_PULSE_DELAY && relativeTime <= 2*SENSOR_PULSE_DELAY) +
            (2*(relativeTime > 2*SENSOR_PULSE_DELAY && relativeTime <= 3*SENSOR_PULSE_DELAY));
    
    // Set the appropriate distanceVector index based on when the ECHO was received
    distanceVector[i] = relativeTime - (SENSOR_PULSE_DELAY*i);
}


int main(void) {
    setup();
    
    //loop
    while (1) {
        //check sensors, respond accordingly
        
        
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
    
    //Timer 1
        //used for keeping track of how long we've been driving/turning
    T1CON = 0;
    TMR1 = 0;
    T1CONbits.TCKPS = 0b11; //PRE 1:256
    _T1IF = 0; //ensure interrupt flag is off
    PR1 = 65535; //set timer 1 to its max length
    IEC0bits.T1IE = 1; //Enable timer 1 interrupt
    T1CONbits.TON = 0; //we will start timer 1 whenever we need to
    
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

void __attribute__((interrupt, auto_psv)) _T1Interrupt() {
    _T1IF = 0; //Always reset the flag
    count++; //increment counter so we know how long its been
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
    
    
    return;
}

void __attribute__((__interrupt__, __auto_psv__)) _IC1Interrupt(void)
{
    _IC1IF = 0;
    
    signed long int t = IC1BUF + (100*T3Cycles); // Capture time t
    signed long int rt = t % (3*SENSOR_PULSE_DELAY); // Find relative time rt
    updateSensor(t, rt);
    
    PORTBbits.RB9 = 0; // Turn off status LED to indicate first distance measurement accomplished
    return;
}

