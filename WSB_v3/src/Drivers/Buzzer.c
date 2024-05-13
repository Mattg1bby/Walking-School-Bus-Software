#include "Buzzer.h"

void playTone(float frequency) {
  // Input frequency - play tone at that frequency for 0.05s
  // Lowest Frequency = 20Hz --> period = 0.05s
  // Varying frequency varies period, but all the tones need to be the same length
  // Therefore, for loop used to loop tone such that all notes are 0.05s long

  float period = 1 / frequency;
  int duration = (period / 2) * 1000000;
  int repeats = 0.05 / period; 

  for (int i = 0; i <= repeats; i++) {
		// Create Square wave with 50% DR.
    
		// GPIO High
        GPIO_OUTPUT_SET(12, 1);
		os_delay_us(duration);
		// GPIO LOW
        GPIO_OUTPUT_SET(12, 0);
		os_delay_us(duration);
	}
}

float noteToFreq(char pitch, char accidental, int octave) {
	
	float frequency;
  
  // Notes for octave 1
  switch (pitch) {
    case 'C':
      frequency = 32.7;
      break;
    case 'D':
      frequency = 36.71;
      break;
    case 'E':
      frequency = 41.21;
      break;
    case 'F':
      frequency = 43.65;
      break;
    case 'G':
      frequency = 49;
      break;
    case 'A':
      frequency = 55;
      break;
    case 'B':
      frequency = 61.74;
      break;	
  }
  
  // Place the note in the correct octave
  frequency = frequency * pow_custom(2,octave - 1);

  // Flatten or sharpen the note depending on accidental   
  if (accidental == 'b') {
    frequency = frequency / 1.05946309436;
  }
  else if (accidental == '#') {
    frequency = frequency * 1.05946309436; 
  }

  return frequency;
}

void playNote(char pitch, char accidental, int octave, float duration) {
  int n = duration / 0.05; 
  float frequency = noteToFreq(pitch,accidental,octave);

  // call function multiple times at the desired frequency 
	for (int i = 0; i < n; i++) {
		playTone(frequency);
	}
}

// Sound Playing functions

void power_on_sound() {
	int octave = 4;
	float t_const = 1;
	
	playNote('D',0,octave,0.375*t_const);
	playNote('E',0,octave,0.125*t_const);
	playNote('F',0,octave,0.375*t_const);
	playNote('G',0,octave,0.125*t_const);
	playNote('E',0,octave,0.5*t_const);
	playNote('C',0,octave,0.25*t_const);
	playNote('D',0,octave,0.75*t_const);
}

void power_off_sound() {
	int octave = 5;
	float t_const = 0.1;
	
	playNote('C',0,octave+1,4.5*t_const);
	playNote('G',0,octave,4.5*t_const);
	playNote('E',0,octave,3*t_const);
	
	playNote('A',0,octave,2*t_const);
	playNote('B',0,octave,2*t_const);
	playNote('A',0,octave,2*t_const);
	
	playNote('A','b',octave,2*t_const);
	playNote('B','b',octave,2*t_const);
	playNote('A','b',octave,2*t_const);
	
	playNote('G',0,octave,2*t_const);
	playNote('F',0,octave,1.5*t_const);
	playNote('G',0,octave,5*t_const);
}

void device_paired_sound() {
	float t_const = 0.4;
	
	playNote('E',0,5,0.35*t_const);
	playNote('E',0,5,0.5*t_const);
	playNote('D',0,4,0.25*t_const);
	playNote('A',0,5,1*t_const);
}

void wifi_on_sound() {
	float t_const = 0.5;
	
	playNote('E',0,5,0.25*t_const);
	playNote('C',0,5,0.5*t_const);
	playNote('A',0,4,0.25*t_const);
	playNote('G',0,5,1*t_const);
}

void wifi_off_sound() {
	float t_const = 0.5;
	
	playNote('E',0,5,0.25*t_const);
	playNote('C',0,5,0.5*t_const);
	playNote('A',0,4,0.25*t_const);
	playNote('G',0,4,1*t_const);
}

void out_of_range_alarm() {
	playNote('D',0,8,0.25);
	playNote('C',0,8,0.25);
}

void ten_step_alert() {
 	float t_const = 0.1;
	
	playNote('A',0,5,2*t_const);
	playNote('B',0,5,2*t_const);
	playNote('A',0,5,2*t_const);
}
/*
void buzzernote(int period){
   for(int j=0; j<1; j++){   
        for(int i=0; i < 1000; i++){
        toggle = !toggle;
        //os_printf("Toggle : %d \t Int: %d\n",toggle, i);
        if (toggle == 1){
            GPIO_OUTPUT_SET(12, 1);
        }
        else{
            GPIO_OUTPUT_SET(12, 0);
        }
        os_delay_us(period/2);
        }
   }
}*/


double pow_custom(double base, int exponent) {
    if (exponent == 0)
        return 1;
    else if (exponent % 2 == 0) {
        double temp = pow_custom(base, exponent / 2);
        return temp * temp;
    } else {
        double temp = pow_custom(base, (exponent - 1) / 2);
        return base * temp * temp;
    }
}
