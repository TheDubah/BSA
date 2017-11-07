#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <math.h>

//MEASUREMENTS IN 'mm'
#define BICEP 146.05
#define FOREARM 187.325
#define EFFECTOR 85.725

#define BASE_CH 9
#define SHOULDER_CH 7
#define ELBOW_CH 5
#define WRIST_CH 3
#define CLAW_CH 1

#define Y_MAX 260


/*The purpose of this command (and other versions of this command) is to setup serial communication with ssc32u board via USB.
	Parameters include channel(which servo channel connection on board), pulse(what pulse to output on that channel), 
	and time(how long to reach said pulse in milliseconds).*/
	
int servo_command1(int channel_1, int pulse_1, int time){
	//initialize and open serial connection with USB0 along with relevant flags
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}

	//strings to hold int parameters
	char s_channel[3];
	char s_pulse[5];
	char s_time[6];

	//convert to string
	sprintf(s_channel, "%d", channel_1);
	sprintf(s_pulse, "%d", pulse_1);
	sprintf(s_time, "%d", time);

	//parse command to send
	// FORMAT: #<ch> P<pulse> T<time> OR #xxPxxxxTxxxxx\r
	char command[16] = "#";

	strcat(command,s_channel);
	strcat(command,"P");
	strcat(command,s_pulse);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");

	//write to serial device and also error checking
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}

	//close serial device
	close(ssc32u);

	//sleep until move is complete +1 second for margin
	usleep((time+1)*1000);

	return 1;
}


/*Similar function but addresses up to 2 servos by taking two channels, two pulses for
	respective channels and a time to reach those pulses for those channels*/
	
int servo_command2(int channel_1, int pulse_1, int channel_2, int pulse_2, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxxTxxxxx\r
	char command[25] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1)*1000);
	
	return 1;
}

/*To command up to 3 servos at once*/
int servo_command3(int channel_1, int pulse_1, int channel_2, int pulse_2, int channel_3, int pulse_3, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_channel_3[3];
	char s_pulse_3[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_channel_3, "%d", channel_3);
	sprintf(s_pulse_3, "%d", pulse_3);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxx#xxPxxxxTxxxxx\r
	char command[33] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"#");
	strcat(command,s_channel_3);
	strcat(command,"P");
	strcat(command,s_pulse_3);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1)*1000);
	
	return 1;
}

/*To command up to 4 servos at once*/
int servo_command4(int channel_1, int pulse_1, int channel_2, int pulse_2, int channel_3, int pulse_3, int channel_4, int pulse_4, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_channel_3[3];
	char s_pulse_3[5];
	char s_channel_4[3];
	char s_pulse_4[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_channel_3, "%d", channel_3);
	sprintf(s_pulse_3, "%d", pulse_3);
	sprintf(s_channel_4, "%d", channel_4);
	sprintf(s_pulse_4, "%d", pulse_4);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxx#xxPxxxx#xxPxxxxTxxxxx\r
	char command[41] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"#");
	strcat(command,s_channel_3);
	strcat(command,"P");
	strcat(command,s_pulse_3);
	strcat(command,"#");
	strcat(command,s_channel_4);
	strcat(command,"P");
	strcat(command,s_pulse_4);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1)*1000);
	
	return 1;
}

/*To command up to 5 servos at once*/
int servo_command5(int channel_1, int pulse_1, int channel_2, int pulse_2, int channel_3, int pulse_3, int channel_4, int pulse_4, int channel_5, int pulse_5, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_channel_3[3];
	char s_pulse_3[5];
	char s_channel_4[3];
	char s_pulse_4[5];
	char s_channel_5[3];
	char s_pulse_5[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_channel_3, "%d", channel_3);
	sprintf(s_pulse_3, "%d", pulse_3);
	sprintf(s_channel_4, "%d", channel_4);
	sprintf(s_pulse_4, "%d", pulse_4);
	sprintf(s_channel_5, "%d", channel_5);
	sprintf(s_pulse_5, "%d", pulse_5);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxx#xxPxxxx#xxPxxxx#xxPxxxxTxxxxx\r
	char command[49] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"#");
	strcat(command,s_channel_3);
	strcat(command,"P");
	strcat(command,s_pulse_3);
	strcat(command,"#");
	strcat(command,s_channel_4);
	strcat(command,"P");
	strcat(command,s_pulse_4);
	strcat(command,"#");
	strcat(command,s_channel_5);
	strcat(command,"P");
	strcat(command,s_pulse_5);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1)*1000);
	
	return 1;
}

/*************************************************************************************************************************************/

/*coord2pulse takes in y,z points on the 2-D plane seen from the side (not including base rotation) and calculates the kinematics
	for the arm to move forward/back or up/down properly. wrist_direction can be 0 (parallel to ground) or -90(pointing down at ground)
	The time parameter syncs all movement and is passed to internal servo_commandX() function calls. This function only calculates kinematics
	for shoulder,elbow and wrist and not for base therefore you must rotate the base to the desired position before calling coord2pulse.
	
	With only the 3 servos to work with, the idea behind these kinematics is to use triangle math (law of sines/cosines) to calculated
	the angles required for the joints to have the end of the effector reach a certain y-z point.
	
	The return type is a flag that indicates whether any of the servo calculations are out of bounds (flag=1) or not (flag=0)*/

int coord2pulse(double y, double z, int wrist_direction, int time){
	
	/*ERROR CHECKING FOR XYZ BOUNDS*/
	if (y > Y_MAX){
		return 1;
	}
	
	/*variables used for trig calculations below*/
	double zy_theta, d, ad_theta, shoulder_theta, shoulder_theta_deg, elbow_theta, elbow_theta_deg, wrist_theta, wrist_theta_deg;
	
	/*adjust for effector length based where the wrist is pointing (forward or down)*/
	//wrist is pointing forward
	if (wrist_direction==0){
		y = y - EFFECTOR;
	}
	//wrist is pointing down
	else if(wrist_direction==-90){
		z = z + EFFECTOR;
	}
	
	/*d = HYPOTENUSE between imaginary triangle with y-z plane and BICEP-FOREARM (real) triangle*/
	d = sqrt((y*y)+(z*z));
	
	/*elbow angle calculated via LAW OF COSINES and seperate degree variable calculated*/
	elbow_theta = acos( ((FOREARM*FOREARM)+(BICEP*BICEP)-(d*d)) / (2*BICEP*FOREARM) );
	elbow_theta_deg = elbow_theta*180.0/M_PI;
	
	
	/*angle from bottom of plane to straight line towards y,z point (IMAGINARY TRIANGLE)*/
	zy_theta = atan(z/y);
	
	/*ad_theta angle between BICEP and d (HYPOTENUSE) using LAW OF COSINES*/
	ad_theta = acos( ((d*d)+(BICEP*BICEP)-(FOREARM*FOREARM)) / (2*BICEP*d) );
	//ad_theta_deg = ad_theta*180.0/M_PI;
	
	/*final shoulder angle calculation using ad_theta and xy_theta (imaginary triangle opposite to arm triangle)*/
	shoulder_theta = ad_theta + zy_theta;
	shoulder_theta_deg = shoulder_theta*180.0/M_PI;
	
	
	//------------------------------------------------------------------------------------------------------------------
	/*Calculation of the end of elbow is complete. Now the triangle with the end of wrist, wrist joint servo and end of elbow.
		All used to calculate the angle for the wrist with respect to direction input parameter*/
	
	double y1 = BICEP*cos(shoulder_theta);
	double z1 = BICEP*sin(shoulder_theta);
	double d1 = sqrt( ((y-y1)*(y-y1)) + ((z-EFFECTOR-z1)*(z-EFFECTOR-z1)) );
	
	/*calculating final wrist angle in radians with bd_theta and angle between z-axis and hypotenuse*/
	
	/*WRIST IS PARALLEL TO GROUND (0) OR PERPENDICULAR TO GROUND (-90). Also converts to degree values*/
	if (wrist_direction==0){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) ) + M_PI/2;
	}
	else if(wrist_direction==-90){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) );
	}
	
	wrist_theta_deg = (wrist_theta*180.0/M_PI);

	//------------------------------------------------------------------------------------------------------------------
	/* CONVERSION FROM DEGREES TO PULSES*/
	
	/*flag used to indicate whether the previous calculations are out of bounds for the pulse range of each servo
		pulse ranges were obtained from servo manufacturing website*/
	int flag = 0;
	
	/*steps/deg were calculated for each servo by setting servo to go from 0-90 or 0-180 degrees and recording servo pulses
		required for this movement. Then by dividing pulses/degrees you get conversion factor that can be multiplied to
		convert between the two. This was done individually for each servo and contains human error.*/
		
	//SHOULER STEPS/DEG = 9.61111
	//round function used to obtain int from double (servos only take whole number pulses)
	//servo offset: 550 (used as starting point for shoulder servo and is different for other servos).
	int shoulder_pulse = round( (shoulder_theta_deg*9.61111) + 550 ); //556 (another offset to try)

	//flag checking to see if the calcuated servo pulse is beyond safe bounds for shoulder servo pulse (2420).
	if(shoulder_pulse > 2420){
		shoulder_pulse = 2420;
		flag = 1;
	}
	
	//ELBOW STEPS/DEG = 9.277777778
	/*Offset starts from 2230 because elbow servo goes from high bound to lower bound on pulses
		so higher bound is subtracted from.*/
	int elbow_pulse = round( 2230 - ((elbow_theta_deg-18.8622754)*9.277777778) );

	//out of bounds flag checking for elbow servo
	if(elbow_pulse < 556){
		shoulder_pulse = 556;
		flag = 1;
	}
	
	//WRIST STEPS/DEG = 10.277777778
	//Servo offset: 500
	
	//int wrist_pulse = round ( 500 + ((wrist_theta_deg-68.1081081)*10.277777778) );
	int wrist_pulse = round ( 500 + ((wrist_theta_deg-60)*10.277777778) ); //553 (another offset to try)

	//out of bounds flag checking for wrist servo
	if(wrist_pulse > 2520){
		shoulder_pulse = 2520;
		flag = 1;
	}
	
	//send calculated pulses via servo_command function
	servo_command3(SHOULDER_CH,shoulder_pulse,ELBOW_CH,elbow_pulse,WRIST_CH,wrist_pulse,time);
	
	//return flag in case it is used to check (outside of this function) for safe bounds
	return flag;
}

/*Very similar to coord2pulse but takes in a basepulse to move base as well (without kinematics)
	and with that also calculates the x-y-z position of the end effector after this movement
	and returns as pointer to array.*/
double * coord2pulse_XYZ(double base_pulse, double y, double z, int wrist_direction, int time){
	
	static double xyz[3]; // To be returned to top array
	
	/*ERROR CHECKING FOR XYZ BOUNDS*/
	if (y > Y_MAX){
		return xyz;
	}
	
	double zy_theta, d, ad_theta, shoulder_theta, shoulder_theta_deg, elbow_theta, elbow_theta_deg, wrist_theta, wrist_theta_deg;
	
	//needed for xyz calc later on
	double r = y;
	
	//adjust for effector length
	if (wrist_direction==0){
		y = y - EFFECTOR;
	}
	else if(wrist_direction==-90){
		z = z + EFFECTOR;
	}
	
	/*d = HYPOTENUSE between fake triangle with y-z plane and BICEP-FOREARM triangle*/
	d = sqrt((y*y)+(z*z));
	
	/*elbow angle via LAW OF COSINES*/
	elbow_theta = acos( ((FOREARM*FOREARM)+(BICEP*BICEP)-(d*d)) / (2*BICEP*FOREARM) );
	elbow_theta_deg = elbow_theta*180.0/M_PI;
	//printf("elbow_theta_deg: %lf\n",elbow_theta_deg);
	
	
	//angle from bottom of plane to straight line towards y,z point (FAKE TRIANGLE)
	zy_theta = atan(z/y);
	
	/*ad_theta angle between BICEP and d (HYPOTENUSE) using LAW OF COSINES*/
	ad_theta = acos( ((d*d)+(BICEP*BICEP)-(FOREARM*FOREARM)) / (2*BICEP*d) );
	//ad_theta_deg = ad_theta*180.0/M_PI;
	
	/*final shoulder angle calculation using ad_theta and xy_theta (fictitious triangle opposite to arm triangle)*/
	shoulder_theta = ad_theta + zy_theta;
	shoulder_theta_deg = shoulder_theta*180.0/M_PI;
	//printf("shoulder_theta_deg: %lf\n",shoulder_theta_deg);
	
	
	//------------------------------------------------------------------------------------------------------------------
	
	double y1 = BICEP*cos(shoulder_theta);
	double z1 = BICEP*sin(shoulder_theta);
	double d1 = sqrt( ((y-y1)*(y-y1)) + ((z-EFFECTOR-z1)*(z-EFFECTOR-z1)) );
	
	/*calculating final wrist angle in radians with bd_theta and angle between z-axis and hypotenuse*/
	
	/*WRIST IS PARALLEL TO GROUND (0) OR PERPENDICULAR TO GROUND (-90). Also converts to degree values*/
	if (wrist_direction==0){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) ) + M_PI/2;
	}
	else if(wrist_direction==-90){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) );
	}
	
	wrist_theta_deg = (wrist_theta*180.0/M_PI);
	//printf("wrist_theta_deg: %lf\n",wrist_theta_deg);
	
	//------------------------------------------------------------------------------------------------------------------
	/* CALCULATING ACTUAL XYZ TO RETURN*/
	double base_deg = (180/2000)*(base_pulse-500);
	
	//real X
	xyz[0] = r*cos(base_deg);
	
	//real Y
	xyz[1] = r*sin(base_deg);
	
	//real_Z
	xyz[2] = z;
	
	//------------------------------------------------------------------------------------------------------------------
	/* CONVERSION TO PULSE FROM DEGREES*/
	
	//SHOULER STEPS/DEG = 9.61111
	int shoulder_pulse = round( (shoulder_theta_deg*9.61111) + 550 );
	//printf("shoulder_pulse: %d\n",shoulder_pulse);
	
	//ELBOW STEPS/DEG = 9.277777778
	int elbow_pulse = round( 2230 - ((elbow_theta_deg-18.8622754)*9.277777778) );
	//printf("elbow_pulse: %d\n",elbow_pulse);
	
	//WRIST STEPS/DEG = 10.277777778
	//int wrist_pulse = round ( 500 + ((wrist_theta_deg-68.1081081)*10.277777778) );
	int wrist_pulse = round ( 500 + ((wrist_theta_deg-60)*10.277777778) );
	//printf("wrist_pulse: %d\n",wrist_pulse);
	
	//send calculated pulses via servo_command function
	servo_command4(BASE_CH, base_pulse, SHOULDER_CH,shoulder_pulse,ELBOW_CH,elbow_pulse,WRIST_CH,wrist_pulse,time);
	
	return xyz;
}


/*Relocation Code*/
void reloc(int newx, int newy){
// servo_command1(9, 1400, 9000);
		//Turn on magnet
		setvalue(MAGN, 1);
        
		int y = 130; // y = 130
        int z = 25; // z = 25

        coord2pulse(y,z,0,2000); //Default position

        servo_command1(9, newx, 4000); // Going to the location to face the object

        //coord2pulse(y,z,0,4000); //Default position
        
        coord2pulse(y,90,0,2000);//Going up to the opbject height
      
        coord2pulse(newy-10,75,-90,2000); //-90 to bend the gripper down and 230 moving forward
        sleep(1);

        coord2pulse(y, 200,-90,2000); //Go all the way up
       
        servo_command1(9, 2500, 2000);
 
        coord2pulse(y, 60,-90,2000);  //
		
		//Turn off magnet
		setvalue(MAGN, 0);
		
        coord2pulse(y, 100,-90,2000); //
        servo_command1(9, 1500, 2000);
        coord2pulse(y, z,0,2000); //Default position again

        //printf(" I am done!");
}