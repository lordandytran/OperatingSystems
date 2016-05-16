#ifndef PCB_H
#define PCB_H

#define MIN_PRIORITY 1
#define MAX_PRIORITY 30

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
char* PCB_state(PCB_p);
//char* PCB_toString(PCB_p, int*);

#endif
