#include "errors.h"
#include "PCB.h"
#include "FIFOq.h"
#include "OS.h"

#include <stdlib.h>
#include <stdio.h>

void initialize() {
	int* error = 0;

	cswitch_no = 0;
	PC = 0;
	processes = 0;

	created_PCBs = FIFOq_construct();
	ready_PCBs = FIFOq_construct();
	terminated_PCBs = FIFOq_construct();

	idle_pcb = PCB_construct();
	PCB_init(idle_pcb, error);

	idle_pcb->state = running;
	idle_pcb->pc = PC;
	idle_pcb->pid = 0xFFFFFFFF;

	current_pcb = idle_pcb;
}

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
		
		FIFOq_enqueue(created_PCBs, new_process, error);
	}
}

void perform_ISR() {
	current_pcb->state = interrupted;
	current_pcb->pc = PC;
	scheduler(timer_interrupt);
	PC = SysStack;
}

void scheduler(enum Interrupt interrupt) {
	int* error = 0;

	int i;
	PCB_p dequeued;
	while (created_PCBs->size != 0) {
		dequeued = (PCB_p)FIFOq_dequeue(created_PCBs, error); //dangerous
		dequeued->state = running;
		FIFOq_enqueue(ready_PCBs, dequeued, error);
	}
	free(dequeued);

	switch (interrupt) {

	}

}

void dispatcher() {

}