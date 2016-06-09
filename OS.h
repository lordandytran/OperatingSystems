#ifndef OS_H
#define OS_H

#include <stdio.h>
#include <memory.h>

#include "FIFOq.h"
#include "PriorityQ.h"
#include "Mutex.h"

#define MAX_PROCESSES 20
#define MAX_PC 4000
#define MIN_PC 1000
#define MAX_TERMINATE 15
#define TIMER_QUANTUM 500
#define STARVATION_THRESHOLD 5

typedef enum tsr_type {io1_trap, io2_trap, terminate_trap, no_trap, mutex_lock_trap, mutex_unlock_trap, condition_signal_and_wait_trap} TSR;
typedef enum interrupt_type {timer_interrupt, io1_interrupt, io2_interrupt, trap_interrupt, no_interrupt} Interrupt;
typedef enum device_type {timer_device, io1_device, io2_device} Device;

#include "CPU.h"

PCB_p current_pcb;
PCB_p idle_pcb;
TSR trap;

FIFOq_p new_PCBs;
PriorityQ_p ready_PCBs;
FIFOq_p io1_PCBs;
FIFOq_p io2_PCBs;
FIFOq_p terminated_PCBs;

// Prototypes
void OS_initialize();
void OS_loop();
void createIOProcesses(int quantity, unsigned short priority);
void populateMutexPCArrays(PCB_p pcb);
void populateIOTrapArrays(PCB_p pcb, int ioDevice);
void createComputeProcesses(int quantity, unsigned short priority);
void createConsumerProducerProcessPairs(int quantity, unsigned short priority);
void createResourceSharingProcesses(int quantity, unsigned short priority);
void execute_ISR(Interrupt interrupt);
void runScheduler(Interrupt interrupt);
void runDispatcher();
void starvationDetection();
void execute_TSR(TSR routine);
void topOffProcesses();

#endif
