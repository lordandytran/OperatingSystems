#ifndef OS_H
#define OS_H

#include "PriorityQ.h"

#define TIMER 1
#define TERMINATE 2
#define IO_1 3
#define IO_2 4

#define MAX_PROCESSES 20
#define LOWEST_PRIORITY 3
#define HIGHEST_PRIORITY 0
#define MAX_PC 4000
#define MIN_PC 1000
#define MAX_TERMINATE 15

enum interrupt_type {timer, io1, io2};

PCB_p current_pcb;

FIFOq_p new_PCBs;
PriorityQ_p ready_PCBs;
FIFOq_p io1_PCBs;
FIFOq_p io2_PCBs;
FIFOq_p terminated_PCBs;

// Prototypes
void OS_initialize();
void createIOProcesses(int quantity, unsigned short priority);
void populateIOTrapArrays(PCB_p pcb, int ioDevice);
void createComputeProcesses(int quantity, unsigned short priority);
void createConsumerProducerProcessPairs(int quantity, unsigned short priority);
void createResourceSharingProcesses(int quantity, int processesPerResource, unsigned short priority);

#endif