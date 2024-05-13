#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "osapi.h"
#include "user_interface.h"

static int inter_count = 0;

void InitInterrupt();

void handleInterrupt(void *arg);


#endif 