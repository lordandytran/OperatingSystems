#ifndef OS_H
#define OS_H

enum Interrupt { timer_interrupt, IO_interrupt, IO_completion };

unsigned short cswitch_no;
unsigned long PC;
unsigned long SysStack;
int processes;
PCB_p current_pcb;
PCB_p idle_pcb;
FIFOq_p created_PCBs;
FIFOq_p ready_PCBs;
FIFOq_p terminated_PCBs;

//prototypes
void initialize();
void os_loop();
void generate_processes();
void perform_ISR();
void scheduler(enum Interrupt);
void dispatcher();

#endif