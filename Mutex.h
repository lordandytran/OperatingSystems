#ifndef Mutex_H
#define Mutex_H

#include "FIFOq.h"
#include "PCB.h"

#define TRUE 1
#define FALSE 0

typedef struct mutex_t {

	unsigned long ID; //ID of mutex
	PCB_p key; //Which PCB has control
	int locked; //TRUE if locked. FALSE if not
	FIFOq_p wait; //Queue of PCB's that want control

} Mutex;
typedef Mutex * Mutex_p;

//prototypes
Mutex_p Mutex_constructor();
int Mutex_lock(Mutex_p, PCB_p);
int Mutex_unlock(Mutex_p, PCB_p);
int Mutex_trylock(Mutex_p, PCB_p);
void Mutex_next_Controller(Mutex_p);
int Mutex_next_Controller_s(Mutex_p, PCB_p);
int Mutex_Is_Locked(Mutex_p);

#endif
