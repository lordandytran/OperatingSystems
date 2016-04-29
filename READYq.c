#include "PCB.h"
#include "FIFOq.h"
#include "READYq.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>


READYq_p READYq_construct() {
	READYq_p ready_q = (READYq_p)malloc(sizeof(READYq));
	return ready_q;
}

void READYq_init(READYq_p ready_queue, int* error) {
	if (ready_queue == NULL) {
		*error = READYQNULL;
		printf("ready_queue is NULL on READYq_init(READYq_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	int i;
	for (i = MIN_PRIORITY; i <= MAX_PRIORITY; i++) {
		ready_queue->queue_array[i] = FIFOq_construct();
	}
}

void READYq_enqueue(READYq_p ready_queue, PCB_p pcb, int* error) {
	if (ready_queue == NULL) {
		*error = READYQNULL;
		printf("ready_queue is NULL on READYq_enqueue(READYq_p, PCB_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	if (pcb == NULL) {
		*error = PCBNULL;
		printf("pcb is NULL on READYq_enqueue(READYq_p, PCB_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	FIFOq_enqueue(ready_queue->queue_array[pcb->priority], pcb, error);
}

PCB_p READYq_dequeue(READYq_p ready_queue, int* error) {
	if (ready_queue == NULL) {
		*error = READYQNULL;
		printf("ready_queue is NULL on READYq_dequeue(READYq_p, PCB_p, int*) call. ERROR: %d\n", *error);
		return NULL;
	}
	int i;
	for (i = MIN_PRIORITY; i <= MAX_PRIORITY; i++) {
		if (!FIFOq_isEmpty(ready_queue->queue_array[i], error)) {
			return FIFOq_dequeue(ready_queue->queue_array[i], error);
		}
	}
	return NULL;
}