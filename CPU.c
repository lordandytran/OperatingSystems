#include "FIFOq.h"
#include "PCB.h"
#include "CPU.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define QUANTUM 300
#define PCB_INIT_CNT 30
#define TIMER 1
#define TERMINATE 2
#define IO1 3
#define IO2 4

// Timers for each device.
// When the timer is at 1, the device throws an interrupt.
// When the timer is at 0, the device's timer is to its respective quantum.
int ticksRemaining_Timer;   // CPU ticks before next timer interrupt.
int ticksRemaining_IO1;   // CPU ticks before next I/O 1 interrupt.
int ticksRemaining_IO2;   // CPU ticks before next I/O 2 interrupt.

Interrupt CPU_run() {
    return timer_interrupt;
    executeCurrentProcess();
    checkForInterrupt();
}

void CPU_setTimer(int timerAmount) {
    ticksRemaining_Timer = timerAmount;
}

// Performs the following tasks relating to the execution of a process that occur during a single CPU cycle.
/*void executeCurrentProcess() {
    currentPCB->pc++;

    // If current PC exceeds max PC of the process, reset it, and check if process is to be terminated.
    if (currentPCB->pc > currentPCB->max_pc) {
        currentPCB->pc = 0;
        currentPCB->term_count++;

        // Check if the process is to be terminated.
        if (currentPCB->term_count == currentPCB->terminate) {
            //TSR(TERMINATE);
            return; // No more execution needed.
        }
    }

    // Check if the process is to execute an I/O trap during this cycle.
    if(ioRequested(currentPCB->io_1_traps, currentPCB->pc)) {
        //TSR(IO1);
    } else if(ioRequested(currentPCB->io_2_traps, currentPCB->pc)) {
        //TSR(IO2);
    }
}*/

/*int ioRequested(unsigned long* traps, unsigned long PC) {
    for (int i = 0; i < IO_TRAPS; i++) {
        if(traps[i] == PC) {
            return 1;
        }
    }
    return 0;
}*/

/*
void checkForInterrupt() {
    // Check for timer interrupt, call timer ISR if timer interrupt has occurred.
    if (ticksRemaining_Timer == 1) {
        printf("timer\n");
        ISR(TIMER);
    }
    timerTick(TIMER);

    // Check for I/O 1 interrupt, call I/O 1 ISR if I/O 1 interrupt has occurred.
    if (ticksRemaining_IO1 == 1) {
        printf("isr1\n");
        ISR(IO1);
    }
    timerTick(IO1);

    // Check for I/O 2 interrupt, call I/O 2 ISR if I/O 2 interrupt has occurred.
    if (ticksRemaining_IO2 == 1) {
        printf("isr2\n");
        ISR(IO2);
    }
    timerTick(IO2);
}*/

/*void TSR(int trap) {
    char* PCB_string;
    switch (trap) {
        case TIMER:
            currentPCB->state = ready;
            FIFOq_enqueue(readyPCBs, currentPCB);

            PCB_string = PCB_toString(currentPCB);
            printf("Timer interrupt during %s\n", PCB_string);
            free(PCB_string);
            break;
        case TERMINATE:
            currentPCB->state = terminated;
            currentPCB->termination = time(NULL);
            FIFOq_enqueue(terminatedPCBs, currentPCB);

            PCB_string = PCB_toString(currentPCB);
            printf("Terminating %s\n", PCB_string);
            free(PCB_string);

            currentPCB = NULL;
            break;
        case IO1:
            currentPCB->state = waiting;
            FIFOq_enqueue(waitIO1, currentPCB);

            PCB_string = PCB_toString(currentPCB);
            printf("I/O 1 Trap requested by %s\n", PCB_string);
            free(PCB_string);
            break;
        case IO2:
            currentPCB->state = waiting;
            FIFOq_enqueue(waitIO2, currentPCB);

            PCB_string = PCB_toString(currentPCB);
            printf("I/O 2 Trap requested by %s\n", PCB_string);
            free(PCB_string);
            break;
        default:
            return; // If an unknown trap requested, don't do anything.
    }

    scheduler();
}*/

void ISR(int interrupt) {
    PCB_p completedIOPCB;
    char* PCB_string;

    switch (interrupt) {
        case TIMER:
            //TSR(TIMER);
            break;
        /*case IO1:
            printf("%d: %d\n", 1, FIFOq_size(waitIO1));
            completedIOPCB = FIFOq_dequeue(waitIO1);
            completedIOPCB->state = ready;
            FIFOq_enqueue(readyPCBs, completedIOPCB);

            PCB_string = PCB_toString(completedIOPCB);
            printf("I/O 1 Trap requested completed by %s\n", PCB_string);
            free(PCB_string);
            break;
        case IO2:
            printf("%d: %d\n", 2, FIFOq_size(waitIO2));
            completedIOPCB = FIFOq_dequeue(waitIO2);
            completedIOPCB->state = ready;
            FIFOq_enqueue(readyPCBs, completedIOPCB);

            PCB_string = PCB_toString(completedIOPCB);
            printf("I/O 2 Trap requested completed by %s\n", PCB_string);
            free(PCB_string);
            break;*/
        default:
            return; // If an unknown device threw an interrupt, don't do anything.
    }

}


/*void timerTick(int device) {
    switch (device) {
        case TIMER:
            if (FIFOq_is_empty(readyPCBs)) {
                ticksRemaining_Timer = 0;
            } else if (ticksRemaining_Timer == 0) {
                ticksRemaining_Timer = QUANTUM;
            } else {
                ticksRemaining_Timer--;
            }
            break;
        case IO1:
            if (FIFOq_is_empty(waitIO1)) {
                ticksRemaining_IO1 = 0;
            } else if(ticksRemaining_IO1 <= 0) {
                ticksRemaining_IO1 = QUANTUM * ((3 % rand()) + 2);
            } else{
                ticksRemaining_IO1--;
            }
            break;
        case IO2:
            if (FIFOq_is_empty(waitIO2)) {
                ticksRemaining_IO2 = 0;
            } else if(ticksRemaining_IO2 <= 0) {
                ticksRemaining_IO2 = QUANTUM * ((3 % rand()) + 2);
            } else {
                ticksRemaining_IO2--;
            }
            break;
        default:
            return; // If an unknown device is to be ticked down, don't do anything.
    }
}*/

void CPU_initialize() {
    // Setup timers and counters.
    ticksRemaining_Timer = QUANTUM;
    ticksRemaining_IO1 = 0;   // I/O 1 wait queue initially empty.
    ticksRemaining_IO2 = 0;   // I/O 2 wait queue initially empty.
}