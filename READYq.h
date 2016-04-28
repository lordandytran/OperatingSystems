#ifndef READYq_H
#define READYq_H
#include "PCB.h"
#include "FIFOq.h"
#include <stdlib.h>

#define READYQNULL 7187
#define PCBNULL 7193

typedef struct readyq {
	
	FIFOq_p queue_array[16];

} READYq;
typedef READYq * READYq_p;


READYq_p READYq_construct(void);
void READYq_init(READYq_p, int*);
void READYq_enqueue(READYq_p, PCB_p, int*);
PCB_p READYq_dequeue(READYq_p, int*);

#endif