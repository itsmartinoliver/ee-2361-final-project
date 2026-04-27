/*
 * File:   time.c
 * Author: bear
 *
 * Created on April 26, 2026, 4:13 PM
 */
//This library will have a variety of function which interact similar to standard timer apis on systems such as arduino or unix. This library will mainly use timer 1.

#include <stdint.h>
#include <xc.h>

uint16_t prev_count = 0;
float dt = 0.0f;


void Init_Timer1(uint8_t presealer) {
    T1CON = 0;
    TMR1 = 0;
    T1CONbits.TCKPS = 0b11;
    T1CONbits.TON = 1;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt() { //Timer1 Interrupt
    IFS0bits.T1IF = 0;
}


float Get_Elapsed_Time(void) {
    uint16_t current_count = TMR1;
    uint16_t ticks;

    // Handle 16-bit rollover
    if (current_count >= prev_count) {
        ticks = current_count - prev_count;
    } else {
        ticks = (0xFFFF - prev_count) + current_count + 1;
    }

    prev_count = current_count;

    // Convert ticks to seconds: dt = ticks * (Prescaler / FCY)
    // Example: FCY = 16MHz, Prescaler = 256
    return (float)ticks * (256.0f / 16000000.0f);
}
