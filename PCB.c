#include "PCB.h"
#include <stdlib.h>
#include <stdio.h>

PCB_p PCB_construct() {
	PCB_p pcb = (PCB_p) malloc(sizeof(struct pcb_t));
	return pcb;
}

void PCB_destruct(PCB_p pcb) {
	free(pcb);
}

int PCB_init(PCB_p pcb, int* error) {
	// (STATIC) Counter for the PID of the next process initialized.
	unsigned static long PIDCount = 0;

	if (pcb == NULL) {
		*error = PCBNULLERROR;
		printf("PCB is NULL on PCB_init(PCB_p, int*) call. ERROR: %d\n", *error);
		return FAILURE;
	}

	pcb->PID = PIDCount;
	pcb->state = new;
    pcb->PC = 0;
    pcb->creation = time(NULL);
    pcb->termination = -1;      // Not terminated yet.
    pcb->term_count = 0;

	pcb->priority_boost = 0;
	pcb->starvation_count = 0;

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

char* PCB_toStringDetailed(PCB_p pcb, int* error) {
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
    str[0] = '\0';

    switch (pcb->type) {
        case io: {
            char *io1 = toStringIOArray(pcb, error, 1);
            char *io2 = toStringIOArray(pcb, error, 2);

            sprintf(str, "PID: 0x%X, Priority: 0x%X, Type: %s, State: %s, PC: 0x%X, maxPC: 0x%X, Terminate: 0x%X, I/O 1: %s, I/O 2: %s",
                    pcb->PID, pcb->priority, PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC, pcb->maxPC, pcb->terminate, io1, io2);

            free(io1);
            free(io2);
            break;
        }
        case compute:
            sprintf(str, "PID: 0x%X, Priority: 0x%X, Type: %s, State: %s, PC: 0x%X, maxPC: 0x%X, Terminate: 0x%X",
                    pcb->PID, pcb->priority, PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC, pcb->maxPC, pcb->terminate);
            break;
        case producer:
            sprintf(str, "PID: 0x%X, Priority: 0x%X, Type: %s, State: %s, PC: 0x%X, maxPC: 0x%X, Terminate: 0x%X, Pair ID: 0x%X",
                    pcb->PID, pcb->priority, PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC, pcb->maxPC, pcb->terminate, pcb->pair_id);
            break;
        case consumer:
            sprintf(str, "PID: 0x%X, Priority: 0x%X, Type: %s, State: %s, PC: 0x%X, maxPC: 0x%X, Terminate: 0x%X, Pair ID: 0x%X",
                    pcb->PID, pcb->priority, PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC, pcb->maxPC, pcb->terminate, pcb->pair_id);
            break;
        case resource_user: {
            char *lock = toStringLockUnlockArray(pcb, error, 1);
            char *unlock = toStringLockUnlockArray(pcb, error, 2);

            sprintf(str, "PID: 0x%X, Priority: 0x%X, Type: %s, State: %s, PC: 0x%X, maxPC: 0x%X, Terminate: 0x%X, I/O 1: %s, I/O 2: %s",
                    pcb->PID, pcb->priority, PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC, pcb->maxPC, pcb->terminate, lock, unlock);

            free(lock);
            free(unlock);
            break;
        }
        case idle:
            sprintf(str, "PID: 0x%X, Type: %s, State: %s, PC: 0x%X",
                    pcb->PID, PCB_type(pcb, error), PCB_state(pcb, error), pcb->PC);
            break;
    }

    return str;
}

char* toStringIOArray(PCB_p pcb, int* error, int ioNum) {
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
    str[0] = '\0';

    switch (ioNum) {
        case 1:
            sprintf(str, "[0x%X, 0x%X, 0x%X, 0x%X]", pcb->io_1_traps[0], pcb->io_1_traps[1], pcb->io_1_traps[2], pcb->io_1_traps[3]);
            break;
        case 2:
            sprintf(str, "[0x%X, 0x%X, 0x%X, 0x%X]", pcb->io_2_traps[0], pcb->io_2_traps[1], pcb->io_2_traps[2], pcb->io_2_traps[3]);
            break;
        default:
            return "[]";
    }
    return str;
}

char* toStringLockUnlockArray(PCB_p pcb, int* error, int selection) {
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
    str[0] = '\0';

    switch (selection) {
        case 1:
            sprintf(str, "[0x%X, 0x%X, 0x%X, 0x%X]", pcb->lock_pcs[0], pcb->lock_pcs[1], pcb->lock_pcs[2], pcb->lock_pcs[3]);
            break;
        case 2:
            sprintf(str, "[0x%X, 0x%X, 0x%X, 0x%X]", pcb->unlock_pcs[0], pcb->unlock_pcs[1], pcb->unlock_pcs[2], pcb->unlock_pcs[3]);
            break;
        default:
            return "[]";
    }
    return str;
}
