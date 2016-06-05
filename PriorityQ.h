#ifndef PriorityQ_H
#define PriorityQ_H

#include "PCB.h"
#include "FIFOq.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRIORITY 4

typedef struct PriorityQ {

	FIFOq_p queue_array[MAX_PRIORITY];

} PriorityQ;
typedef PriorityQ * PriorityQ_p;


PriorityQ_p PriorityQ_construct(void);
void PriorityQ_init(PriorityQ_p, int*);
void PriorityQ_enqueue(PriorityQ_p, PCB_p, int*);
PCB_p PriorityQ_dequeue(PriorityQ_p, int*);
int PriorityQ_isEmpty(PriorityQ_p, int*);

#endif
