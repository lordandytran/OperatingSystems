#include "CPU.h"
#include "PCB.h"
#include "FIFOq.h"
#include "PriorityQ.h"

// Timers for each device.
// When the timer is at 1, the device throws an interrupt.
// When the timer is at 0, the device's timer is to its respective quantum.
int ticksRemaining_Timer;   // CPU ticks before next timer interrupt.
int ticksRemaining_IO1;   // CPU ticks before next I/O 1 interrupt.
int ticksRemaining_IO2;   // CPU ticks before next I/O 2 interrupt.

Interrupt CPU_run() {
    Interrupt interrupt = no_interrupt;
    trap = no_trap;
    while (interrupt == no_interrupt) {
        trap = executeCurrentProcess();
        // If a trap has been requested, interrupt the running of this process.
        if(trap != no_trap) {
            return trap_interrupt;
        }

        interrupt = checkForInterrupt();
    }
    return interrupt;
}

void CPU_setTimer(int timerAmount) {
    ticksRemaining_Timer = timerAmount;
}

// Performs the following tasks relating to the execution of a process that occur during a single CPU cycle.
TSR executeCurrentProcess() {
    current_pcb->PC++;

    // If current PC exceeds max PC of the process, reset it, and check if process is to be terminated.
    if (current_pcb->PC > current_pcb->maxPC) {
        current_pcb->PC = 0;
        current_pcb->term_count++;

        // Check if the process is to be terminated.
        if (current_pcb->term_count == current_pcb->terminate) {
            return terminate_trap; // No more execution needed.
        }
    }

    switch (current_pcb->type) {
        case io:
            // Check if the process is to execute an I/O trap during this cycle.
            if (ioRequested(current_pcb->io_1_traps, current_pcb->PC)) {
                return io1_trap;
            } else if (ioRequested(current_pcb->io_2_traps, current_pcb->PC)) {
                return io2_trap;
            }
            break;
        case producer:
            // Note: The assumption is that the producer is running only at the following times:
            // The producer has just been unblocked from a conditional wait (so the mutex is unlocked, variable not incremented)
            if (mutexRequest(current_pcb->lock_pcs, current_pcb->PC)) {
                // It's time to increment the variable, try to acquire the mutex lock.
                return mutex_lock_trap;
                // If the mutex lock fails, the process won't resume execution until it acquires the mutex lock.
                // If the mutex lock is successful, the process will resume execution again here in CPU.
                // The above if statement won't be executed, as the PC will now be incremented one past the mutex lock PC value.
            }
            // The producer has just successfully acquired a mutex lock (so the mutex is locked, variable is not incremented)
            if (current_pcb->shared_resource_mutex->key == current_pcb) {
                // Now that we have the mutex, we may now write to memory.
                (*current_pcb->shared_resource)++;
                printf("Producer #%d writing %d to shared memory.\n", current_pcb->pair_id, *current_pcb->shared_resource);
                // Now that the shared memory is modified, signal the consumer to read it.
                // Request to be blocked until the consumer has read again (and signaled the producer to wake up).
                return condition_signal_and_wait_trap;
            }
            break;
        case consumer:
            // Note: The assumption is that the consumer is running only at the following times:
            // The consumer has just been unblocked from a conditional wait (so the mutex is unlocked, variable not read)
            if (mutexRequest(current_pcb->lock_pcs, current_pcb->PC)) {
                // It's time to read the variable, try to acquire the mutex lock.
                return mutex_lock_trap;
                // If the mutex lock fails, the process won't resume execution until it acquires the mutex lock.
                // If the mutex lock is successful, the process will resume execution again here in CPU.
                // The above if statement won't be executed, as the PC will now be incremented one past the mutex lock PC value.
            }
            // The consumer has just successfully acquired a mutex lock (so the mutex is locked, variable is not read)
            if (current_pcb->shared_resource_mutex->key == current_pcb) {
                // Now that we have the mutex, we may now read from memory.
                printf("Consumer #%d reading %d from shared memory.\n", current_pcb->pair_id, *current_pcb->shared_resource);
                // Now that the shared memory is read, signal the producer to write to it.
                // Request to be blocked until the producer has written again (and signaled the consumer to wake up).
                return condition_signal_and_wait_trap;
            }
            break;
        case resource_user:
            break;
        case compute:
            break;
        case idle:
            break;
    }

    return no_trap;
}

int ioRequested(unsigned long* traps, unsigned long PC) {
    for (int i = 0; i < IO_TRAP_QUANTITY; i++) {
        if(traps[i] == PC) {
            return 1;
        }
    }
    return 0;
}

int mutexRequest(unsigned long *lockPCs, unsigned long PC) {
    for (int i = 0; i < MUTEX_PC_QUANTITY; i++) {
        if (lockPCs[i] == PC) {
            return 1;
        }
    }
    return 0;
}


Interrupt checkForInterrupt() {
    int error;

    if (ticksRemaining_Timer == 1) {
        return timer_interrupt;
    } else {
        ticksRemaining_Timer--;
    }

    if (ticksRemaining_IO1 == 1) {
        ticksRemaining_IO1 = TIMER_QUANTUM * ((3 % rand()) + 2);
        return io1_interrupt;
    } else if (!FIFOq_isEmpty(io1_PCBs, &error)) {
        ticksRemaining_IO1--;
    }

    if (ticksRemaining_IO2 == 1) {
        ticksRemaining_IO2 = TIMER_QUANTUM * ((3 % rand()) + 2);
        return io2_interrupt;
    } else if (!FIFOq_isEmpty(io2_PCBs, &error)) {
        ticksRemaining_IO2--;
    }

    return no_interrupt;
}

// TODO: Revise timer/io quantum resets.
void timerTick(Device device) {
    int error;

    switch (device) {
        case timer_device:
            if (PriorityQ_isEmpty(ready_PCBs, &error)) {
                ticksRemaining_Timer = 0;
            } else if (ticksRemaining_Timer == 0) {
                ticksRemaining_Timer = TIMER_QUANTUM;
            } else {
                ticksRemaining_Timer--;
            }
            break;
        case io1_device:
            if (FIFOq_isEmpty(io1_PCBs, &error)) {
                ticksRemaining_IO1 = 0;
            } else if(ticksRemaining_IO1 == 1) {
                ticksRemaining_IO1 = TIMER_QUANTUM * ((3 % rand()) + 2);
            } else if (ticksRemaining_IO1 > 1){
                ticksRemaining_IO1--;
            }
            break;
        case io2_device:
            if (FIFOq_isEmpty(io2_PCBs, &error)) {
                ticksRemaining_IO2 = 0;
            } else if(ticksRemaining_IO2 == 1) {
                ticksRemaining_IO2 = TIMER_QUANTUM * ((3 % rand()) + 2);
            } else if(ticksRemaining_IO2 > 1){
                ticksRemaining_IO2--;
            }
            break;
        default:
            return; // If an unknown device is to be ticked down, don't do anything.
    }
}

void CPU_initialize() {
    // Setup timers and counters.
    ticksRemaining_Timer = 1;
    ticksRemaining_IO1 = TIMER_QUANTUM * ((3 % rand()) + 2);
    ticksRemaining_IO2 = TIMER_QUANTUM * ((3 % rand()) + 2);
}
