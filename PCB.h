#ifndef PCB_H
#define PCB_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 30

#define FAILURE 0
#define SUCCESS 1

#define PCBNULLERROR 2557
#define STRNULLERROR 2579

enum state_type { new_, ready, running, interrupted, waiting, halted };

typedef struct pcb {

	unsigned long pid;
	enum state_type state;
	unsigned short priority;

	unsigned long pc;
} PCB;
typedef PCB * PCB_p;


PCB_p PCB_construct(void);
void PCB_destruct(PCB_p);
int PCB_init(PCB_p, int*);
int PCB_set_pid(PCB_p, unsigned long, int*);
unsigned long PCB_get_pid(PCB_p, int*);
char * PCB_toString(PCB_p, int*);

#endif