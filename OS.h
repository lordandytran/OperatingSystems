#ifndef OS_H
#define OS_H

#include "PriorityQ.h"
#include "FIFOq.h"
#include "CPU.h"
#include <stdio.h>

#define MAX_PROCESSES 20
#define LOWEST_PRIORITY 3
#define HIGHEST_PRIORITY 0
#define MAX_PC 4000
#define MIN_PC 1000
#define MAX_TERMINATE 15
#define TIMER_QUANTUM 500

typedef enum tsr_type {timer} TSR;

PCB_p current_pcb;
PCB_p idle_pcb;

FIFOq_p new_PCBs;
PriorityQ_p ready_PCBs;
FIFOq_p io1_PCBs;
FIFOq_p io2_PCBs;
FIFOq_p terminated_PCBs;

// Prototypes
void OS_initialize();
void OS_loop();
void createIOProcesses(int quantity, unsigned short priority);
void populateIOTrapArrays(PCB_p pcb, int ioDevice);
void createComputeProcesses(int quantity, unsigned short priority);
void createConsumerProducerProcessPairs(int quantity, unsigned short priority);
void createResourceSharingProcesses(int quantity, int processesPerResource, unsigned short priority);
void execute_ISR(Interrupt interrupt);
void runScheduler(Interrupt interrupt);
void runDispatcher();

#endif