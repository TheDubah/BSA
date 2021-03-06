//STANDARD LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//MULTI-THREAD/ERROR HANDLING
#include <signal.h>
#include <pthread.h>

//ORIGINAL LIBRARIES
#include "sensors.h" //-> Used for obtaining distances from left and right receivers
#include "ssc32usb.h" //-> Used for arm movement
#include "neuralnet.h" //-> Used for Neural Network functionality

#define PINGCOUNT 3
#define BUFFERSIZE 6
#define BORDER 200
#define STARTPULSE 800
#define STARTINGY 130
#define STARTINGZ 25

//Neural Network Macros
#define SIDEINPUT 20
#define SIDEHIDDEN 15

#define SHAPEINPUT 9
#define SHAPEHIDDEN 6
#define SHAPEOUTPUT 3

#define SEED 117 


//main program/operation flow
int main(void){
	
	//For sensors setup
	setup();
	
	//Starting y-z position (can be changed)
	int y = STARTINGY;
	int z = STARTINGZ;
	coord2pulse(y,z,0,4000);
	//coord2pulse(y,300,0,4000);
	
	servo_command1(1, 2200, 2000); //Gripper open
	printf("Closed now \n");
	
	coord2pulse(370,25,0,4000); //Go forward
	servo_command1(1, 1200, 3500); //Gripper closed
	coord2pulse(y,250,0,4000); // go up
	

	servo_command1(9, 2200, 2000); //Move to the right
	printf("Closed now \n");
	coord2pulse(y,z,0,4000); // Default position
	
	servo_command1(1, 2200, 2000); //Gripper open
	servo_command1(9, 1500, 3500);

	
	//coord2pulse(y,25,0,4000);
	//coord2pulse(y,375,0,4000);
	coord2pulse(y,250,0,4000);
	coord2pulse(y,25,0,4000);
	coord2pulse(y,300,0,4000);
	
	
	//Variables used later, names give some idea but read further
	int i,j;
	double left_ping, right_ping;
	double right_buffer[BUFFERSIZE], left_buffer[BUFFERSIZE];
	int left_valid, right_valid;
	int object_started = 0;
	int object_start, object_end;
	
	//INITIAL START POSITION FOR THE BASE (can be adjusted with MACRO above)
	///servo_command1(BASE_CH,STARTPULSE,4000);
	
	double objectStart_xyz[3];
	double objectEnd_xyz[3];
	double *curr_xyz;
	curr_xyz = coord2pulse_XYZ(STARTPULSE+700,y,z,0,4000);
	
	double scan1[500];
	double scan2[500];
	double scan3[500];
	double scan1R[500];
	double scan2R[500];
	double scan3R[500];
	
	int a = 0; //index for saving into scan1 array
	int b,c;
	
	/*The major for-loop for the stage1 scan that goes ~180 deg to scan for objects*/
	//for(i=0;i<700;i=i+1){
	for(i=700;i>0;i=i-1){
		printf("i=%d\n",i);
		
		//STARTPULSE is the constant starting pulse for the base servo. Can be adjusted with MACRO above.
		///servo_command1(BASE_CH,(i+STARTPULSE),1);
		curr_xyz = coord2pulse_XYZ( (STARTPULSE+i),y,z,0,1 );
		
		//used as counters when going back through buffer to count how many pings are proper object pings. left_valid for Left reciever and right_valid is for Right sensor.
		left_valid = 0;
		right_valid = 0;
		
		/***************************************************/
		//left sensor pings with LgetCM(). PINGCOUNT is the number of times to ping and return the lowest ping distance from the count. can be adjusted with MACRO above.
		left_ping = LgetCM(PINGCOUNT) * 10;
		
		//This for-loop simply iterates through the buffer, pushing elements down one by one, and adding the newest ping to the the first [0] spot in buffer array.
		//	It also counts the number of valid object pings in the buffer. Valid pings are determined by seeing if they are smaller than the BORDER (aka an object and not empty space,
		//	border or table.
		for(j=BUFFERSIZE-1;j>0;j--){
			left_buffer[j] = left_buffer[j-1];
			if(left_buffer[j] < BORDER){
				left_valid++;
			}
		}
		//Here is where the 0th spot in buffer is assigned the new ping value
		left_buffer[0] = left_ping;
		
		//Print to see what we get on the new ping.
		printf("Left Echo: %lf\n",left_ping);
		
		
		/*ENTIRE PROCESS IS REPEATED FOR RIGHT PING*/
		right_ping = RgetCM(PINGCOUNT) * 10;

		for(j=BUFFERSIZE-1;j>0;j--){
			right_buffer[j] = right_buffer[j-1];
			if(right_buffer[j] < BORDER){
				right_valid++;
			}
		}
		right_buffer[0] = right_ping;
		
		printf("Right Echo: %lf\n",right_ping);
		
		/***************************************************/
		
		if(object_started == 1){
			/*right_ping = RgetCM(PINGCOUNT);
			if(left_ping < right_ping){
				scan1[a] = left_ping;
			}
			else{
				scan1[a] = right_ping;
			}*/
			scan1[a] = left_ping;
			scan1R[a] = right_ping;
			printf("%d: %lf\n",a,scan1[a]);
			a++;
		}
		
		//These two if statements are used to look back at array in this iteration of loop to see if we have enough valid pings (4) to safely
		// mark an edge of an object. If object_started is 0 (initial state) then it is marked with 1 and the object_start base pulse is saved.
		//if(left_valid>=4 && object_started==0){
		if(left_valid>=4 && right_valid>=4 && object_started==0){	
			object_started = 1;
			object_start = i-4;
			printf("------------------Object Start @ i=%d------------------\n",i-4);
			
			//Save the xyz coordinates of the starting edge to objectStart_xyz by copying.
			for(j=0;j<3;j++){
				objectStart_xyz[j] = curr_xyz[j];
			}
			
			
		}

		// If object_started is 1 and there are less than 4 valid pings, this means you are no longer looking at object and must now mark
		// object end. object_started is 0 and base pulse is saved in object_end.
		//if(left_valid<4 && object_started==1){
		if(left_valid<4 && right_valid<4 && object_started==1){
			object_started = 0;
			object_end = i-4;
			printf("------------------Object End @ i=%d------------------\n",i-4);
			
			//Save the xyz coordinates of the falling off edge to objectEnd_xyz by copying.
			for(j=0;j<3;j++){
				objectEnd_xyz[j] = curr_xyz[j];
			}
			
			int mid_point = ((object_end - object_start) / 2) + object_start + STARTPULSE;
			servo_command1(BASE_CH,mid_point,2000);
			
			/*double dist2obj = LgetCM(5);
			
			printf("Dist2obj: %lf",y+(dist2obj*10)-30);
			y = y+(dist2obj*10)-30;
			coord2pulse(y,z,0,2000);*/
			
			/*Get close to object within certain distance*/
			double y_ping = LgetCM(PINGCOUNT) * 10; //to get mm from cm ping
			double y_pingR = RgetCM(PINGCOUNT) * 10;
			while(y_ping > 60){
				y_ping = LgetCM(PINGCOUNT) * 10; //to get mm for cm
				printf("Y_PING: %lf\n",y_ping);
				y++;
				coord2pulse(y,z,0,1);
			}
			
			int flag = 0;
			y_ping = LgetCM(PINGCOUNT);
			
			b = 0;
			while((y_ping < BORDER) && (flag == 0)){
				y_ping = LgetCM(PINGCOUNT) * 10;
				/*y_pingR = RgetCM(PINGCOUNT) * 10;
				if(y_ping > y_pingR){
					y_ping = y_pingR;
				}*/
				
				scan2[b] = y_ping;
				
				y_pingR = RgetCM(PINGCOUNT) * 10;
				scan2R[b] = y_pingR;
				
				flag = coord2pulse(y,z++,0,1);
				b++;
			}
			
			double z_ping = 0;
			double z_pingR = 0;
			coord2pulse(y,z,-90,2000);
			
			c = 0;
			z += 20; //Testing higher height
			y += 60; //move to close in gap
			coord2pulse(y,z,-90,2000);
			while((z_ping < z) && (flag == 0)){
				z_ping = LgetCM(PINGCOUNT) * 10;
				/*z_pingR = RgetCM(PINGCOUNT) * 10;
				if(z_ping > z_pingR){
					z_ping = z_pingR;
				}*/
				scan3[c] = z_ping;
				
				z_pingR = RgetCM(PINGCOUNT) * 10;
				scan3R[c] = z_pingR;
				
				flag = coord2pulse((y++),z,-90,1);
				c++;
			}
			
			printf("----------------FLAG %d\n",flag);
			
			//Move the claw back (y-axis)
			y = STARTINGY;
			coord2pulse(y,z+30,-90,2000);
			//Move claw facing forward
			coord2pulse(y,z,0,2000);
			//Move the claw down (z-axis)
			z = STARTINGZ;
			coord2pulse(y,z,0,2000);
			curr_xyz = coord2pulse_XYZ( (STARTPULSE+i),y,z,0,1000 );
			
			for(i=0;i<a;i++){
				printf("scan1[%d]: %lf\n",i,scan1[i]);
			}
			
			for(i=0;i<a;i++){
				printf("scan1R[%d]: %lf\n",i,scan1R[i]);
			}
			
			for(i=0;i<b;i++){
				printf("scan2[%d]: %lf\n",i,scan2[i]);
			}
			
			for(i=0;i<b;i++){
				printf("scan2R[%d]: %lf\n",i,scan2R[i]);
			}
			
			for(i=0;i<c;i++){
				printf("scan3[%d]: %lf\n",i,scan3[i]);
			}
			
			for(i=0;i<c;i++){
				printf("scan3R[%d]: %lf\n",i,scan3R[i]);
			}
			
			double scan1_hold[SIDEINPUT];
			double scan2_hold[SIDEINPUT];
			double scan3_hold[SIDEINPUT];
			
			//scan 1
			int trueSize = a;
			int trim = 8;
			int inputNodes = SIDEINPUT;
			  
			int newSize = trueSize - trim;
			int step = newSize / inputNodes;
			int half = newSize / 2;
			int listStart = half - (step * (inputNodes/2)) + (trim/2);
			int listEnd = listStart + (step * (inputNodes-1));
			
			int k = 0;
			for(i=listStart;i<=listEnd;i=i+step){
				printf("scan1[%d]_20: %lf\n",i,scan1[i]);
				scan1_hold[k] = scan1[i];
				k++;
			}
			
			char g;
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			trueSize = b;
			
			newSize = trueSize - trim;
			step = newSize / inputNodes;
			half = newSize / 2;
			listStart = half - (step * (inputNodes/2)) + (trim/2);
			listEnd = listStart + (step * (inputNodes-1));
			
			k = 0;
			for(i=listStart;i<=listEnd;i=i+step){
				printf("scan2[%d]_20: %lf\n",i,scan2[i]);
				scan2_hold[k] = scan2[i];
				k++;
			}
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			k = 0;
			if(c > SIDEINPUT){
				trueSize = c;
				
				newSize = trueSize - trim;
				step = newSize / inputNodes;
				half = newSize / 2;
				listStart = half - (step * (inputNodes/2)) + (trim/2);
				listEnd = listStart + (step * (inputNodes-1));
				
				for(i=listStart;i<=listEnd;i=i+step){
					printf("scan3[%d]_20: %lf\n",i,scan3[i]);
					scan3_hold[k] = scan3[i];
					k++;
				}
			}
			else{
				for(i=0;i<c;i++){
					printf("scan3[%d]_20: %lf\n",i,scan3[i]);
					scan3_hold[i] = scan3[i];
				}
			}
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			
			double scan1_deriv[SIDEINPUT];
			double scan2_deriv[SIDEINPUT];
			double scan3_deriv[SIDEINPUT];
			for(i=0;i<SIDEINPUT-1;i++){
				scan1_deriv[i] = scan1_hold[i+1] - scan1_hold[i];
				printf("scan1_deriv[%d]: %lf\n",i,scan1_deriv[i]);
			}
			scan1_deriv[SIDEINPUT-1] = scan1_deriv[SIDEINPUT-2];
			printf("scan1_deriv[%d]: %lf\n",SIDEINPUT-1,scan1_deriv[SIDEINPUT-1]);
			printf("\n");
			
			for(i=0;i<SIDEINPUT-1;i++){
				scan2_deriv[i] = scan2_hold[i+1] - scan2_hold[i];
				printf("scan2_deriv[%d]: %lf\n",i,scan2_deriv[i]);
			}
			scan2_deriv[SIDEINPUT-1] = scan2_deriv[SIDEINPUT-2];
			printf("scan2_deriv[%d]: %lf\n",SIDEINPUT-1,scan2_deriv[SIDEINPUT-1]);
			printf("\n");
			
			for(i=0;i<SIDEINPUT-1;i++){
				scan3_deriv[i] = scan3_hold[i+1] - scan3_hold[i];
				printf("scan3_deriv[%d]: %lf\n",i,scan3_deriv[i]);
			}
			scan3_deriv[SIDEINPUT-1] = scan3_deriv[SIDEINPUT-2];
			printf("scan3_deriv[%d]: %lf\n",SIDEINPUT-1,scan2_deriv[SIDEINPUT-1]);
			printf("\n");


		}
		
	}
	
	//Print the xyz coordinates for the start and stop of object in scan1
	printf("--Object Start XYZ--\n");
	printf("X: %lf\n",objectStart_xyz[0]);
	printf("Y: %lf\n",objectStart_xyz[1]);
	printf("Z: %lf\n",objectStart_xyz[2]);
	
	printf("--Object End XYZ--\n");
	printf("X: %lf\n",objectEnd_xyz[0]);
	printf("Y: %lf\n",objectEnd_xyz[1]);
	printf("Z: %lf\n",objectEnd_xyz[2]);
	
	
	/*int mid_point = ((object_end - object_start) / 2) + object_start + STARTPULSE;
	servo_command1(BASE_CH,mid_point,2000);
	
	double dist2obj = LgetCM(5);
	
	printf("Dist2obj: %lf",y+(dist2obj*10)-50);
	
	coord2pulse(y+(dist2obj*10)-50,z,0,2000);
	
	double y_ping = LgetCM(3);
	while(y_ping < BORDER){
		y_ping = LgetCM(3);
		coord2pulse(y+(dist2obj*10)-50,z++,0,1);
	}
	
	double z_ping = 0;
	coord2pulse(y+(dist2obj*10)-50,z,-90,2000);
	coord2pulse(y+(dist2obj*10),z,-90,2000);
	while(z_ping < (double)z/10){
		z_ping = LgetCM(3);
		coord2pulse((y++)+(dist2obj*10),z,-90,1);
	}*/
	
	
	
	/************************************/
	
	//initial user menu and input
	/************************************/
	
	
	
	
	/************************************/
	
	//start from default position and commence environment ping (arm movment with sensor pings -- save positions of hits)
	/************************************/
	
	
	
	
	/************************************/
	
	//if hit go to object and scan (go into object recognition scan -- determine if object or not...repeat if necessary)
	/************************************/
	
	
	
	
	/************************************/
	
	//relocate object
	/************************************/
	
	
	
	
	/************************************/
	
	
	
	
	
	return 1;
}//End main
