#include "errors.h"
#include "PCB.h"
#include "FIFOq.h"
#include "OS.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAXLOOPS 100 //FRUITLOOPS


int main() {

	//seeeed
	time_t t;
	srand((unsigned)time(&t));

	int* error = 0;

	cswitch_no = 0;
	PC = 0;
	processes = 0;

	ready_PCBs = FIFOq_construct();
	terminated_PCBs = FIFOq_construct();

	idle_pcb = PCB_construct();
	PCB_init(idle_pcb, error);

	idle_pcb->state = running;
	idle_pcb->pc = PC;
	idle_pcb->pid = 0xFFFFFFFF;

	current_pcb = idle_pcb;

	int i;
	for (i = 0; i < MAXLOOPS; i++) {
		os_loop();
	}

	return 0;
}
