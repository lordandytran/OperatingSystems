#include "OS.h"
#include "Mutex.h"

#define MAX_PROCESSES 20
#define QUANTUM_OF_SOLACE 400

void initialize() {
    // Initialize all of the queues.
    io1_PCBs = FIFOq_construct();
    io2_PCBs = FIFOq_construct();
    ready_PCBs = PriorityQ_construct();
    terminated_PCBs = FIFOq_construct();

	generatePCBS(0, filler, MAX_PROCESSES * 0.05);

	//Producer auto generates a consumer partner (pair)
	generatePCBS(1, producer, 1);
	generatePCBS(1, resource_user, 1);

	//Producer auto generates a consumer partner (pair)
	generatePCBS(2, producer, 1);
	generatePCBS(2, resource_user, 1);

	//Producer auto generates a consumer partner (pair)
	generatePCBS(3, producer, 1);
	generatePCBS(3, resource_user, 1);

	//Populates remaining processes with IO
	fillIO();
	
}

void fillIO() {
	generatePCBS(1, io, (MAX_PROCESSES * 0.80) - ready_PCBs->queue_array[1]->size);
	generatePCBS(2, io, (MAX_PROCESSES * 0.10) - ready_PCBs->queue_array[2]->size);
	generatePCBS(3, io, (MAX_PROCESSES * 0.05) - ready_PCBs->queue_array[3]->size);
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

void os_loop() {
	CPU_quantum();
	scheduler();
}

void scheduler() {


}

void CPU_cycle() {

}

void CPU_quantum() {
	
	int i;
	for (i = 0; i < QUANTUM_OF_SOLACE; i++) {
		current_pcb->pc++;
		// If current PC exceeds max PC of the process, reset it, and check if process is to be terminated.
		if (current_pcb->pc > current_pcb->maxPC) {
			current_pcb->pc = 0;
			current_pcb->term_count++;

			// Check if the process is to be terminated.
			if (current_pcb->term_count == current_pcb->terminate) {
				Trap_Service_Routine(TERMINATE);
				return; // No more execution needed.
			}
		}

		// Check if the process is to execute an I/O trap during this cycle.
		if (current_pcb->type == io && ioRequested(current_pcb->io_1_traps, current_pcb->pc)) {
			Trap_Service_Routine(IO_1);
		}
		else if (current_pcb->type == io && ioRequested(current_pcb->io_2_traps, current_pcb->pc)) {
			Trap_Service_Routine(IO_2);
		}

		if (Interrupt_Service_Routine(i))
			break;
	}
}

int Interrupt_Service_Routine(int current_cycle) {
	if (current_cycle = QUANTUM_OF_SOLACE - 1)
		return (int)Timer;
	return FALSE;
}

void Trap_Service_Routine(int trap) {
	int error = 0;
	char* PCB_string;
	switch (trap) {
	case TIMER:
		current_pcb->state = ready;
		FIFOq_enqueue(ready_PCBs, current_pcb, &error);
		break;
	case TERMINATE:
		current_pcb->state = terminated;
		current_pcb->termination = time(NULL);
		FIFOq_enqueue(terminated_PCBs, current_pcb, error);
		current_pcb = NULL;
		break;
	case IO_1:
		current_pcb->state = waiting;
		FIFOq_enqueue(io1_PCBs, current_pcb, &error);
		break;
	case IO_2:
		current_pcb->state = waiting;
		FIFOq_enqueue(io2_PCBs, current_pcb, &error);
		break;
	default:
		return; // If an unknown trap requested, don't do anything.
	}

	scheduler();
}

int ioRequested(unsigned long* traps, unsigned long PC) {
	for (int i = 0; i < IO_TRAP_QUANTITY; i++) {
		if (traps[i] == PC) {
			return 1;
		}
	}
	return 0;
}