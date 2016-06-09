/*
 * TCSS 422 - Spring 2016
 * Final Project
 * Team 2:
 * Derek Moore
 * Ashton Ohms
 * Anh Tran
 * Vitaliy Radchishin
*/

#include "OS.h"

#define MAXLOOPS 2000     // Number of CPU cycles to simulate.

int main() {
    srand((unsigned int) (time(0)));
    OS_initialize();
    deadlock = FALSE;

    for (int i = 0; i < MAXLOOPS; i++) {
        OS_loop();
    }
    printf("Simulation complete. %d processes run, %d remaining.\n", processes_created, (processes_created - processes_terminated));
    return 0;
}
