#include "Mutex.h"
#include "PCB.h"
#include "FIFOq.h"
#include "errors.h"

#include <stdlib.h>

Mutex_p Mutex_constructor(unsigned long ident) {
	Mutex_p mutex = (Mutex_p)malloc(sizeof(Mutex));
	mutex->ID = ident;
	mutex->pcbID = 0;
	mutex->locked = FALSE;
	mutex->wait = FIFOq_construct();
	return mutex;
}

//TRUE if Successful. FALSE if lock failed.
int Mutex_lock(Mutex_p mut, PCB_p pcb) {
	int* error = 0;
	if (mut->pcbID == 0 || mut->pcbID == pcb->pid) {
		mut->pcbID = pcb->pid;
		mut->locked = TRUE;
		return TRUE;
	}
	else {
		FIFOq_enqueue(mut->wait, pcb, error);
		return FALSE;
	}
}

//TRUE if Successful. FALSE if unlock failed.
int Mutex_unlock(Mutex_p mut, PCB_p pcb) {
	int* error = 0;
	if (mut->locked == FALSE)
		return TRUE;
	if (pcb->pid == mut->pcbID) {
		mut->locked = FALSE;
		return TRUE;
	}
	FIFOq_enqueue(mut->wait, pcb, error);
	return FALSE;
}

//replace controller with given and locks. True if successful. False if failed.
int Mutex_trylock(Mutex_p mut, PCB_p pcb) {
	int* error = 0;
	if (mut->locked == FALSE) {
		mut->pcbID = pcb->pid;
		mut->locked = TRUE;
		return TRUE;
	}
	else {
		FIFOq_enqueue(mut->wait, pcb, error);
		return FALSE;
	}
}

//Force unlocks and replaces controller with next in queue.
void Mutex_next_Controller(Mutex_p mut) {
	int* error = 0;
	mut->pcbID = ((PCB_p)FIFOq_dequeue(mut->wait, error))->pid;
	mut->locked = FALSE;
}

int Mutex_Is_Locked(Mutex_p mut) {
	if (mut->locked)
		return TRUE;
	return FALSE;
}
