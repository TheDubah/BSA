#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include <unistd.h>
#include <signal.h>
#include <pthread.h>
 
#define TRIG 23
#define ECHO1 24
#define ECHO2 22
 
/* void setup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}
 
int getCM() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(20);
        digitalWrite(TRIG, LOW);
 
        //Wait for echo start
        while(digitalRead(ECHO) == LOW);
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
 
        return distance;
} */

void setup(void) {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHO1, INPUT);
		pinMode(ECHO2, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}

void trigger(void){
	
	//Send trig pulse
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIG, LOW);
	
}

/*
long * ping(void){
	
 	pid_t child1, child2;
	static long measurements[2] = {0,0};
	int ping_failed=1;
	long travel1 = 0;
	long travel2 = 0;
	
	long timerStart;
	long timer;
	long startTime1;
	long startTime2;
	long endTime1;
	long endTime2;
	
	
	while(ping_failed==1){
		
		child1 = fork(); //create child 1
		if (child1 < 0){exit(1);} //fork1 failed
		
		if (child1 == 0){
			//CHILD1 CODE
			printf("CHILD1 \n");
			while(digitalRead(ECHO1) == LOW);
			
			startTime1 = micros();
			while(digitalRead(ECHO1) == HIGH);
			endTime1 = micros();
			
			travel1 = endTime1 - startTime1;
			
			return travel1;
			
			_exit(3);
		}
		else{
			child2=fork(); //create child 2
			if (child2 < 0){exit(1);} //fork2 failed
			
			if (child2 == 0){
				//CHILD2 CODE
				//printf("CHILD 2 1\n");
				while(digitalRead(ECHO2) == LOW);
			
				startTime2 = micros();
				while(digitalRead(ECHO2) == HIGH);
				endTime2 = micros();
				
				travel2 = endTime2 - startTime2;
				
				return travel2;
				
				_exit(3);
			}
			
			else{
				//PARENT CODE
				trigger();
				
				while(wait(&travel1) & wait(&travel2)){
					
				}
				
			}
		}
		
		
 		if (child1!=0 && child2==0){
			printf("Parent here\n");
		}
		else if (child1==0 && child2!=0){
			printf("CHILD1 here\n");
		}
		else if (child1==0 && child2==0){
			printf("CHILD2 here\n");
		}
		
		
 		sem = 0;
		
		if (child1!=0 && child2==0){
			trigger(); //parent triggers
		}
		
		if (child1==0 && child2!=0){ //child1
			printf("CHILD1 \n");
			while(digitalRead(ECHO1) == LOW);
			
			startTime1 = micros();
			while(digitalRead(ECHO1) == HIGH);
			endTime1 = micros();
			
			travel1 = endTime1 - startTime1;
			
			sem++;
			
			_exit(3);
		}
			
		else if (child1==0 && child2==0){ //child2
			//printf("CHILD 2 1\n");
			while(digitalRead(ECHO2) == LOW);
		
			startTime2 = micros();
			while(digitalRead(ECHO2) == HIGH);
			endTime2 = micros();
			
			travel2 = endTime2 - startTime2;
			
			sem++;
			
			_exit(3);
		}
		
		//start clock for retrigger
 		timerStart = millis();
		timer = 0;
		
		while(sem < 2 && timer < 5000){ //5 sec wait or untill readings recieved from both sensors
			timer = millis() - timerStart;
		}
		
		if (sem==2){
			ping_failed=0;
			measurements[0] = travel1 / 58; //measurements in cm
			measurements[1] = travel2 / 58;
		}
		else{
			printf("Ping failed. Retriggering...\n");
		}
		
		//kill child processes
		kill(child1, SIGKILL);
		kill(child2, SIGKILL); 
	
	}
	return measurements;
}*/


void *ping1(void *travel1_void_ptr){
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	long *travel1 = (long *)travel1_void_ptr;
	
	long startTime1, endTime1;
	
	while(digitalRead(ECHO1) == LOW);
	printf("PING 1\n");
	
	startTime1 = micros();
	while(digitalRead(ECHO1) == HIGH);
	endTime1 = micros();
	
	(*travel1) = endTime1 - startTime1;
	
	return NULL;
}

void *ping2(void *travel2_void_ptr){
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	long *travel2 = (long *)travel2_void_ptr;
	
	long startTime2, endTime2;
	
	while(digitalRead(ECHO2) == LOW);
	printf("PING 2\n");

	startTime2 = micros();
	while(digitalRead(ECHO2) == HIGH);
	endTime2 = micros();
	
	(*travel2) = endTime2 - startTime2;
	printf("PING2 = %ld\n",(*travel2));
	
	return NULL;
}

long * pinging(void){
	long travel1, travel2;
	//int sem;
	int ping_failed = 1;
	
	long timerStart;
	long timer;
	static long measurements[2] = {0,0};
	
	pthread_t thread1, thread2;
	
	
	while(ping_failed==1){
		//sem = 0;
		travel1 = -1;
		travel2 = -1;
		
		printf("beginning\n");
		
		//CREATE THREADS
		if(pthread_create(&thread1,NULL,ping1,&travel1)){
			fprintf(stderr, "Error creating thread\n");
			return NULL;
		}

		if(pthread_create(&thread2,NULL,ping2,&travel2)){
			fprintf(stderr, "Error creating thread\n");
			return NULL;
		}
		
		printf("threads created successfully\n");
		
		//Trigger from main thread
		trigger();
		printf("triggerred successfully\n");
		
		//start clock for retrigger
		timerStart = millis();
		timer = 0;
		
		while((travel1==-1 || travel2==-1) && timer < 5000){ //5 sec wait or untill readings recieved from both sensors
			timer = millis() - timerStart;
		}
		
		if (travel1!=-1 && travel2!=-1){
			ping_failed=0;
			measurements[0] = travel1 / 58; //measurements in cm
			measurements[1] = travel2 / 58;
			
			printf("Ping 1: %ld\n",measurements[0]);
			printf("Ping 2: %ld\n",measurements[1]);
			
			//(void) pthread_join(thread1,NULL);
			//(void) pthread_join(thread2,NULL);
			if(pthread_join(thread1,NULL)){
				//printf("Killing thread 1\n");
				fprintf(stderr, "Error joinging thread\n");
				return NULL;
			}
			
			if(pthread_join(thread2,NULL)){
				//printf("Killing thread 1\n");
				fprintf(stderr, "Error joinging thread\n");
				return NULL;
			}
		}
		else{
			printf("Ping failed. Retriggering...\n");
			if(pthread_cancel(thread1)){
				fprintf(stderr, "Error canceling thread\n");
				return NULL;
			}
			
			if(pthread_cancel(thread2)){
				fprintf(stderr, "Error canceling thread\n");
				return NULL;
			}
		}
		
		//KILL THREADS

/* 		if(pthread_join(thread1,NULL)){
			//printf("Killing thread 1\n");
			fprintf(stderr, "Error joinging thread\n");
			return NULL;
		}
		
		if(pthread_join(thread2,NULL)){
			//printf("Killing thread 1\n");
			fprintf(stderr, "Error joinging thread\n");
			return NULL;
		} */
		
		printf("threads killed successfully\n");
		
	}
	return measurements;
}


int main(void) {
        setup();
		
        long *pings;
		
		pings = pinging();
		
		printf("Ping1: %li \n", pings[0]);
		printf("Ping2: %li \n", pings[1]);
		
		
/*		pthread_t thread1, thread2;
		
		if(pthread_create(&thread1,NULL,print1,&x)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}

		if(pthread_create(&thread2,NULL,print2,&x)){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		
		while(x < 2);
		
		printf("x: %ld\n",x);
		
		if(pthread_join(thread1,NULL)){
			fprintf(stderr, "Error joingin thread\n");
			return 2;
		}
		
		if(pthread_join(thread2,NULL)){
			fprintf(stderr, "Error joingin thread\n");
			return 2;
		}*/
		
        return 0;
}