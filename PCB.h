#ifndef PCB_H
#define PCB_H

#include <time.h>

#define IO_TRAP_QUANTITY 4
#define MUTEX_PC_QUANTITY 4
#define MAX_PC_VAL 5000
#define STARVATION_THRESHOLD 15

enum state_type { new, ready, running, interrupted, waiting, terminated };
enum pcb_type { io, compute, producer, consumer, resource_user, idle };

typedef struct pcb {
	unsigned long PID;
	unsigned short priority;    // Note: Uninitialized
	unsigned short priority_boost;
	unsigned short starvation_count;

	enum pcb_type type;         // Note: Uninitialized
	enum state_type state;
	unsigned long PC;
	unsigned long maxPC;        // Note: Uninitialized
	time_t creation;
	time_t termination;
	unsigned int terminate;     // Note: Uninitialized
	unsigned int term_count;
	unsigned long io_1_traps[IO_TRAP_QUANTITY];     // Note: Uninitialized
	unsigned long io_2_traps[IO_TRAP_QUANTITY];     // Note: Uninitialized

	unsigned int pair_id; // Producer/consumer pair ID
    int shared_resource;
	unsigned int lock_pcs[MUTEX_PC_QUANTITY]; // Steps where this PID calls a lock
	unsigned int unlock_pcs[MUTEX_PC_QUANTITY]; // Steps where this PID calls an unlock
} PCB;
typedef PCB * PCB_p;


PCB_p PCB_construct(void);
void PCB_destruct(PCB_p);
int PCB_init(PCB_p, int*);
char* PCB_state(PCB_p, int*);
char* PCB_type(PCB_p pcb, int* error);
char* PCB_toString(PCB_p, int*);
char* PCB_toStringDetailed(PCB_p pcb, int* error);
char* toStringIOArray(PCB_p pcb, int* error, int ioNum);
char* toStringLockUnlockArray(PCB_p pcb, int* error, int selection);

#endif
