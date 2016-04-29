#ifndef PCB_H
#define PCB_H

//#include <time.h>

#define MIN_PRIORITY 1
#define MAX_PRIORITY 30

//#define MAX_PC_VAL 2345
//#define IO_TRAPS 4

enum state_type { new_, ready, running, interrupted, waiting, halted };

typedef struct pcb {

	unsigned long pid;
	enum state_type state;
	unsigned short priority;
	unsigned long pc;
	//unsigned long sw;
	//unsigned int max_pc;
	//time_t creation;
	//time_t termination;
	//unsigned int terminate;
	//unsigned int term_count;
	//unsigned int io_1_traps[IO_TRAPS];
	//unsigned int io_2_traps[IO_TRAPS];

} PCB;
typedef PCB * PCB_p;


PCB_p PCB_construct(void);
void PCB_destruct(PCB_p);
int PCB_init(PCB_p, int*);
int PCB_set_pid(PCB_p, unsigned long, int*);
unsigned long PCB_get_pid(PCB_p, int*);
char * PCB_toString(PCB_p, int*);

#endif
