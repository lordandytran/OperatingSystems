#ifndef _CPU_H
#define _CPU_H

// Function prototypes
void CPU_cycle();
void executeCurrentProcess();
void checkForInterrupt();
void TSR(int);
void scheduler();
void ISR(int);
void timerTick(int);
int ioRequested(unsigned long* traps, unsigned long PC);
void init();
void populateWithRandomPCBs(FIFOq_p queue, int amount);
void populateIOTrapArrays(PCB_p, int);


#endif
