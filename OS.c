#include "errors.h"
#include "PCB.h"
#include "FIFOq.h"
#include "OS.h"

#include <stdlib.h>
#include <stdio.h>


void os_loop() {
	generate_processes();

	PC += ((rand() % 1000) + 3000);
	current_pcb->pc = PC;

	SysStack = PC;

	perform_ISR();
	
}

void generate_processes() {
	
	int* error = 0;

	int quantity = rand() % 5;
	int i;
	for (i = 0; i < quantity; i++) {
		if (processes >= 30)
			return;

		PCB_p new_process = PCB_construct();
		PCB_init(new_process, error);
		new_process->pid = (unsigned long)rand(); //This needs to change
		new_process->state = new_;
		processes++;
		
		FIFOq_enqueue(ready_PCBs, new_process, error);
	}
}

void perform_ISR() {
	current_pcb->state = interrupted;
	current_pcb->pc = PC;
	scheduler(timer_interrupt);
	PC = SysStack;
}

void scheduler(enum Interrupt interrupt) {

}

void dispatcher() {

}