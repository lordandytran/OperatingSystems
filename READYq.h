#ifndef READYq_H
#define READYq_H
#include "PCB.h"
#include "FIFOq.h"
#include <stdlib.h>

typedef struct readyq {

	FIFOq_p queue_array[4];

} READYq;
typedef READYq * READYq_p;


READYq_p READYq_construct(void);
void READYq_init(READYq_p, int*);
void READYq_enqueue(READYq_p, PCB_p, int*);
PCB_p READYq_dequeue(READYq_p, int*);

#endif
