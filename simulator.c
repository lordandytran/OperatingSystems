#include "OS.h"

#define MAXLOOPS 200     // Number of CPU cycles to simulate.

int main() {
	srand((unsigned int) (time(0)));
	OS_initialize();

    for (int i = 0; i < MAXLOOPS; i++) {
        OS_loop();
    }

	return 0;
}
