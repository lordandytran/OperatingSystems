#include "FIFOq.h"
#include "PCB.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>


FIFOq_p FIFOq_construct() {
	FIFOq_p queue = (FIFOq_p)malloc(sizeof(FIFOq));
	queue->front = NULL;
	queue->end = NULL;
	queue->size = 0;
	return queue;
}

void FIFOq_destruct(FIFOq_p queue) {
	while (queue->front != NULL) {
		struct Node* current = (struct Node*)malloc(sizeof(struct Node));
		current = queue->front;
		queue->front = queue->front->next;
		free(current);
	}
	free(queue);
}

void FIFOq_enqueue(FIFOq_p queue, void* item, int* error) {
	if (queue == NULL) {
		*error = QNULLERROR;
		printf("queue is NULL on FIFOq_enqueue(FIFOq_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
	if (temp == NULL) {
		*error = NODENULLERROR;
		printf("temp Node is NULL on FIFOq_enqueue(FIFOq_p, int*) call. ERROR: %d\n", *error);
		return;
	}
	temp->value = item;
	if (FIFOq_isEmpty(queue, error)) {
		queue->front = temp;
		queue->end = temp;
	}
	else {
		queue->end->next = temp;
		queue->end = temp;
	}
	queue->end->next = NULL;
	queue->size++;
}

void* FIFOq_dequeue(FIFOq_p queue, int* error) {
	if (queue == NULL) {
		*error = QNULLERROR;
		printf("queue is NULL on FIFOq_dequeue(FIFOq_p, int*) call. ERROR: %d\n", *error);
		return NULL;
	}
	struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
	if (temp == NULL) {
		*error = NODENULLERROR;
		printf("temp Node is NULL on FIFOq_dequeue(FIFOq_p, int*) call. ERROR: %d\n", *error);
		return NULL;
	}
	if (FIFOq_isEmpty(queue, error)) {
		printf("%s", "Queue is empty! No elements dequeued.\n");
		free(temp);
		return NULL;
	}
	else {
		temp = queue->front;
		queue->front = queue->front->next;
		queue->size--;
	}
	return temp->value;
}

int FIFOq_isEmpty(FIFOq_p queue, int* error) {
	if (queue == NULL) {
		*error = QNULLERROR;
		printf("queue is NULL on FIFOq_isEmpty(FIFOq_p, int*) call. ERROR: %d\n", *error);
		return -1;
	}
	if (queue->size == 0) {
		return 1;
	}
	return 0;
}

int FIFOq_getSize(FIFOq_p queue, int* error) {
	if (queue == NULL) {
		*error = QNULLERROR;
		printf("queue is NULL on FIFOq_getSize(FIFOq_p, int*) call. ERROR: %d\n", *error);
		return -1;
	}
	return queue->size;
}

/*
char* FIFOq_toString(FIFOq_p queue, int* error) {
	char* str = malloc(512);
	char* node = malloc(16);
	sprintf(str, "Q:Count=%d: ", FIFOq_getSize(queue, error));
	struct Node* temp = queue->front;
	while (temp != NULL) {
		sprintf(node, "P%u->", ((PCB_p)temp->value)->pid);
 		strcat(str, node);
 		temp = temp->next;
	}
	strcat(str, "*");
	free(node);
	free(temp);
	return str;
}*/
