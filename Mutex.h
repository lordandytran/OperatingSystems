#ifndef Mutex_H
#define Mutex_H

#include "PCB.h"
#include "FIFOq.h"

#define TRUE 1
#define FALSE 0

typedef struct mutex_t {

	int ident;
	int lock;
	PCB_p pcb;
	FIFOq_p wait;

} Mutex;
typedef Mutex * Mutex_p;

//prototypes
Mutex_p mutex_constructor(int);
int mutex_lock(Mutex_p, PCB_p);
int mutex_unlock(Mutex_p, PCB_p);
int mutex_trylock(Mutex_p, PCB_p);

#endif
