#ifndef Mutex_H
#define Mutex_H

#include "PCB.h"
#include "FIFOq.h"

#define TRUE 1
#define FALSE 0

typedef struct mutex_t {

	unsigned long ID; //ID of mutex
	PCB_p key; //Which PCB has control
	int locked; //TRUE if locked. FALSE if not
	FIFOq_p wait; //Queue of PCB's that want control
	int destroyed;  // True if the shared resource is still allocated (in case a process terminates).

} Mutex;
typedef Mutex * Mutex_p;

typedef struct conditional_t {
    unsigned long ID;
    PCB_p waitingPCB;
    Mutex_p mutex;  // The mutex the current PCB is waiting for.
} Conditional;
typedef Conditional * Conditional_p;

//prototypes
Mutex_p Mutex_constructor();
int Mutex_lock(Mutex_p, PCB_p);
int Mutex_unlock(Mutex_p, PCB_p);
int Mutex_trylock(Mutex_p, PCB_p);

Conditional_p Conditional_constructor();
void Conditional_destructor(Conditional_p);
void Condition_wait(Conditional_p, Mutex_p, PCB_p);
struct pcb* Condition_signal(Conditional_p, PCB_p);

void Mutex_next_Controller(Mutex_p);
int Mutex_next_Controller_s(Mutex_p, PCB_p);
int Mutex_Is_Locked(Mutex_p);

#endif
