#include "errors.h"
#include "PCB.h"
#include "FIFOq.h"
#include "OS.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAXLOOPS 100000 //FROOTLOOPS


int main() {
	srand(time(0));

	initialize();

	int i;
	for (i = 0; i < MAXLOOPS; i++) {
		os_loop();
	}

	return 0;
}
