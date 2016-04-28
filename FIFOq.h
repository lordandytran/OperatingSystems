#ifndef FIFOq_H
#define FIFOq_H

#define QNULLERROR 4127
#define NODENULLERROR 4129

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

#endif