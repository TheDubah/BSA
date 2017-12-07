#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


int main(void) {
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	long duration = 0;
	long start = tp.nsec / 1000;
	while(duration < 5000000){
		clock_gettime(CLOCK_REALTIME, &tp);
	};
	
}