#include "Mutex.h"
#include "PCB.h"
#include "FIFOq.h"
#include "errors.h"

#include <stdlib.h>

Mutex_p mutex_constructor(int ident) {
	Mutex_p mutex = (Mutex_p)malloc(sizeof(Mutex));
	mutex->ident = ident;
	mutex->pcb = NULL;
	mutex->wait = FIFOq_construct();
	mutex->lock = TRUE;
	return mutex;
}

int Mutex_lock(Mutex_p mut, PCB_p pcb) {
	int* error = 0;
	if (mut->pcb == NULL) {
		mut->pcb = pcb;
		mut->lock = TRUE;
		return TRUE;
	}
	else {
		FIFOq_enqueue(mut->wait, pcb);
		return FALSE;
	}
}

int Mutex_unlock(Mutex_p mut, PCB_p pcb) {

}

int Mutex_trylock(Mutex_p mut, PCB_p pcb) {

}
