#include "PCB.h"
#include "FIFOq.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


char* FIFOq_toString(FIFOq_p, int*);

int main() {

	srand(time(NULL));

	int* error = malloc(sizeof(int*));

	//constructs FIFO queue
	FIFOq_p queue = FIFOq_construct();

	/* Enqueue Test */
	printf("Enqueue Test: \n\n");
	int i;
	for (i = MIN_PRIORITY; i <= MAX_PRIORITY; i++) {

		PCB_p pcb = PCB_construct();
		PCB_init(pcb, error);

		pcb->pid = (unsigned long)i;
		pcb->priority = (unsigned short)(rand() % 31);
		pcb->state = (enum state_type)(rand() % 6);
		pcb->pc = (unsigned long)rand();

		FIFOq_enqueue(queue, pcb, error);
		printf("%s : contents: %s", FIFOq_toString(queue, error), PCB_toString(queue->end->value, error));

	}

	printf("\n\nPress any key to continue with dequeue test: \n\n");
	getchar();

	/* Dequeue Test */
	printf("Dequeue Test: \n\n");
	for (i = FIFOq_getSize(queue, error); i > 0; i--) {
		PCB_p temp = PCB_construct();
		printf("%s\n", FIFOq_toString(queue, error));
		temp = FIFOq_dequeue(queue, error);
		printf("%s\ncontents: %s\n", FIFOq_toString(queue, error), PCB_toString(temp, error));
		free(temp);
	}

	return 0;
}


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
}