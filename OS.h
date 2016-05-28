#ifndef OS_H
#define OS_H

#include "PriorityQ.h"

PCB_p current_pcb;

FIFOq_p terminated_PCBs;
FIFOq_p io1_PCBs;
FIFOq_p io2_PCBs;
PriorityQ_p ready_PCBs;

// Prototypes
void initialize();
void populateIOTrapArrays(PCB_p pcb, int ioDevice);

#endif