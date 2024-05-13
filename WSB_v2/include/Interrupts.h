#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "osapi.h"
#include "user_interface.h"

int inter_count;

void InitInterrupt();

void handleInterrupt(void *arg);


#endif 