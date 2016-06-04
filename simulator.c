#include "errors.h"
#include "PCB.h"
#include "FIFOq.h"
#include "OS.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAXLOOPS 100000     // Number of CPU cycles to simulate.

int main() {
	srand(time(0));
	OS_initialize();

	return 0;
}
