#include "OS.h"
#include "Mutex.h"

void initialize() {
    // Initialize all of the queues.
    io1_PCBs = FIFOq_construct();
    io2_PCBs = FIFOq_construct();
    ready_PCBs = PriorityQ_construct();
    terminated_PCBs = FIFOq_construct();


}

void generatePCBs(unsigned short priority, enum pcb_type type, int amount) {
    int error;
    for(int i = 0; i < amount; i++) {
        PCB_p newPCB = PCB_construct();
        PCB_init(newPCB, &error);
        newPCB->type = type;
        newPCB->priority = priority;
        newPCB->state = ready;


        Mutex_p producer_Mutex;


        switch (type) {
            case filler:
                break;
            case io:
                populateIOTrapArrays(newPCB, 1);
                populateIOTrapArrays(newPCB, 2);
                break;
            case resource_user: {
                PCB_p pardner = PCB_construct();
                PCB_init(pardner, &error);
                pardner->type = resource_user;
                pardner->priority = priority;
                pardner->state = ready;
                producer_Mutex = Mutex_constructor();
                pardner->mutex_point = producer_Mutex;
                newPCB->mutex_point = producer_Mutex;
                PriorityQ_enqueue(ready_PCBs, pardner, &error);
                break;
            }
            case producer: {
                PCB_p pardner = PCB_construct();
                PCB_init(pardner, &error);
                pardner->type = consumer;
                pardner->priority = priority;
                pardner->state = ready;
                producer_Mutex = Mutex_constructor();
                pardner->mutex_point = producer_Mutex;
                newPCB->mutex_point = producer_Mutex;
                PriorityQ_enqueue(ready_PCBs, pardner, &error);
                break;
            }

        }

        PriorityQ_enqueue(ready_PCBs, newPCB, &error);
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