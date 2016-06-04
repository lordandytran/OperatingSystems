#ifndef CPU_H
#define CPU_H

#include "OS.h"
#include "FIFOq.h"

// Function prototypes
Interrupt CPU_run();
void CPU_setTimer(int timerAmount);
TSR executeCurrentProcess();
Interrupt checkForInterrupt();
void timerTick(Device device);
int ioRequested(unsigned long* traps, unsigned long PC);
void CPU_initialize();

#endif
