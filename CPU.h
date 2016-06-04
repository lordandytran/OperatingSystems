#ifndef CPU_H
#define CPU_H

typedef enum interrupt_type {timer_interrupt} Interrupt;

// Function prototypes
Interrupt CPU_run();
void CPU_setTimer(int timerAmount);
void executeCurrentProcess();
void checkForInterrupt();
//void TSR(int);
void scheduler();
void ISR(int);
void timerTick(int);
int ioRequested(unsigned long* traps, unsigned long PC);
void CPU_initialize();

#endif
