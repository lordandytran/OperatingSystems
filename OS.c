#include "OS.h"

void OS_initialize() {
    int error;

    // Initialize all of the queues.
    new_PCBs = FIFOq_construct();
    io1_PCBs = FIFOq_construct();
    io2_PCBs = FIFOq_construct();
    ready_PCBs = PriorityQ_construct();
    PriorityQ_init(ready_PCBs, &error);
    terminated_PCBs = FIFOq_construct();

    // Create the idle process and assign it as the first process by running the dispatcher.
    idle_pcb = PCB_construct();
    PCB_init(idle_pcb, &error);
    idle_pcb->PID = (unsigned long) -1;     // Make the idle PID the maximum value of type unsigned long.
    idle_pcb->priority = 4;
    idle_pcb->type = idle;
    idle_pcb->maxPC = MAX_PC;
    idle_pcb->terminate = 0;

    // TODO: Revise
	// Create a an initial set of processes.
    createComputeProcesses((int) (MAX_PROCESSES * 0.05), 0);
    createConsumerProducerProcessPairs(1, 1);
    createConsumerProducerProcessPairs(1, 2);
    createConsumerProducerProcessPairs(1, 3);
    //createResourceSharingProcesses(1, 2, 1);
    //createResourceSharingProcesses(1, 2, 2);
    //createResourceSharingProcesses(1, 2, 3);
    createIOProcesses((int) ((MAX_PROCESSES * 0.8) - 4), 1);
    createIOProcesses((int) ((MAX_PROCESSES * 0.1) - 4), 2);
    createIOProcesses((int) ((MAX_PROCESSES * 0.05) - 4), 3);

    // Initialize the system.
    CPU_initialize();
    runDispatcher();
}

void OS_loop() {
    int error;

    topOffProcesses();

    // Run the current process until the next interrupt or trap call.
    char* string = PCB_toString(current_pcb, &error);
    printf("Now Running: %s\n", string);
    free(string);
    Interrupt interrupt = CPU_run();

    starvationDetection();
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

            printf("I/O 1 Trap request complete.\n");
            runScheduler(io1_interrupt);

            current_pcb->state = running;
            break;
        }
        case io2_interrupt: {
            current_pcb->state = interrupted;

            printf("I/O 2 Trap request complete.\n");
            runScheduler(io2_interrupt);

            current_pcb->state = running;
            break;
        }
        case trap_interrupt: {
            current_pcb->state = interrupted;

            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("Trap requested by %s\n", PCB_string);

            execute_TSR(trap);

            break;
        }
        case no_interrupt:
            // This shouldn't happen here.
            break;
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
                // If this is a boosted PCB, we can unboost it
                if(current_pcb->priority_boost) {
                    current_pcb->priority_boost = 0;
                }

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
            break;
        }
        case io2_interrupt: {
            PCB_p completedIOPCB = FIFOq_dequeue(io2_PCBs, &error);
            completedIOPCB->state = ready;
            PriorityQ_enqueue(ready_PCBs, completedIOPCB, &error);

            char* PCB_string = PCB_toString(completedIOPCB, &error);
            printf("Returned to ready queue: %s\n", PCB_string);
            free(PCB_string);
            break;
        }
        case trap_interrupt:
            // The current PCB has been placed in it's appropriate queue by the TSR,
            // so just run the dispatcher to dispatch the next process in line.
            runDispatcher();
            break;
        case no_interrupt:
            // This shouldn't happen here.
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
    CPU_setTimer(TIMER_QUANTUM);

    char* string = PCB_toString(current_pcb, &error);
    printf("Switching to: %s\n", string);
    free(string);

    current_pcb->state = running;
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

            runScheduler(trap_interrupt);
            break;
        }
        case io2_trap: {
            current_pcb->state = waiting;
            FIFOq_enqueue(io2_PCBs, current_pcb, &error);

            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("I/O 2 Trap requested by %s\n", PCB_string);
            free(PCB_string);

            runScheduler(trap_interrupt);
            break;
        }
        case terminate_trap: {
            current_pcb->state = terminated;
            current_pcb->termination = time(NULL);
            FIFOq_enqueue(terminated_PCBs, current_pcb, &error);

            char* PCB_string = PCB_toString(current_pcb, &error);
            printf("Terminated process: %s\n", PCB_string);
            free(PCB_string);

            current_pcb = NULL;
            runScheduler(trap_interrupt);
            break;
        }
        case mutex_lock_trap:
            printf("PID %lu: requested lock on mutex M%lu - ", current_pcb->PID, current_pcb->shared_resource_mutex->ID);

            if (Mutex_lock(current_pcb->shared_resource_mutex, current_pcb)) {
                // The mutex lock succeeded. Resume process operation.
                printf("succeeded\n");
                return;
            } else {
                // The mutex lock has failed.
                PCB_p blockingPCB = (PCB_p) current_pcb->shared_resource_mutex->key;
                printf("blocked by PID %lu\n", blockingPCB->PID);
                // The process has now been enqueued in the mutex queue, and will get the lock when it is its turn.
                // So enqueue PCB back into the ready queue and run the scheduler to dispatch the next process.
                // TODO: Move the enqueue into the scheduler.
                PriorityQ_enqueue(ready_PCBs, current_pcb, &error);
                runScheduler(trap_interrupt);   // Current process has been blocked, run scheduler to dispatch the next one.
            }
            break;
        case mutex_unlock_trap:
            printf("PID %lu: requested unlock on mutex M%lu - ", current_pcb->PID, current_pcb->shared_resource_mutex->ID);

            if (Mutex_unlock(current_pcb->shared_resource_mutex, current_pcb)) {
                // The mutex unlock succeeded. Resume process operation.
                printf("succeeded\n");
            } else {

            }
            break;
        case condition_signal_and_wait_trap:
            printf("PID %lu: sent signal on condition %lu\n", current_pcb->PID, current_pcb->conditional_variable->ID);

            PCB_p returnedPCB = Condition_signal(current_pcb->conditional_variable, current_pcb);
            if (returnedPCB != NULL) {
                PriorityQ_enqueue(ready_PCBs, returnedPCB, &error);
            }

            printf("PID %lu: requested wait on condition %lu with mutex M%lu\n", current_pcb->PID,
                   current_pcb->conditional_variable->ID, current_pcb->shared_resource_mutex->ID);
            Condition_wait(current_pcb->conditional_variable, current_pcb->shared_resource_mutex, current_pcb);
            // Don't enqueue the current PCB as now it's waiting.

            // Interrupt this PCB and run the scheduler to dispatch the next process.
            runScheduler(trap_interrupt);
            break;
        case no_trap:
            // This shouldn't happen here.
            break;
    }
}

void starvationDetection() {
    int error;

    // The current PCB is getting run time
    current_pcb->starvation_count = 0;

    // Start looping at 1, since there will never be starvation at the
    // highest priority
    for(int i = 1; i < MAX_PRIORITY; i++) {
        FIFOq_p levelQueue = ready_PCBs->queue_array[i];

        if(levelQueue->size > 0) {
            // Increment the starvation count for the front node
            PCB_p head = ((PCB_p)levelQueue->front->value);
            head->starvation_count++;
            //printf("Starvation count for PID %lu: %d\n", head->PID, head->starvation_count);

            // Check if the starvation count exceeds starvation threshold
            if(head->starvation_count > STARVATION_THRESHOLD) {
                printf("PID %lu was starved for %d cycles. Boosting priority level.\n", head->PID,
                       head->starvation_count);

                // Boost it and enqueue it at the next highest queue
                head->priority_boost = 1;
                head->starvation_count = 0;
                PCB_p boosted = FIFOq_dequeue(ready_PCBs->queue_array[i], &error);
                PriorityQ_enqueue(ready_PCBs, boosted, &error);
            }
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

        char* stringPCB = PCB_toStringDetailed(newPCB, &error);
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

        char* stringPCB = PCB_toStringDetailed(newPCB, &error);
        printf("New compute process created: %s\n", stringPCB);
        free(stringPCB);
    }
}

void createConsumerProducerProcessPairs(int quantity, unsigned short priority) {
    static unsigned int currentPair = 0;
    int error;

    for (int i = 0; i < quantity; i++) {
        PCB_p consumerPCB = PCB_construct();
        PCB_init(consumerPCB, &error);
        consumerPCB->priority = priority;
        consumerPCB->type = consumer;
        consumerPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
        consumerPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE
        populateMutexPCArrays(consumerPCB);
        consumerPCB->pair_id = currentPair;

        PCB_p producerPCB = PCB_construct();
        PCB_init(producerPCB, &error);
        producerPCB->priority = priority;
        producerPCB->type = producer;
        producerPCB->maxPC = (unsigned long) ((rand() % MAX_PC) + MIN_PC);   // MIN_PC <= maximum PC of this PCB <= MAX_PC
        producerPCB->terminate = (unsigned int) (rand() % MAX_TERMINATE);    // 0 <= terminate <= MAX_TERMINATE
        populateMutexPCArrays(producerPCB);
        producerPCB->pair_id = currentPair;

        int* sharedResource = malloc(sizeof(int));
        Mutex_p mutex = Mutex_constructor();
        Conditional_p conditional = Conditional_constructor();
        consumerPCB->shared_resource = sharedResource;
        producerPCB->shared_resource = sharedResource;
        consumerPCB->shared_resource_mutex = mutex;
        producerPCB->shared_resource_mutex = mutex;
        consumerPCB->conditional_variable = conditional;
        producerPCB->conditional_variable = conditional;

        FIFOq_enqueue(new_PCBs, consumerPCB, &error);
        FIFOq_enqueue(new_PCBs, producerPCB, &error);
        currentPair++;

        char* stringConsumerPCB = PCB_toStringDetailed(consumerPCB, &error);
        char* stringProducerPCB = PCB_toStringDetailed(producerPCB, &error);
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
            populateMutexPCArrays(newPCB);

            FIFOq_enqueue(new_PCBs, newPCB, &error);

            char* stringPCB = PCB_toStringDetailed(newPCB, &error);
            printf("New resource-using process created: %s\n", stringPCB);
            free(stringPCB);
        }
    }
}

void topOffProcesses() {
    int error;

    // readyPCBs + 1 because there is a process running that is not in the ready queue.
    while (PriorityQ_size(ready_PCBs, &error) + 1 + FIFOq_getSize(io1_PCBs, &error)
           + FIFOq_getSize(io2_PCBs, &error) + FIFOq_getSize(new_PCBs, &error) < MAX_PROCESSES) {
        int type = rand() % 4;
        // This will be the priority of processes that are not compute processes
        // (since priority 0 processes are only computer processes).
        unsigned short priority = (unsigned short) ((unsigned  short) (rand() % 3) + 1);

        switch (type) {
            case 0:
                createIOProcesses(1, priority);
                break;
            case 1:
                createResourceSharingProcesses(1, 2, priority);
                break;
            case 2:
                createConsumerProducerProcessPairs(1, priority);
                break;
            case 3:
                createComputeProcesses(1, 0);
                break;
            default:
                break;
        }
    }
}

void populateMutexPCArrays(PCB_p pcb) {
    unsigned long lockPCs[MUTEX_PC_QUANTITY] = {50, 150, 2500, 3500};
    memcpy(pcb->lock_pcs, lockPCs, MUTEX_PC_QUANTITY * sizeof(unsigned long));

    int unlockPCs[MUTEX_PC_QUANTITY] = {10, 20, 30, 40};
    memcpy(pcb->unlock_pcs, unlockPCs, MUTEX_PC_QUANTITY * sizeof(unsigned long));
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
