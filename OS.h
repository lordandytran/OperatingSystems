#ifndef OS_H
#define OS_H

#include "PriorityQ.h"

#define TIMER 1
#define TERMINATE 2
#define IO_1 3
#define IO_2 4

enum interrupt_type {Timer, IO1, IO2};

PCB_p current_pcb;

FIFOq_p terminated_PCBs;
FIFOq_p io1_PCBs;
FIFOq_p io2_PCBs;
PriorityQ_p ready_PCBs;

// Prototypes
void initialize();
void generatePCBs(unsigned short, enum pcb_type, int);
void populateIOTrapArrays(PCB_p pcb, int ioDevice);
void fillIO();
void os_loop();
void scheduler();
void CPU_quantum();
int Interrupt_Service_Routine(int);
void Trap_Service_Routine(int);
int ioRequested(unsigned long*, unsigned long);

#endif