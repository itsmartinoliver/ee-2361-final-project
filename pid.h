/*
 * File:   pid.h
 * Author: bear
 *
 * Created on April 26, 2026, 4:01 PM
 */

#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

// #include <stdint.h>

typedef struct {
    float Kp;         // proportional component
    float Ki;         // integral component
    float Kd;         // derivative component
    float setpoint;   // Current PID location
    float prev_error; // Previous error
    float integral;
    float out_max;
    float out_min;
} PID_Fixed;

PIDController pidInit(float p, float i, float d, float maxOutput);
int16_t PID_update(PID_Fixed *pid, int16_t measurement);

#ifdef __cplusplus
}
#endif

#endif /* PID_H */
