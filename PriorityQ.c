#include "PCB.h"
#include "FIFOq.h"
#include "PriorityQ.h"
#include "errors.h"
#include "OS.h"
#include <stdlib.h>
#include <stdio.h>


PriorityQ_p PriorityQ_construct() {
	PriorityQ_p priority_q = (PriorityQ_p)malloc(sizeof(PriorityQ));
	return priority_q;
}

void PriorityQ_init(PriorityQ_p priority_queue, int* error) {
	if (priority_queue == NULL) {
		*error = PriorityQNULL;
		printf("priority_queue is NULL on PriorityQ_init(PriorityQ_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	int i;
	for (i = LOWEST_PRIORITY; i <= HIGHEST_PRIORITY; i++) {
		priority_queue->queue_array[i] = FIFOq_construct();
	}
}

void PriorityQ_enqueue(PriorityQ_p priority_queue, PCB_p pcb, int* error) {
	if (priority_queue == NULL) {
		*error = PriorityQNULL;
		printf("priority_queue is NULL on PriorityQ_enqueue(PriorityQ_p, PCB_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	if (pcb == NULL) {
		*error = PCBNULL;
		printf("pcb is NULL on PriorityQ_enqueue(PriorityQ_p, PCB_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	FIFOq_enqueue(priority_queue->queue_array[pcb->priority], pcb, error);
}

PCB_p PriorityQ_dequeue(PriorityQ_p priority_queue, int* error) {
	if (priority_queue == NULL) {
		*error = PriorityQNULL;
		printf("priority_queue is NULL on PriorityQ_dequeue(PriorityQ_p, PCB_p, int*) call. ERROR: %d\n", *error);
		return NULL;
	}
	int i;
	for (i = LOWEST_PRIORITY; i <= HIGHEST_PRIORITY; i++) {
		if (!FIFOq_isEmpty(priority_queue->queue_array[i], error)) {
			return FIFOq_dequeue(priority_queue->queue_array[i], error);
		}
	}
	return NULL;
}