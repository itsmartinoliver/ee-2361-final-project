/*
 * File:   newmainXC16.c
 * Author: bear
 *
 * Created on April 26, 2026, 4:01 PM
 */

// #include "xc.h"
#include "pid.h"

#define PID_INTERRUPT

typedef struct {
    float Kp;         // proportional component
    float Ki;         // integral component
    float Kd;         // derivative component
    float setpoint;   // Current PID location
    float prev_error; // Previous error
    float integral;
    float out_max;
    float out_min;
    int dt; // time between PID updates
} PIDController;

/* pidInit: this function will initialize the PIDController and start timer 1 to call an interrup updating the PID value PID to update with if PID_INTERRUPT is set
*/
// PIDController pidInit(float p, float i, float d, float maxOutput, float minInput, int dt, float *pidUpdater(PIDController,float), void * pidAction(float)) {
//     #ifdef PID_INTERRUPT
//
//     #endif /* ifdef MACRO */
//
//     return PIDController{
//         .Kp = p,
//         .Ki = i,
//         .Kd = d,
//         .setpoint = 0,
//         .maxOutput = maxOutput,
//         .minInput = minInput,
//     };
// }

float PID_Update(PIDController *pid, float measurement) {
    float error = pid->setpoint - measurement;

    // Proportional term
    float P = pid->Kp * error;

    // Integral term with basic anti-windup clamping
    pid->integral += error * pid->dt;
    float I = pid->Ki * pid->integral;

    // Derivative term (on error)
    float D = pid->Kd * ((error - pid->prev_error) / pid->dt);

    float output = P + I + D;

    // Clamp the output of the PID controller so Speed forward + speed PID is
    // under control
    if (output > pid->out_max)
        output = pid->out_max;
    else if (output < pid->out_max)
        output = pid->out_max;

    pid->prev_error = error;

    return output;
}
