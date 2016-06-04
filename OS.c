#include "OS.h"
#include "Mutex.h"
#include<stdio.h>

void OS_initialize() {
    // Initialize all of the queues.
    new_PCBs = FIFOq_construct();
    io1_PCBs = FIFOq_construct();
    io2_PCBs = FIFOq_construct();
    ready_PCBs = PriorityQ_construct();
    terminated_PCBs = FIFOq_construct();

    // TODO: Revise
	// Create a an initial set of processes.
    createComputeProcesses((int) (MAX_PROCESSES * 0.05), 0);
    createConsumerProducerProcessPairs(1, 1);
    createConsumerProducerProcessPairs(1, 2);
    createConsumerProducerProcessPairs(1, 3);
    createResourceSharingProcesses(1, 2, 1);
    createResourceSharingProcesses(1, 2, 2);
    createResourceSharingProcesses(1, 2, 3);
    createIOProcesses((int) ((MAX_PROCESSES * 0.8) - 4), 1);
    createIOProcesses((int) ((MAX_PROCESSES * 0.1) - 4), 2);
    createIOProcesses((int) ((MAX_PROCESSES * 0.05) - 4), 3);
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