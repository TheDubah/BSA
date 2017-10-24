//STANDARD LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "sensors.h"

//MULTI-THREAD/ERROR HANDLING
#include <signal.h>
#include <pthread.h>

//ORIGINAL LIBRARIES
//#include "sensors.h" //-> Used for obtaining distances from left and right receivers
#include "ssc32usb.h" //-> Used for arm movement
//#include "neuralnet.h" //-> Used for Neural Network functionality


int relocate (int mid_point){
	
	servo_command1(BASE_CH,mid_point,2000);
		
	if (mid_point >= 1500 ){
		
		coord2pulse(130,25,0,3000);
		
		coord2pulse(220,65,-90,3000);
		
		coord2pulse(220,150,-90,3000);
		//coord2pulse(130,195,-90,2000);

		servo_command1(BASE_CH,2500,3000);

		coord2pulse(220,85,-90,3000);
		coord2pulse(220,150,-90,3000);


		//servo_command1(BASE_CH,1500,2000);

	}
	
	
	if (mid_point < 1500 ){
		coord2pulse(130,35,-90,2000);

		//servo_command1(BASE_CH,1500,2000);

		coord2pulse(220,65,-90,2000);
		coord2pulse(220,120,-90,2000);
		//coord2pulse(130,195,-90,2000);

		servo_command1(BASE_CH,2500,2000);

		coord2pulse(220,65,-90,2000);
		coord2pulse(220,75,-90,2000);


		servo_command1(BASE_CH,1500,2000);

	}
	
	return 1;
}


int main(void){
	
	setup();
	setvalue(MAGN, 1);
	setvalue(MAGN, 1);
	while(1);
	//int mid_point = 1500;
	//relocate(mid_point);
	return 1;
	
		
}
