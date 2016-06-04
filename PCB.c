#include "PCB.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>

PCB_p PCB_construct() {
	PCB_p pcb = (PCB_p) malloc(sizeof(PCB));
	return pcb;
}

void PCB_destruct(PCB_p pcb) {
	free(pcb);
}

int PCB_init(PCB_p pcb, int* error, enum pcb_type type) {
	// (STATIC) Counter for the PID of the next process initialized.
	unsigned static long PIDCount = 0;

	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_init(PCB_p, int*) call. ERROR: %d\n", *error);
		return FAILURE;
	}

	pcb->PID = PIDCount;
	pcb->state = new;
	pcb->type = type;
    pcb->PC = 0;
    pcb->creation = time(NULL);
    pcb->termination = -1;      // Not terminated yet.
    pcb->term_count = 0;
	
    PIDCount++;
	return SUCCESS;
}

char* PCB_state(PCB_p pcb, int* error) {
    if (pcb == NULL) {
        *error = PCBNULLERROR;
        printf("PCB is NULL on PCB_toString(PCB_p, int*) call. ERROR: %d\n", *error);
        return (char*)FAILURE;
    }

	switch (pcb->state) {
		case new:
			return "new";
		case ready:
			return "ready";
        case running:
            return "running";
		case interrupted:
			return "interrupted";
		case waiting:
			return "waiting";
        case terminated:
            return "terminated";
	}
}

char* PCB_type(PCB_p pcb, int* error) {
    if (pcb == NULL) {
        *error = PCBNULLERROR;
        printf("PCB is NULL on PCB_toString(PCB_p, int*) call. ERROR: %d\n", *error);
        return (char*)FAILURE;
    }

    switch (pcb->type) {
        case io:
            return "io";
        case compute:
            return "compute";
        case producer:
            return "producer";
        case consumer:
            return "consumer";
        case resource_user:
            return "resource-user";
        case idle:
            return "idle";
    }
}

// TODO: Revise and expand for different types of PCBs (i/o, compute, etc.)
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

	sprintf(str, "PID: 0x%X, Priority: 0x%X, Type: %s, State: %s, PC: 0x%X", pcb->PID, pcb->priority,
            PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC);
	return str;
}
