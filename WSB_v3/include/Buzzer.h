#ifndef _USER_BUZZER_H
#define _USER_BUZZER_H

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include <stdio.h>

// Sound creation functions
void playTone(float frequency); // Generate tone at certain frequency
void playTone2(float frequency); // Generate tone at certain frequency
float noteToFreq(char pitch, char accidental, int octave); // Convert any musical (in note musical notation) to the correct frequency
void playNote(char pitch, char accidental, int octave, float duration); // Play a note at the specified musical pitch for a specified duration in s

// Music playing functions 
void power_on_sound();
void power_off_sound();
void wifi_on_sound();
void wifi_off_sound();
void device_paired_sound();
void out_of_range_alarm();
void ten_step_alert();
// Power functions
double pow_custom(double base, int exponent);

#endif