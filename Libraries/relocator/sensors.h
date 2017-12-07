#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

//MULTI-THREAD/ERROR HANDLING
#include <signal.h>
#include <pthread.h>
 
#define TRIG 18
#define ECHOL 23
#define ECHOR 24
#define MAGN 17
 
void setup() {
	export(TRIG);
	export(ECHOL);
	export(ECHOR);
	export(MAGN);

	setdirection(MAGN, "out");
	setdirection(TRIG, "out");
	setdirection(ECHOR, "in");
	setdirection(ECHOL, "in");
	setvalue(TRIG, 0);
	setvalue(MAGN, 0);
	sleep(0.5);
	printf("Setup Complete\n");
}

void *LgetCMulti(void *temp_ping_ptr){
	//signal handling for thread cancling and void pointer setup
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	double *temp_ping = (double *)temp_ping_ptr;
	
	
	struct timeval start, stop;

	setvalue(TRIG, 1);
	usleep(10);
	setvalue(TRIG,0);

	////Wait for Echo to go high then begin timing
	while(getvalue(ECHOL) == 0){}
	gettimeofday(&start, NULL);


	////Wait until Echo goes low to stop timing
	while(getvalue(ECHOL) == 1){}
	gettimeofday(&stop, NULL);
	
	double LtravelTime = (double)( ((stop.tv_sec-start.tv_sec)*1000000ULL) + (stop.tv_usec-start.tv_usec) );


	double Ldistance = (LtravelTime) * 0.01715;
	Ldistance = Ldistance - .31; //error compensation
	
	
	(*temp_ping) = Ldistance;
	return NULL;
}

double LgetCM(int pingcount){
	
	double current_ping, temp_ping;
	pthread_t thread1;
	int timer_current;
	struct timeval start, stop;
	int i;
	
	for(i=0;i<pingcount;i++){
		temp_ping = 0;
		current_ping = 10000;
		while(temp_ping == 0){
			//Create thread
			if(pthread_create(&thread1,NULL,LgetCMulti,&temp_ping)){
				fprintf(stderr, "Error creating thread\n");
				return 0;
			}
			
			//get start of timer (us)
			gettimeofday(&start, NULL);
			timer_current = 0;
			
			//wait until ping returns from other thread or 1 second passes
			while(temp_ping==0 && timer_current<1000000){
				gettimeofday(&stop, NULL);
				timer_current = (int)((stop.tv_sec-start.tv_sec)*1000000ULL+(stop.tv_usec-start.tv_usec));
			}
			
			//ping failed to return so keep looping in while loop
			if(temp_ping==0){
				printf("Left ping failed. Retriggering...\n");
				if(pthread_cancel(thread1)){
					fprintf(stderr, "Error canceling thread\n");
					return 0;
				}
			}
			//Ping succeeded, exit while loop and report current_ping
			else{
				if(pthread_join(thread1,NULL)){
					fprintf(stderr, "Error joining thread\n");
					return 0;
				}
				
				//Only hang on to lowest temp ping from for loop (early filtering for extremely high values)
				if(temp_ping < current_ping){
					current_ping = temp_ping;
				}
				
			}
		}
	}
		
	return current_ping;
}


double LgetCM_avg(int pingcount){
	
	double current_ping, temp_ping;
	pthread_t thread1;
	int timer_current;
	struct timeval start, stop;
	int i;
	
	current_ping = 0;
	for(i=0;i<pingcount;i++){
		temp_ping = 0;
		while(temp_ping == 0){
			//Create thread
			if(pthread_create(&thread1,NULL,LgetCMulti,&temp_ping)){
				fprintf(stderr, "Error creating thread\n");
				return 0;
			}
			
			//get start of timer (us)
			gettimeofday(&start, NULL);
			timer_current = 0;
			
			//wait until ping returns from other thread or 1 second passes
			while(temp_ping==0 && timer_current<1000000){
				gettimeofday(&stop, NULL);
				timer_current = (int)((stop.tv_sec-start.tv_sec)*1000000ULL+(stop.tv_usec-start.tv_usec));
			}
			
			//ping failed to return so keep looping in while loop
			if(temp_ping==0){
				printf("Left ping failed. Retriggering...\n");
				if(pthread_cancel(thread1)){
					fprintf(stderr, "Error canceling thread\n");
					return 0;
				}
			}
			//Ping succeeded, exit while loop and report current_ping
			else{
				if(pthread_join(thread1,NULL)){
					fprintf(stderr, "Error joining thread\n");
					return 0;
				}
								
				current_ping += temp_ping;
			}
		}
	}
	
	current_ping = current_ping / pingcount;
	
	return current_ping;
}

void *RgetCMulti(void *temp_ping_ptr){
	//signal handling for thread cancling and void pointer setup
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	double *temp_ping = (double *)temp_ping_ptr;
	
	
	struct timeval start, stop;

	setvalue(TRIG, 1);
	usleep(10);
	setvalue(TRIG,0);

	////Wait for Echo to go high then begin timing
	while(getvalue(ECHOR) == 0){}
	gettimeofday(&start, NULL);


	////Wait until Echo goes low to stop timing
	while(getvalue(ECHOR) == 1){}
	gettimeofday(&stop, NULL);
	
	double RtravelTime = (double)( ((stop.tv_sec-start.tv_sec)*1000000ULL) + (stop.tv_usec-start.tv_usec) );


	double Rdistance = (RtravelTime) * 0.01715;
	Rdistance = Rdistance - .31; //error compensation
	
	
	(*temp_ping) = Rdistance;
	return NULL;
}

double RgetCM(int pingcount){

	double current_ping, temp_ping;
	pthread_t thread1;
	int timer_current;
	struct timeval start, stop;
	int i;
	
	for(i=0;i<pingcount;i++){
		temp_ping = 0;
		current_ping = 10000;
		while(temp_ping == 0){
			//Create thread to get actual ping
			if(pthread_create(&thread1,NULL,RgetCMulti,&temp_ping)){
				fprintf(stderr, "Error creating thread\n");
				return 0;
			}
			
			//get start of timer (us)
			gettimeofday(&start, NULL);
			timer_current = 0;
			
			//wait until ping returns from other thread or 1 second passes
			while(temp_ping==0 && timer_current<1000000){
				gettimeofday(&stop, NULL);
				timer_current = (int)((stop.tv_sec-start.tv_sec)*1000000ULL+(stop.tv_usec-start.tv_usec));
			}
			
			//ping failed to return so keep looping in while loop
			if(temp_ping==0){
				printf("Right ping failed. Retriggering...\n");
				if(pthread_cancel(thread1)){
					fprintf(stderr, "Error canceling thread\n");
					return 0;
				}
			}
			//Ping succeeded, exit while loop and report current_ping
			else{
				if(pthread_join(thread1,NULL)){
					fprintf(stderr, "Error joining thread\n");
					return 0;
				}
				
				//Only hang on to lowest temp ping from for loop (early filtering for extremely high values)
				if(temp_ping < current_ping){
					current_ping = temp_ping;
				}
				
			}
		}
	}
	
	
	return current_ping;
}

double RgetCM_avg(int pingcount){
	
	double current_ping, temp_ping;
	pthread_t thread1;
	int timer_current;
	struct timeval start, stop;
	int i;
	
	current_ping = 0;
	for(i=0;i<pingcount;i++){
		temp_ping = 0;
		while(temp_ping == 0){
			//Create thread to get actual ping
			if(pthread_create(&thread1,NULL,RgetCMulti,&temp_ping)){
				fprintf(stderr, "Error creating thread\n");
				return 0;
			}
			
			//get start of timer (us)
			gettimeofday(&start, NULL);
			timer_current = 0;
			
			//wait until ping returns from other thread or 1 second passes
			while(temp_ping==0 && timer_current<1000000){
				gettimeofday(&stop, NULL);
				timer_current = (int)((stop.tv_sec-start.tv_sec)*1000000ULL+(stop.tv_usec-start.tv_usec));
			}
			
			//ping failed to return so keep looping in while loop
			if(temp_ping==0){
				printf("Right ping failed. Retriggering...\n");
				if(pthread_cancel(thread1)){
					fprintf(stderr, "Error canceling thread\n");
					return 0;
				}
			}
			//Ping succeeded, exit while loop and report current_ping
			else{
				if(pthread_join(thread1,NULL)){
					fprintf(stderr, "Error joining thread\n");
					return 0;
				}
								
				current_ping += temp_ping;
			}
		}
	}
	
	current_ping = current_ping / pingcount;
	
	return current_ping;
}