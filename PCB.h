#ifndef PCB_H
#define PCB_H

#include "Mutex.h"
#include <time.h>

#define MIN_PRIORITY 1
#define MAX_PRIORITY 30
#define IO_TRAP_QUANTITY 4
#define MAX_PC_VAL 5000
#define MAX_TERMINATION_COUNT

enum state_type { new, ready, running, interrupted, waiting, terminated };
enum pcb_type { io, filler, producer, consumer, resource_user };

typedef struct pcb {

	unsigned long pid;
	enum state_type state;
	enum pcb_type type;
	unsigned short priority;
	unsigned long pc;
	unsigned long maxPC;
	time_t creation;
	time_t termination;
	unsigned int terminate;
	unsigned int term_count;
	unsigned long io_1_traps[IO_TRAP_QUANTITY];
	unsigned long io_2_traps[IO_TRAP_QUANTITY];
	Mutex_p mutex_point;


} PCB;
typedef PCB * PCB_p;


PCB_p PCB_construct(void);
void PCB_destruct(PCB_p);
int PCB_init(PCB_p, int*);
char* PCB_state(PCB_p);
//char* PCB_toString(PCB_p, int*);

#endif
