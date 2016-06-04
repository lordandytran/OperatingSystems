#ifndef OS_H
#define OS_H

#include <stdio.h>

#include "FIFOq.h"
#include "PriorityQ.h"

#define MAX_PROCESSES 20
#define MAX_PC 4000
#define MIN_PC 1000
#define MAX_TERMINATE 15
#define TIMER_QUANTUM 500

typedef enum tsr_type {io1_trap, io2_trap, terminate_trap} TSR;
typedef enum interrupt_type {timer_interrupt, io1_interrupt, io2_interrupt, trap_interrupt} Interrupt;
typedef enum device_type {timer_device, io1_device, io2_device} Device;

#include "CPU.h"

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
void execute_TSR(TSR routine);

#endif