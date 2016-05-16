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
	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_init(PCB_p, int*) call. ERROR: %d\n", *error);
		return FAILURE;
	}
	pcb->pid = 0;
	pcb->state = halted;
	pcb->priority = 0;
	pcb->pc = 0;
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