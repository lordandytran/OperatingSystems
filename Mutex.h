#ifndef Mutex_H
#define Mutex_H

#include "PCB.h"
#include "FIFOq.h"

#define TRUE 1
#define FALSE 0

typedef struct mutex_t * Mutex_p;
typedef struct conditional_t * Conditional_p;

struct mutex_t {

	unsigned long ID; //ID of mutex
	struct pcb_t* key; //Which PCB has control
	int locked; //TRUE if locked. FALSE if not
	FIFOq_p wait; //Queue of PCB's that want control
	int destroyed;  // True if the shared resource is still allocated (in case a process terminates).

};

struct conditional_t {
    unsigned long ID;
    struct pcb_t* waitingPCB;
    Mutex_p mutex;  // The mutex the current PCB is waiting for.
};

//prototypes
Mutex_p Mutex_constructor();
int Mutex_lock(Mutex_p, PCB_p);
int Mutex_unlock(Mutex_p, PCB_p);
int Mutex_trylock(Mutex_p, PCB_p);
void Mutex_remove(Mutex_p mut, PCB_p pcb);

Conditional_p Conditional_constructor();
void Conditional_destructor(Conditional_p);
void Condition_wait(Conditional_p, Mutex_p, PCB_p);
PCB_p Condition_signal(Conditional_p conditional, PCB_p pcb);

void Mutex_next_Controller(Mutex_p);
int Mutex_next_Controller_s(Mutex_p, PCB_p);
int Mutex_Is_Locked(Mutex_p);

#endif
