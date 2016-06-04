#include "OS.h"

void OS_initialize() {
    int error;

    // Create the idle process and assign it as the first process.
    idle_pcb = PCB_construct();
    PCB_init(idle_pcb, &error);
    idle_pcb->priority = 4;
    idle_pcb->type = idle;
    idle_pcb->maxPC = MAX_PC;
    idle_pcb->terminate = 0;
    current_pcb = idle_pcb;

    // Initialize all of the queues.
    new_PCBs = FIFOq_construct();
    io1_PCBs = FIFOq_construct();
    io2_PCBs = FIFOq_construct();
    ready_PCBs = PriorityQ_construct();
    PriorityQ_init(ready_PCBs, &error);
    terminated_PCBs = FIFOq_construct();

    // TODO: Revise
	// Create a an initial set of processes.
    //createComputeProcesses((int) (MAX_PROCESSES * 0.05), 0);
    createConsumerProducerProcessPairs(1, 1);
    createConsumerProducerProcessPairs(1, 2);
    createConsumerProducerProcessPairs(1, 3);
    createResourceSharingProcesses(1, 2, 1);
    createResourceSharingProcesses(1, 2, 2);
    createResourceSharingProcesses(1, 2, 3);
    createIOProcesses((int) ((MAX_PROCESSES * 0.8) - 4), 1);
    createIOProcesses((int) ((MAX_PROCESSES * 0.1) - 4), 2);
    createIOProcesses((int) ((MAX_PROCESSES * 0.05) - 4), 3);

    // Initialize the system.
    CPU_initialize();   // Sets up the CPU so that a timer interrupt occurs immediately to allow scheduler to run.
}

void OS_loop() {
    // TODO: Generate new processes.
    int error;

    // Run the current process until the next interrupt or trap call.
    char* string = PCB_toString(current_pcb, &error);
    printf("Now Running: %s\n", string);
    free(string);
    Interrupt interrupt = CPU_run();

    execute_ISR(interrupt);
}

void execute_ISR(Interrupt interrupt) {
    int error;

    switch (interrupt) {
        case timer_interrupt: {
            current_pcb->state = interrupted;

            char *PCB_string = PCB_toString(current_pcb, &error);
            printf("Timer interrupt during %s\n", PCB_string);
            free(PCB_string);

            runScheduler(timer_interrupt);
            break;
        }
        case io1_interrupt: {
            current_pcb->state = interrupted;

            printf("I/O 1 Trap request complete.");
            runScheduler(io1_interrupt);

            current_pcb->state = running;
            break;
        }
        case io2_interrupt: {
            current_pcb->state = interrupted;

            printf("I/O 2 Trap request complete.");
            runScheduler(io2_interrupt);

            current_pcb->state = running;
            break;
        }
        case trap_interrupt: {
            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("Trap interrupt during %s\n", PCB_string);
            runScheduler(trap_interrupt);
        }
    }
}

void runScheduler(Interrupt interrupt) {
    int error;

    // Add any newly created PCBs to the ready queue.
    while (!FIFOq_isEmpty(new_PCBs, &error)) {
        PCB_p newPCB = FIFOq_dequeue(new_PCBs, &error);
        newPCB->state = ready;
        PriorityQ_enqueue(ready_PCBs, newPCB, &error);
        char* string = PCB_toString(newPCB, &error);
        printf("Added to ready queue: %s\n", string);
        free(string);
    }

    switch (interrupt) {
        case timer_interrupt:
            current_pcb->state = ready;

            // Put the interrupted process into the ready queue (if it's not the idle process).
            if (current_pcb != idle_pcb) {
                PriorityQ_enqueue(ready_PCBs, current_pcb, &error);
                char* string = PCB_toString(current_pcb, &error);
                printf("Returned to ready queue: %s\n", string);
                free(string);
            }

            runDispatcher();
            break;
        case io1_interrupt: {
            PCB_p completedIOPCB = FIFOq_dequeue(io1_PCBs, &error);
            completedIOPCB->state = ready;
            PriorityQ_enqueue(ready_PCBs, completedIOPCB, &error);

            char* PCB_string = PCB_toString(completedIOPCB, &error);
            printf("Returned to ready queue: %s\n", PCB_string);
            free(PCB_string);
        }
        case io2_interrupt: {
            PCB_p completedIOPCB = FIFOq_dequeue(io2_PCBs, &error);
            completedIOPCB->state = ready;
            PriorityQ_enqueue(ready_PCBs, completedIOPCB, &error);

            char* PCB_string = PCB_toString(completedIOPCB, &error);
            printf("Returned to ready queue: %s\n", PCB_string);
            free(PCB_string);
        }
        case trap_interrupt:
            // TODO: Revise
            // The current PCB has been placed in it's appropriate queue, so just run the dispatcher to start the next process.
            runDispatcher();
            break;
    }

    // Housekeeping work.
    // Free all terminated PCBs.
    while (!FIFOq_isEmpty(terminated_PCBs, &error)) {
        PCB_p terminatedPCB = FIFOq_dequeue(terminated_PCBs, &error);
        char* string = PCB_toString(terminatedPCB, &error);
        printf("Freeing terminated PCB: %s\n", string);
        free(string);
        PCB_destruct(terminatedPCB);
    }
}

void runDispatcher() {
    int error;

    if(PriorityQ_isEmpty(ready_PCBs, &error)) {
        current_pcb = idle_pcb;
    } else {
        current_pcb = PriorityQ_dequeue(ready_PCBs, &error);
    }
    CPU_setTimer(TIMER_QUANTUM);    // Not sure if the dispatcher should do this.

    char* string = PCB_toString(current_pcb, &error);
    printf("Switching to: %s\n", string);
    free(string);
}

void execute_TSR(TSR routine) {
    int error;

    switch (routine) {
        case io1_trap: {
            current_pcb->state = waiting;
            FIFOq_enqueue(io1_PCBs, current_pcb, &error);

            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("I/O 1 Trap requested by %s\n", PCB_string);
            free(PCB_string);
            break;
        }
        case io2_trap: {
            current_pcb->state = waiting;
            FIFOq_enqueue(io2_PCBs, current_pcb, &error);

            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("I/O 2 Trap requested by %s\n", PCB_string);
            free(PCB_string);
            break;
        }
        case terminate_trap: {
            current_pcb->state = terminated;
            current_pcb->termination = time(NULL);
            FIFOq_enqueue(terminated_PCBs, current_pcb, &error);

            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("Terminating: %s\n", PCB_string);
            free(PCB_string);

            current_pcb = NULL;
            break;
        }
    }
}

void createIOProcesses(int quantity, unsigned short priority) {
    int error;

    for (int i = 0; i < quantity; i++) {
        PCB_p newPCB = PCB_construct();
        PCB_init(newPCB, &error);
        newPCB->priority = priority;
        newPCB->type = io;
        newPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
        newPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE

        populateIOTrapArrays(newPCB, 1);
        populateIOTrapArrays(newPCB, 2);

        FIFOq_enqueue(new_PCBs, newPCB, &error);

        char* stringPCB = PCB_toString(newPCB, &error);
        printf("New IO process created: %s\n", stringPCB);
        free(stringPCB);
    }
}

void createComputeProcesses(int quantity, unsigned short priority) {
    int error;

    for (int i = 0; i < quantity; i++) {
        PCB_p newPCB = PCB_construct();
        PCB_init(newPCB, &error);
        newPCB->priority = priority;
        newPCB->type = compute;
        newPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
        newPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE

        FIFOq_enqueue(new_PCBs, newPCB, &error);

        char* stringPCB = PCB_toString(newPCB, &error);
        printf("New compute process created: %s\n", stringPCB);
        free(stringPCB);
    }
}

// TODO: Add mutex stuff
void createConsumerProducerProcessPairs(int quantity, unsigned short priority) {
    int error;

    for (int i = 0; i < quantity; i++) {
        PCB_p consumerPCB = PCB_construct();
        PCB_init(consumerPCB, &error);
        consumerPCB->priority = priority;
        consumerPCB->type = consumer;
        consumerPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
        consumerPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE

        PCB_p producerPCB = PCB_construct();
        PCB_init(producerPCB, &error);
        producerPCB->priority = priority;
        producerPCB->type = producer;
        producerPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
        producerPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE

        FIFOq_enqueue(new_PCBs, consumerPCB, &error);
        FIFOq_enqueue(new_PCBs, producerPCB, &error);

        char* stringConsumerPCB = PCB_toString(consumerPCB, &error);
        char* stringProducerPCB = PCB_toString(producerPCB, &error);
        printf("New consumer/producer process pair created: %s | %s\n", stringProducerPCB, stringConsumerPCB);
        free(stringConsumerPCB);
        free(stringProducerPCB);

    }
}

void createResourceSharingProcesses(int quantity, int processesPerResource, unsigned short priority) {
    int error;

    for (int i = 0; i < quantity; i++) {
        for (int j = 0; j < processesPerResource; j++) {
            PCB_p newPCB = PCB_construct();
            PCB_init(newPCB, &error);
            newPCB->priority = priority;
            newPCB->type = resource_user;
            newPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
            newPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE

            FIFOq_enqueue(new_PCBs, newPCB, &error);

            char* stringPCB = PCB_toString(newPCB, &error);
            printf("New resource-using process created: %s\n", stringPCB);
            free(stringPCB);
        }
    }
}

// Populates the passed I/O device's array of the passed PCB with random PC values.
// Ensures that PC values are unique.
void populateIOTrapArrays(PCB_p pcb, int ioDevice) {
    unsigned long max = pcb->maxPC;

    unsigned long num0 = rand() % max;
    unsigned long num1 = rand() % max;
    unsigned long num2 = rand() % max;
    unsigned long num3 = rand() % max;

    // Ensure num1 is unique.
    while (num1 == num0) {
        num1 = rand() % max;
    }

    // Ensure num2 is unique.
    while (num2 == num0 || num2 == num1) {
        num2 = rand() % max;
    }

    // Ensure num3 is unique.
    while (num3 == num0 || num3 == num1 || num3 == num2) {
        num3 = rand() % max;
    }

    // Set it to appropriate array in PCB.
    // This is very poor practice, but it avoids memory leaks.
    // If time permits we can convert the arrays to be instantiated here, and destroyed in PCB_destruct().
    if (ioDevice == 1) {
        pcb->io_1_traps[0] = num0;
        pcb->io_1_traps[1] = num1;
        pcb->io_1_traps[2] = num2;
        pcb->io_1_traps[3] = num3;
    }
    else if (ioDevice == 2) {
        pcb->io_2_traps[0] = num0;
        pcb->io_2_traps[1] = num1;
        pcb->io_2_traps[2] = num2;
        pcb->io_2_traps[3] = num3;
    }
}