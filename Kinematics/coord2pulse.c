#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//MEASUREMENTS IN 'mm'
#define BICEP 146.05
#define FOREARM 187.325
#define EFFECTOR 85.725

//X-axis is left to right on plane, Y-axis is front to back on plane and Z-axis is bottom to top if looking from Arm POV.

int * coord2pulse(double x, double y, double z, int wrist_direction){
	
	/*ERROR CHECKING FOR XYZ BOUNDS*/
	if (wrist_direction==0){}
	else if(wrist_direction==-90){}
	else{}
	
	
	static int pulses[4];
	
	double zy_theta, d, ad_theta, shoulder_theta, shoulder_theta_deg, elbow_theta, elbow_theta_deg, wrist_theta, wrist_theta_deg;
	
	//adjust for effector length
	/*if (wrist_direction==0){
		y = y - EFFECTOR;
	}
	else if(wrist_direction==-90){
		z = z + EFFECTOR;
	}*/
	
	/*d = HYPOTENUSE between fake triangle with y-z plane and BICEP-FOREARM triangle*/
	d = sqrt((y*y)+(z*z));
	
	/*elbow angle via LAW OF COSINES*/
	elbow_theta = acos( ((FOREARM*FOREARM)+(BICEP*BICEP)-(d*d)) / (2*BICEP*FOREARM) );
	elbow_theta_deg = elbow_theta*180.0/M_PI;
	printf("elbow_theta_deg: %lf\n",elbow_theta_deg);
	
	
	//angle from bottom of plane to straight line towards y,z point (FAKE TRIANGLE)
	zy_theta = atan(z/y);
	
	/*ad_theta angle between BICEP and d (HYPOTENUSE) using LAW OF COSINES*/
	ad_theta = acos( ((d*d)+(BICEP*BICEP)-(FOREARM*FOREARM)) / (2*BICEP*d) );
	//ad_theta_deg = ad_theta*180.0/M_PI;
	
	/*final shoulder angle calculation using ad_theta and xy_theta (fictitious triangle opposite to arm triangle)*/
	shoulder_theta = ad_theta + zy_theta;
	shoulder_theta_deg = shoulder_theta*180.0/M_PI;
	printf("shoulder_theta_deg: %lf\n",shoulder_theta_deg);
	
	
	//------------------------------------------------------------------------------------------------------------------
	
	double y1 = BICEP*cos(shoulder_theta);
	double z1 = BICEP*sin(shoulder_theta);
	double d1 = sqrt( ((y-y1)*(y-y1)) + ((z-EFFECTOR-z1)*(z-EFFECTOR-z1)) );
	
	/*bd_theta is angle between FOREARM and hypotenuse*/
	//bd_theta_deg = 180.0 - ad_theta - elbow_theta_deg;
	//bd_theta = bd_theta_deg*M_PI/180;
	//printf("bd_theta_deg: %lf\n",bd_theta_deg);
	
	/*calculating final wrist angle in radians with bd_theta and angle between z-axis and hypotenuse*/
	
	/*WRIST IS PARALLEL TO GROUND (0) OR PERPENDICULAR TO GROUND (-90). Also converts to degree values*/
	if (wrist_direction==0){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) ) + M_PI/2;
	}
	else if(wrist_direction==-90){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) );
	}
	
	wrist_theta_deg = (wrist_theta*180.0/M_PI);
	printf("wrist_theta_deg: %lf\n",wrist_theta_deg);
	//------------------------------------------------------------------------------------------------------------------
	
	/*BASE ROTATION CALC*/
	/*NEED TO CHECK FOR ARM LIMITS!!!*/
	double base_theta;
	if (x == 0){
		base_theta = M_PI/2;
	}
	else if (x < 0){
		base_theta = M_PI - atan(y/(-x));
	}
	else {
		base_theta = atan(y/x);
	}
	
	double base_theta_deg = base_theta*180.0/M_PI;
	printf("base_theta_deg: %lf\n\n",base_theta_deg);
	
	/* CONVERSION TO PULSE FROM DEGREES*/
	int base_pulse = round( (base_theta_deg-0) * (2500-500) / (180-0) + 500 );
	
	int shoulder_pulse = round( (shoulder_theta_deg*9.61111) + 550 );
	
	int elbow_pulse = round( 2230 - ((elbow_theta_deg+7.26699)*8.94444) );
	
	int wrist_pulse = round ( 500 + ((elbow_theta_deg-63.2432432)*10.277777778) );
	
	/*int base_pulse = round( (base_theta_deg-0) * (2500-500) / (180-0) + 500 );
	
	int shoulder_pulse = round( (shoulder_theta_deg-0) * (2280-550) / (180-0) + 500 );
	
	int elbow_pulse = round( (elbow_theta_deg-0) * (2230-750) / (180-30) + 500 );
	
	int wrist_pulse = round( (wrist_theta_deg-0) * (2500-500) / (260-60) + 500 );*/
	
	pulses[0] = base_pulse;
	pulses[1] = shoulder_pulse;
	pulses[2] = elbow_pulse;
	pulses[3] = wrist_pulse;
	
	
	return pulses;
}


int * coord2pulse_YZ(double y, double z, int wrist_direction){
	
	/*ERROR CHECKING FOR XYZ BOUNDS*/
	if (wrist_direction==0){}
	else if(wrist_direction==-90){}
	else{}
	
	
	static int pulses[3];
	
	double zy_theta, d, ad_theta, shoulder_theta, shoulder_theta_deg, elbow_theta, elbow_theta_deg, wrist_theta, wrist_theta_deg;
	
	//adjust for effector length
	/*if (wrist_direction==0){
		y = y - EFFECTOR;
	}
	else if(wrist_direction==-90){
		z = z + EFFECTOR;
	}*/
	
	/*d = HYPOTENUSE between fake triangle with y-z plane and BICEP-FOREARM triangle*/
	d = sqrt((y*y)+(z*z));
	
	/*elbow angle via LAW OF COSINES*/
	elbow_theta = acos( ((FOREARM*FOREARM)+(BICEP*BICEP)-(d*d)) / (2*BICEP*FOREARM) );
	elbow_theta_deg = elbow_theta*180.0/M_PI;
	printf("elbow_theta_deg: %lf\n",elbow_theta_deg);
	
	
	//angle from bottom of plane to straight line towards y,z point (FAKE TRIANGLE)
	zy_theta = atan(z/y);
	
	/*ad_theta angle between BICEP and d (HYPOTENUSE) using LAW OF COSINES*/
	ad_theta = acos( ((d*d)+(BICEP*BICEP)-(FOREARM*FOREARM)) / (2*BICEP*d) );
	//ad_theta_deg = ad_theta*180.0/M_PI;
	
	/*final shoulder angle calculation using ad_theta and xy_theta (fictitious triangle opposite to arm triangle)*/
	shoulder_theta = ad_theta + zy_theta;
	shoulder_theta_deg = shoulder_theta*180.0/M_PI;
	printf("shoulder_theta_deg: %lf\n",shoulder_theta_deg);
	
	
	//------------------------------------------------------------------------------------------------------------------
	
	double y1 = BICEP*cos(shoulder_theta);
	double z1 = BICEP*sin(shoulder_theta);
	double d1 = sqrt( ((y-y1)*(y-y1)) + ((z-EFFECTOR-z1)*(z-EFFECTOR-z1)) );
	
	/*bd_theta is angle between FOREARM and hypotenuse*/
	//bd_theta_deg = 180.0 - ad_theta - elbow_theta_deg;
	//bd_theta = bd_theta_deg*M_PI/180;
	//printf("bd_theta_deg: %lf\n",bd_theta_deg);
	
	/*calculating final wrist angle in radians with bd_theta and angle between z-axis and hypotenuse*/
	
	/*WRIST IS PARALLEL TO GROUND (0) OR PERPENDICULAR TO GROUND (-90). Also converts to degree values*/
	if (wrist_direction==0){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) ) + M_PI/2;
	}
	else if(wrist_direction==-90){
		wrist_theta = acos( ((FOREARM*FOREARM)+(EFFECTOR*EFFECTOR)-(d1*d1)) / (2*FOREARM*EFFECTOR) );
	}
	
	wrist_theta_deg = (wrist_theta*180.0/M_PI);
	printf("wrist_theta_deg: %lf\n\n",wrist_theta_deg);
	//------------------------------------------------------------------------------------------------------------------
	
	
	/* CONVERSION TO PULSE FROM DEGREES*/
	int shoulder_pulse = round( (shoulder_theta_deg*9.61111) + 550 );
	
	int elbow_pulse = round( 2230 - ((elbow_theta_deg+7.26699)*8.94444) );
	
	int wrist_pulse = round ( 500 + ((elbow_theta_deg-63.2432432)*10.277777778) );
	
	pulses[0] = shoulder_pulse;
	pulses[1] = elbow_pulse;
	pulses[2] = wrist_pulse;
	
	
	return pulses;
}


int main(void){
	
	int *servo_pulses;
	
	//servo_pulses = coord2pulse(50,100,100, -90);
	//servo_pulses = coord2pulse_YZ(100,100, -90);
	servo_pulses = coord2pulse_YZ(187.325,146.05,-90);
	//printf("BASE: %d\n",servo_pulses[0]);
	printf("SHOULDER: %d\n",servo_pulses[0]);
	printf("ELBOW: %d\n",servo_pulses[1]);
	printf("WRIST: %d\n",servo_pulses[2]);
	
	return 0;
}