#include "PCB.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>


PCB_p PCB_construct() {
	PCB_p pcb = (PCB_p)malloc(sizeof(PCB));
	return pcb;
}

void PCB_destruct(PCB_p pcb) {
	free((void*)pcb);
}

int PCB_init(PCB_p pcb, int* error) {
	unsigned static long PIDCount = 0;

	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_init(PCB_p, int*) call. ERROR: %d\n", *error);
		return FAILURE;
	}
	pcb->pid = PIDCount;
	pcb->state = new;
    pcb->maxPC = (unsigned long) (rand() % MAX_PC_VAL); // Set a randomly assigned max PC.
    pcb->pc = 0;
    pcb->creation = time(NULL);
    pcb->termination = -1; // Not terminated yet.
    pcb->terminate = (unsigned int) ((rand() % MAX_TERMINATION_COUNT + 1)); // Set a randomly assigned termination count.
    pcb->term_count = 0;
    pcb->mutex_point = NULL;
	pcb->boost = 0;
	pcb->quanta = 0;


    PIDCount++;
	return SUCCESS;
}

char* PCB_state(PCB_p pcb) {
	char* state = malloc(15);
	switch (pcb->state) {
		case 0:
			state = "new";
			break;
		case 1:
			state = "ready";
			break;
		case 2:
			state = "interrupted";
			break;
		case 3:
			state = "waiting";
			break;
		case 4:
			state = "halted";
			break;
		default:
			state = "FATAL ERROR";
			break;
	}
	return state;
}

/*
char* PCB_toString(PCB_p pcb, int* error) {
	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_toString(PCB_p, int*) call. ERROR: %d\n", *error);
		return (char*)FAILURE;
	}
	char* str = malloc(256);
	if (str == NULL) {
		*error = STRNULLERROR;
		printf("String initialization failure on PCB_toString(PCB_p, int*) call. ERROR: %d\n", *error);
		return (char*)FAILURE;
	}
	sprintf(str, "PID: 0x%X, State: %d, Priority: 0x%X, PC: 0x%X\n", pcb->pid, (int)pcb->state, pcb->priority, pcb->pc);
	return str;
}*/