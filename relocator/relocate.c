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
#define STARTINGY 130
#define STARTINGZ 60

int relocate (int mid_point, int y, int z){
	servo_command1(BASE_CH,mid_point,3000);
	coord2pulse(STARTINGY,STARTINGZ,0,3000);
	servo_command1(CLAW_CH,1,3000);
	
		
	if (mid_point >= 1500 ){
		
		coord2pulse(STARTINGY,z,0,3000); //
		setvalue(MAGN, 1);	
		coord2pulse(STARTINGY,z,-90,3000);
		
		coord2pulse(y,z,-90,3000);
		coord2pulse(y,z,-90,3000);
		coord2pulse(y-100,z+110,-90,3000);
		//coord2pulse(130,195,-90,2000);

	 	servo_command1(BASE_CH,2500,3000);

		coord2pulse(120,20,-90,3000);
		setvalue(MAGN, 0);
		coord2pulse(120,150,-90,3000); 
		servo_command1(BASE_CH,mid_point,3000);
		coord2pulse(STARTINGY,STARTINGZ,0,3000);

	}
	
	
	if (mid_point < 1500 ){
		coord2pulse(STARTINGY,z,0,3000); //
		setvalue(MAGN, 1);	
		coord2pulse(STARTINGY,z,-90,3000);
		
		coord2pulse(y,z,-90,3000);
		coord2pulse(y,z,-90,3000);
		coord2pulse(y-100,z+110,-90,3000);
		//coord2pulse(130,195,-90,2000);

	 	servo_command1(BASE_CH,200,3000);

		coord2pulse(120,20,-90,3000);
		setvalue(MAGN, 0);
		coord2pulse(120,150,-90,3000); 
		servo_command1(BASE_CH,mid_point,3000);
		coord2pulse(STARTINGY,STARTINGZ,0,3000);

	}
	
	return 1;
}


int main(void){
	
	setup();
	setvalue(MAGN, 0);	
	//setvalue(MAGN, 1);
	int mid_point = 1100;
	int y = 222;
	int z = 90;
	relocate(mid_point,y,z);
	
	return 1;
	
		
}
