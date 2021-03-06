/*
 * TCSS 422 - Spring 2016
 * Final Project
 * Team 2:
 * Derek Moore
 * Ashton Ohms
 * Anh Tran
 * Vitaliy Radchishin
*/

#include "Mutex.h"

#include <stdlib.h>

Mutex_p Mutex_constructor() {
	static unsigned long id = 0;
	Mutex_p mutex = (Mutex_p)malloc(sizeof(struct mutex_t));
	mutex->ID = id;
	mutex->key = NULL;
	mutex->locked = FALSE;
	mutex->wait = FIFOq_construct();
	mutex->destroyed = FALSE;
    id++;
	return mutex;
}

//TRUE if Successful. FALSE if lock failed.
int Mutex_lock(Mutex_p mut, PCB_p pcb) {
	int error = 0;
	if (mut->key == NULL || mut->key == pcb) {
		mut->key = pcb;
		mut->locked = TRUE;
		return TRUE;
	}
	else {
		FIFOq_enqueue(mut->wait, pcb, &error);
		return FALSE;
	}
}

//TRUE if Successful. FALSE if unlock failed.
int Mutex_unlock(Mutex_p mut, PCB_p pcb) {
	int error = 0;

    // If the mutex is already unlocked.
	if (mut->locked == FALSE)
		return TRUE;

    // If the mutex is locked by the passed pcb, unlock it.
	if (pcb == mut->key) {
		mut->locked = FALSE;

        // If there are other PCBs waiting to acquire the lock, give them priority.
        if(mut->wait->size != 0) {
            mut->key = FIFOq_dequeue(mut->wait, &error);
            FIFOq_enqueue(mut->wait, pcb, &error);
        } else {
            mut->key = NULL;
        }
		return TRUE;
	}
	return FALSE;
}

//replace controller with given PCB and locks. True if successful. False if failed.
int Mutex_trylock(Mutex_p mut, PCB_p pcb) {
	int error = 0;
	if (mut->locked == FALSE) {
		mut->key = pcb;
		mut->locked = TRUE;
		return TRUE;
	}
	else {
		//FIFOq_enqueue(mut->wait, pcb, &error); try_lock doesn't put the pcb into the waiting queue.
		return FALSE;
	}
}

// Removes all instances of the passed pcbs in the waiting list and the key. Useful for process termination.
void Mutex_remove(Mutex_p mut, PCB_p pcb) {
    int error = 0;
    Mutex_unlock(mut, pcb);
    FIFOq_remove(mut->wait, pcb, &error);
}



Conditional_p Conditional_constructor() {
    Conditional_p conditional = malloc(sizeof(struct conditional_t));
    static unsigned long id_counter = 0;
    conditional->ID = id_counter;
    id_counter++;
    conditional->waitingPCB = NULL;
    conditional->mutex = NULL;

	return conditional;
}

void Conditional_destructor(Conditional_p conditional) {
    // If the waiting PCB hasn't been saved and its reference set to NULL, too bad.
    if (conditional->waitingPCB != NULL) {
        PCB_destruct(conditional->waitingPCB);
    }

    // If the mutex hasn't been saved and its reference set to NULL, too bad.
    if (conditional->mutex) {
        // destroy the mutex
    }

    free(conditional);
}

void Condition_wait(Conditional_p conditional, Mutex_p mutex, PCB_p pcb) {
    // Make sure the requesting PCB doesn't already have a lock on the mutex.
    Mutex_unlock(mutex, pcb);

    conditional->mutex = mutex;
    conditional->waitingPCB = pcb;
}

PCB_p Condition_signal(Conditional_p conditional, PCB_p pcb) {
    PCB_p returningPCB = conditional->waitingPCB;
    if (returningPCB != NULL) {
        conditional->waitingPCB = NULL;
        conditional->mutex = NULL;
    }
    return returningPCB;
}


//Force unlocks and replaces controller with next in queue. Dangerous
void Mutex_next_Controller(Mutex_p mut) {
	int error = 0;
	mut->key = (PCB_p)FIFOq_dequeue(mut->wait, &error);
	mut->locked = FALSE;
}

//Unlocks safely and replaces controller with next in queue.
//Returns true if successful, false otherwise.
int Mutex_next_Controller_s(Mutex_p mut, PCB_p pcb) {
	int error = 0;
	if (mut->key == pcb) {
		//Mutex_unlock(mut, pcb);
		mut->locked = FALSE;
		mut->key = (PCB_p)FIFOq_dequeue(mut->wait, &error);
		return TRUE;
	}
	return FALSE;
}

int Mutex_Is_Locked(Mutex_p mut) {
	if (mut->locked)
		return TRUE;
	return FALSE;
}
