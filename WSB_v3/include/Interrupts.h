#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#define	FPM_SLEEP_MAX_TIME			 0xFFFFFFF


#include "osapi.h"
#include "user_interface.h"

int inter_count;

void InitInterrupt();

void handleInterrupt(void *arg);


#endif 