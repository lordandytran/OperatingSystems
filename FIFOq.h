/*
 * TCSS 422 - Spring 2016
 * Final Project
 * Team 2:
 * Derek Moore
 * Ashton Ohms
 * Anh Tran
 * Vitaliy Radchishin
*/

#ifndef FIFOq_H
#define FIFOq_H

typedef struct LinkedListQueue {

	int size;

	struct Node {
		void* value;
		struct Node *next;
	};

	struct Node *front;
	struct Node *end;

} FIFOq;
typedef FIFOq * FIFOq_p;


FIFOq_p FIFOq_construct(void);
void FIFOq_destruct(FIFOq_p);
void FIFOq_enqueue(FIFOq_p, void*, int*);
void* FIFOq_dequeue(FIFOq_p, int*);
int FIFOq_isEmpty(FIFOq_p, int*);
int FIFOq_getSize(FIFOq_p, int*);
void FIFOq_remove(FIFOq_p queue, void* item, int* error);

#endif