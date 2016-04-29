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

int PCB_set_pid(PCB_p pcb, unsigned long pid, int* error) {
	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_set_pid(PCB_p, unsigned long, int*) call. ERROR: %d\n", *error);
		return FAILURE;
	}
	pcb->pid = pid;
	return SUCCESS;
}

unsigned long PCB_get_pid(PCB_p pcb, int* error) {
	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_get_pid(PCB_p, int*) call. ERROR: %d\n", *error);
		return FAILURE;
	}
	return pcb->pid;
}

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
}