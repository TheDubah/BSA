//STANDARD LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

//MULTI-THREAD/ERROR HANDLING
#include <signal.h>
#include <pthread.h>

//ORIGINAL LIBRARIES
#include "sensors.h" //-> Used for obtaining distances from left and right receivers
#include "ssc32usb.h" //-> Used for arm movement
#include "neuralnet.h" //-> Used for Neural Network functionality
#include "regression.h" //-> Used for polynomial regression

#define PINGCOUNT 1
#define SCAN1BUFFER 10
#define SCAN2BUFFER 5
#define SCAN3BUFFER 5
#define BORDER 200
#define STARTBASE 1700
#define ENDPULSE 1100
#define STARTINGY 130
#define STARTINGZ 60
#define DIST2OBJ 100
#define BIGDELTA 5000
#define SCALE1FACTOR 100000 //Scaling for training values to below 1
#define SCALE2FACTOR 1000000
#define SIZESEPERATOR 120 //Seperating value for big and small shapes
#define SCAN2MIN 30 //Minimum scan before dyncamic buffer takes over to detect edge
#define OBJECTSONPLANE 3
#define SPEED 1
#define SCAN1FILTRANGE 50
#define SCAN2FILTRANGE 40
#define RELOCTIME 2000

#define COEFFICIENTS 3

#define SCAN1TRIM 100

//Neural Network Macros
#define SEED 900


//Function declaration used within main but defined below
void relocate (int mid_point, int y, int z);
void train_all(void);

//main program/operation flow
int main(void){
	
	/*User Input*/
	//Ask user what object they want to grab
	int i;
	int invalidInput = 1;
	int userLabelID;
	
	while(invalidInput){
		char userInput;
		printf("Please enter shape to grab or select training:\n");
		printf("1: Cube\n");
		printf("2: Cylinder\n");
		printf("3: Sphere\n");
		printf("4: Training\n");
		userInput = getchar();
	

		//Obtain labelID from input
		if(userInput == '1'){
			userLabelID = 0;
			invalidInput = 0;
		}
		else if(userInput == '2'){
			userLabelID = 1;
			invalidInput = 0;
		}
		else if(userInput == '3'){
			userLabelID = 2;
			invalidInput = 0;
		}
		else if(userInput == '4'){
			train_all();
			return 1;
		}
		else{
			printf("Invalid input!!\n");
			printf("------------------\n\n");
		}
	}
	
	//For sensors setup
	setup();
	
	//Starting y-z position (can be changed)
	int y = STARTINGY;
	int z = STARTINGZ;
	coord2pulse(y,z,0,4000);
	
	//Variables used later, names give some idea but read further
	int j,m;
	double left_ping;//, right_ping;
	double left_buffer[SCAN1BUFFER];//, right_buffer[SCAN1BUFFER]
	int left_valid;//, right_valid;
	int object_started = 0;
	int object_start, object_end;
	int scan2FiltMax, scan1FiltMax;
	int mid_point;
	int deltaStart;
	
	int unique[50];
	int isUnique;
	int mode;
	int max_repeat;
	int repeat;
	int uIndex;
	int scan2Valid_reset;
	
	//Smooth start
	servo_command1(BASE_CH,STARTBASE,4000);
	coord2pulse(y,z,0,4000);
	
	double scan1[500];
	double scan2[500];
	//double scan1R[500];
	//double scan2R[500];
	
	double scan1Filt[500];
	double scan2Filt[500];
	
	int a; //index for saving into scan1 array
	int b;
	
	//object struct array to hold objects
	//int curr_objInd = 0;
	//Object *objects[OBJECTSONPLANE];
	
	
	//initialize buffer with values that don't false trigger object start
	for(i=0;i<SCAN1BUFFER;i++){
		left_buffer[i] = 9999;
	}
	
	/*The major for-loop for the sweep scan that goes ~180 deg to scan for objects*/
	//for(i=0;i<700;i=i+1){
	for(m=STARTBASE;m>ENDPULSE;m--){
		printf("m=%d\n",m);
		
		//STARTBASE is the constant starting pulse for the base servo. Can be adjusted with MACRO above.
		servo_command1(BASE_CH,m,SPEED);
		coord2pulse(y,z,0,1);
		
		//used as counters when going back through buffer to count how many pings are proper object pings. left_valid for Left reciever and right_valid is for Right sensor.
		left_valid = 0;
		//right_valid = 0;
		
		/***************************************************/
		//left sensor pings with LgetCM(). PINGCOUNT is the number of times to ping and return the lowest ping distance from the count. can be adjusted with MACRO above.
		left_ping = LgetCM(PINGCOUNT) * 10;
		
		//This for-loop simply iterates through the buffer, pushing elements down one by one, and adding the newest ping to the the first [0] spot in buffer array.
		//	It also counts the number of valid object pings in the buffer. Valid pings are determined by seeing if they are smaller than the BORDER (aka an object and not empty space,
		//	border or table.
		for(j=SCAN1BUFFER-1;j>0;j--){
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
		/*right_ping = RgetCM(PINGCOUNT) * 10;

		for(j=SCAN1BUFFER-1;j>0;j--){
			right_buffer[j] = right_buffer[j-1];
			if(right_buffer[j] < BORDER){
				right_valid++;
			}
		}
		right_buffer[0] = right_ping;
		
		printf("Right Echo: %lf\n",right_ping);*/
		
		/***************************************************/
		
		//Store scan1 pings if sweep discovered object
		/*if(object_started == 1){
			scan1[a] = left_ping;
			//scan1R[a] = right_ping;
			printf("%d: %lf\n",a,scan1[a]);
			a++;
			scan1Min--;
		}*/
		
		//These two if statements are used to look back at array in this iteration of loop to see if we have enough valid pings (4) to safely
		// mark an edge of an object. If object_started is 0 (initial state) then it is marked with 1 and the object_start base pulse is saved.
		if(left_valid>=(SCAN1BUFFER/2) && object_started==0){
		//if(left_valid>=(SCAN1BUFFER/2) && right_valid>=(SCAN1BUFFER/2) && object_started==0){	
			object_started = 1;
			object_start = m-(SCAN1BUFFER/2);
			printf("------------------Object Start @ m=%d------------------\n",m-(SCAN1BUFFER/2));
			a = 0;
			b = 0;
			
			//reset scan1 and scan2 arrays
			for(i=0;i<500;i++){
				scan1[i] = 0;
				scan2[i] = 0;
				scan1Filt[i] = 0;
				scan2Filt[i] = 0;
			}
			//a_size = m;
			
			//Save the xyz coordinates of the starting edge to objectStart_xyz by copying.
			/*for(j=0;j<(SCAN1BUFFER/2)-1;j++){
				objectStart_xyz[j] = curr_xyz[j];
			}*/
			
			
		}

		// If object_started is 1 and there are less than 4 valid pings, this means you are no longer looking at object and must now mark
		// object end. object_started is 0 and base pulse is saved in object_end.
		if(left_valid<(SCAN1BUFFER/2) && object_started==1){
		//if(left_valid<(SCAN1BUFFER/2) && object_started==1 && scan1Min < 1){
		//if(left_valid<(SCAN1BUFFER/2) && right_valid<(SCAN1BUFFER/2) && object_started==1){
			object_started = 0;
			//int a_size = a;
			//a_size -= m;
			object_end = m-(SCAN1BUFFER/2);
			printf("------------------Object End @ m=%d------------------\n",m-(SCAN1BUFFER/2));
			
			//Save the xyz coordinates of the falling off edge to objectEnd_xyz by copying.
			/*for(j=0;j<(SCAN1BUFFER/2)-1;j++){
				objectEnd_xyz[j] = curr_xyz[j];
			}*/
			
			
			//Create object struct
			Object *obj = (Object *)malloc(sizeof(Object));
			
			mid_point = ((object_end - object_start) / 2) + object_start;
			obj -> mid_point = mid_point;
			
			//Move to midpoint
			servo_command1(BASE_CH,mid_point,2000);
			
			/*Get close to object within certain distance*/
			double y_ping = LgetCM(PINGCOUNT) * 10; //to get mm from cm ping
			//double y_pingR = RgetCM(PINGCOUNT) * 10;
			while(y_ping > DIST2OBJ){
				y_ping = LgetCM(PINGCOUNT) * 10; //to get mm for cm
				printf("Y_PING: %lf\n",y_ping);
				y++;
				coord2pulse(y,z,0,1);
			}
			
			//Save distance to object for relocation code
			obj -> dist2obj = DIST2OBJ + y;
			
			//int flag = 0;
			int flag;
			y_ping = LgetCM(PINGCOUNT);
			
			b = 0;
			
			//STEPHANES FUNCTION FOR TESTING ONLY		
			/*while(b<50){
				y_ping = LgetCM(PINGCOUNT) * 10;
				scan2[b] = y_ping;
				coord2pulse(y,z++,0,1);
				printf("%lf\n",scan2[b]);
				b++;
			}*/
			
			
			//Scan2 with minimum 20mm upwards scan (safety)
			// scan2_valid used as buffer to detect proper edge
			//Go back down from bottom of scan
			//coord2pulse(y,25,0,4000);
			int scan2_valid = SCAN2BUFFER;
			while( ((scan2_valid > 0) && (flag == 0)) || (b<SCAN2MIN) ){
				y_ping = LgetCM(PINGCOUNT) * 10;
				
				//scan2[b] = y_ping;
				
				if(y_ping > BORDER){
					scan2Valid_reset = 0;
					if(scan2Valid_reset == 0){
						scan2_valid--;
					}
				}
				else{
					scan2Valid_reset = 1;
					scan2_valid = SCAN2BUFFER;
				}
				
				//y_pingR = RgetCM(PINGCOUNT) * 10;
				//scan2R[b] = y_pingR;
				
				flag = coord2pulse(y,z++,0,1);
				b++;
			}
			
			//Get scan2 midpoint
			obj -> side2_midpoint = 25 + ((z-25)/2);
			
			//Save height for object
			obj -> objheight = z;
			
			
			//Top down scan
			b = 0;
			for(i=z;i>25;i--){
				flag = coord2pulse(y,i,0,SPEED);
				y_ping = LgetCM(PINGCOUNT) * 10;
				scan2[b] = y_ping;
				b++;
			}
			
			//int b_size = b;
			//report that exit was based on limit
			if(flag==1){
				printf("Arm reached limit!\n");
			}
			
			for(i=0;i<b;i++){
				printf("%lf\n",scan2[i]);
			}
	
	
			//Side2 FILTERING
			int side2_unique[50];
			int side2_isUnique;
			int side2_mode = 0;
			int side2_max_repeat = 0;
			int side2_repeat = 0;
			int side2_uIndex = 0;
			//init to -1 so uniques are defined at impossible starting point
			for(i=0;i<50;i++){
				side2_unique[i] = -1;
			}
			
			//Filtering
			//Loop for going through all points in scan
			for(i=0;i<b;i++){
				side2_isUnique = 1;
				
				//loop to check if current scan value is unique(new)
				for(j=0;j<50;j++){
					if( (int)floor(scan2[i]) == side2_unique[j] ){
						side2_isUnique = 0;
					}
				}
				
				//if it remains unique after checking all uniques add to unique and check how many (mode)
				if(side2_isUnique == 1){
					side2_unique[side2_uIndex] = (int)floor(scan2[i]);
					
					side2_repeat = 1;
					for(j=i;j<b;j++){
						if( (int)floor(scan2[j]) == (side2_unique[side2_uIndex])){
							side2_repeat++;
						}
					}
					
					//if largest count of this unique value, set as mode
					if(side2_repeat > side2_max_repeat){
						side2_max_repeat = side2_repeat;
						side2_mode = side2_unique[side2_uIndex];
					}
					side2_uIndex++;
				}
			}
			
			
			//Actual filtering
			scan2FiltMax = 0;
			for(i=0;i<b;i++){
				if( (scan2[i] < (side2_mode + SCAN2FILTRANGE)) && (scan2[i] > (side2_mode - SCAN2FILTRANGE)) ){
				//if( scan2[i] < BORDER ){
					scan2Filt[scan2FiltMax] = scan2[i];
					scan2FiltMax++;
				}
			}
			//b_size = scan2FiltMax;
			
			for(i=0;i<scan2FiltMax;i++){
				printf("FILTERED: %lf\n",scan2Filt[i]);
			}
			
			/*
			int scan2ChopMax = scan2FiltMax-20;
			double scan2Chop[scan2ChopMax];
			int chopInd = 0;
			for(i=10;i<scan2FiltMax-10;i++){
				scan2Chop[chopInd] = scan2Filt[i];
				chopInd++;
			}
			
			regression(COEFFICIENTS, scan2Chop,chopInd,obj);*/
			regression(COEFFICIENTS, scan2Filt,scan2FiltMax,obj);
			
			//print coefficients
			printf("SIDE 2 COEFFICIENTS: ");
			for(i=COEFFICIENTS-1;i>=0;i--){
				printf("x^%d: %lf ",i,obj->coeff[i]);
			}
			printf("\n");
			
			//deltaStart = (scan2FiltMax/2);
			deltaStart = 1;
			//Calculate deltas for input into side network
			double scan2Delta[SIDE2INPUT];
			int deltaIndex = 0;
			int trueDelta = BIGDELTA/SIDE2INPUT;
			//for(i=1;i<vertStart+101;i++){
			for(i=deltaStart;i<deltaStart+BIGDELTA;i=i+trueDelta){
				//scan2Delta[deltaIndex] = (((obj -> coeff[2])*pow(i+trueDelta,2)) + ((obj -> coeff[1])*pow(i+trueDelta,1)) + (obj -> coeff[0])) - (((obj -> coeff[2])*pow(i,2)) + ((obj -> coeff[1])*pow(i,1)) + (obj -> coeff[0]));
				scan2Delta[deltaIndex] = fabs( ((obj -> coeff[2])*pow(i+trueDelta,2)) - ((obj -> coeff[2])*pow(i,2)));
				printf("Delta: %lf\n", scan2Delta[deltaIndex]);
				deltaIndex++;
			}
			
			double scan2DeltaScaled[SIDE2INPUT];
			for(i=0;i<SIDE2INPUT;i++){
				scan2DeltaScaled[i] = scan2Delta[i] / SCALE2FACTOR;
			}
			
			//
			char side4Log[20];
			side4Log[1] = 'X';
			
			writeRawScan("Scan2",scan2Filt,scan2FiltMax);
			writeRawLog("Scan2_Log",scan2,b,side4Log);
			writeRawLog("Scan2_Log_Filt",scan2Filt,scan2FiltMax,side4Log);
			
			writeTrainInput("Side2Net",scan2DeltaScaled,SIDE2INPUT);
			//writeTrainInput("Side2NetSmall",scan2DeltaScaled,SIDE2INPUT);
			
			double *result2;
			//BIG SHAPE
			result2 = computeSide2("Side2Net",scan2DeltaScaled);
			for(i=0;i<SIDE2OUTPUT;i++){
				obj -> side2Net_result[i] = result2[i];
				//printf("sideNet2[%d]: %lf\n",i,obj -> side2Net_result[i]);
			}
			printf("Side2Big FLAT: %lf\n",result2[0]);
			printf("Side2Big CURVE: %lf\n\n",result2[1]);
			
			
			//printf("\nPlease enter side for logs \n");
			//fgets(side4Log, 20, stdin);
			
			
			/*Check if side is what you want*/
			//Start by checking what side2 is
			int side1Proceed = 0;
			if((obj -> side2Net_result[0]) > (obj -> side2Net_result[1])){
				//Side2 was flat
				printf("ID: %d\n",userLabelID);
				if((userLabelID == 0) || (userLabelID == 1)){
					side1Proceed = 1;
				}
			}
			else{
				//Side2 was curve
				if(userLabelID == 2){
					side1Proceed = 1;
				}
			}
			
			printf("SIDE1PROCEED: %d\n",side1Proceed);
			if(side1Proceed == 1){
			
				//Set arm to center of object (z-axis)
				coord2pulse(y,obj->side2_midpoint,0,2000);
				
				
				//Move to right edge of object
				int scan1_valid = SCAN2BUFFER;
				int m_temp = obj->mid_point;
				while(scan1_valid > 0){
					y_ping = LgetCM(PINGCOUNT) * 10;
					
					if(y_ping > BORDER){
						scan1_valid--;
					}
					else{
						scan1_valid = SCAN2BUFFER;
					}
					
					//Move base right till it falls off
					servo_command1(BASE_CH,m_temp++,SPEED);
				}
				
				
				//Move left to right scan1
				scan1_valid = SCAN2BUFFER;
				a = 0;
				while(scan1_valid > 0){
					y_ping = LgetCM(PINGCOUNT) * 10;
					
					//Insert into scan1
					scan1[a] = y_ping;
					a++;
					
					if((y_ping > BORDER) && (m_temp<obj->mid_point)){
						scan1_valid--;
					}
					else{
						scan1_valid = SCAN2BUFFER;
					}
					
					servo_command1(BASE_CH,m_temp--,SPEED);
				}
				
				
				//FILTERING
				mode = 0;
				max_repeat = 0;
				repeat = 0;
				uIndex = 0;
				//init to -1 so uniques are defined at impossible starting point
				for(i=0;i<50;i++){
					unique[i] = -1;
				}
				
				//Filtering
				//Loop for going through all points in scan
				for(i=0;i<a;i++){
					isUnique = 1;
					
					//loop to check if current scan value is unique(new)
					for(j=0;j<50;j++){
						if( (int)floor(scan1[i]) == unique[j] ){
							isUnique = 0;
						}
					}
					
					//if it remains unique after checking all uniques add to unique and check how many (mode)
					if(isUnique == 1){
						unique[uIndex] = (int)floor(scan1[i]);
						
						repeat = 1;
						for(j=i;j<a;j++){
							if( (int)floor(scan1[j]) == (unique[uIndex])){
								repeat++;
							}
						}
						
						//if largest count of this unique value, set as mode
						if(repeat > max_repeat){
							max_repeat = repeat;
							mode = unique[uIndex];
						}
						uIndex++;
					}
				}
				
				
				//Actual filtering
				scan1FiltMax = 0;
				for(i=0;i<a;i++){
					if( (scan1[i] < (mode + 20)) && (scan1[i] > (mode - SCAN1FILTRANGE)) ){
					//if( scan1[i] < BORDER ){
						scan1Filt[scan1FiltMax] = scan1[i];
						scan1FiltMax++;
					}
				}
				
				//a_size = scan1FiltMax;
				
				
				writeRawScan("Scan1",scan1Filt,scan1FiltMax);
				writeRawLog("Scan1_Log",scan1,a,side4Log);
				writeRawLog("Scan1_Log_Filt",scan1Filt,scan1FiltMax,side4Log);
				
				/*CURVE FITTING*/
				regression(COEFFICIENTS, scan1Filt,scan1FiltMax,obj);
				
				/*
				double scan1Var;
				for(i=0;i<scan1FiltMax;i++){
					scan1Var = ((obj->coeff[0]) + ((obj->coeff[1])*pow(i,1)) + ((obj->coeff[2])*pow(i,2))) - scan1Filt[i];
					if(scan1Var < 0){
						scan1Var = -scan1Var;
					}
					
					if((scan1Var > 10) && (i > 0)){
						//scan1Filt2[i] = ((obj->coeff[0]) + ((obj->coeff[1])*pow(i,1)) + ((obj->coeff[2])*pow(i,2)));
						scan1Filt2[i] = scan1Filt2[i-1];
					}
					else{
						scan1Filt2[i] = scan1Filt[i];
					}
				}
				
				writeRawLog("Scan1_Log_Filt2",scan1Filt2,scan1FiltMax,side4Log);
				
				regression(COEFFICIENTS, scan1Filt2,scan1FiltMax,obj);
				*/
				
				//print coefficients
				printf("SIDE 1 COEFFICIENTS: ");
				for(i=COEFFICIENTS-1;i>=0;i--){
					printf("x^%d: %lf ",i,obj->coeff[i]);
				}
				printf("\n");
				
				//Grab coefficients for 1st and 2nd power (no constant)
				/*double side1realCoeff[COEFFICIENTS-1];
				for(i=1;i<COEFFICIENTS;i++){
					side1realCoeff[i-1] = obj->coeff[i];
				}
				
				double *side1result = computeSide1("Side1Net",side1realCoeff);
				
				printf("CoeffInputs:\n"); 
				for(i=0;i<COEFFICIENTS;i++){
					printf("%lf ",obj->coeff[i]);
				}
				printf("\n");
				
				printf("Side1Result:\n"); 
				for(i=0;i<SIDE1OUTPUT;i++){
					obj -> side1Net_result[i] = side1result[i];
					printf("%lf ",obj -> side1Net_result[i]);
				}
				printf("\n");*/
				
				//deltaStart = (scan1FiltMax/2);
				deltaStart = 1;
				
				//Calculate deltas for input into side network
				double scan1Delta[SIDE1INPUT];
				deltaIndex = 0;
				trueDelta = BIGDELTA/SIDE1INPUT;
				for(i=deltaStart;i<deltaStart+BIGDELTA;i=i+trueDelta){
					//delta = [i+delta] - [i]
					//scan1Delta[deltaIndex] = (((obj -> coeff[2])*pow(i+trueDelta,2)) + ((obj -> coeff[1])*pow(i+trueDelta,1)) + (obj -> coeff[0])) - (((obj -> coeff[2])*pow(i,2)) + ((obj -> coeff[1])*pow(i,1)) + (obj -> coeff[0]));
					scan1Delta[deltaIndex] = fabs( ((obj -> coeff[2])*pow(i+trueDelta,2)) - ((obj -> coeff[2])*pow(i,2)));
					printf("Delta: %lf\n", scan1Delta[deltaIndex]);
					deltaIndex++;
				}
				
				double scan1DeltaScaled[SIDE1INPUT];
				for(i=0;i<SIDE1INPUT;i++){
					scan1DeltaScaled[i] = scan1Delta[i] / SCALE1FACTOR;
				}
				
				writeTrainInput("Side1Net",scan1DeltaScaled,SIDE1INPUT);
				//writeTrainInput("Side1NetSmall",scan1DeltaScaled,SIDE1INPUT);
				
				writeRawLog("Scan1Inputs_Log",scan1Delta,SIDE1INPUT,side4Log);
				
				double *result1;
				//BIG SHAPE
				//if(a_size > SIZESEPERATOR){
					result1 = computeSide1("Side1Net",scan1DeltaScaled);
					for(i=0;i<SIDE1OUTPUT;i++){
						obj -> side1Net_result[i] = result1[i];
						//printf("sideNet1[%d]: %lf\n",i,obj -> side1Net_result[i]);
					}
					printf("Side1Big FLAT: %lf\n",result1[0]);
					printf("Side1Big CURVE: %lf\n\n",result1[1]);
				//}
				//SMALL SHAPE
				/*else{
					result1 = computeSide1("Side1NetSmall",scan1DeltaScaled);
					for(i=0;i<SIDE1OUTPUT;i++){
						obj -> side1Net_result[i] = result1[i];
						//printf("sideNet1[%d]: %lf\n",i,obj -> side1Net_result[i]);
						
					}
					printf("Side1Small FLAT: %lf\n",result1[0]);
					printf("Side1Small CURVE: %lf\n",result1[1]);
				}*/

				

				//}
				//SMALL SHAPE
				/*else{
					result2 = computeSide2("Side2NetSmall",scan2DeltaScaled);
					for(i=0;i<SIDE2OUTPUT;i++){
						obj -> side2Net_result[i] = result2[i];
						//printf("sideNet2[%d]: %lf\n",i,obj -> side2Net_result[i]);
					}
					printf("Side2Small FLAT: %lf\n",result2[0]);
					printf("Side2Small CURVE: %lf\n",result2[1]);
				}*/
				
				
				//Identifying shape
				double shapeInput[SHAPEINPUT];
				//BIG SHAPE
				//if(a_size > SIZESEPERATOR){
					shapeInput[0] = obj -> side1Net_result[0];
					shapeInput[1] = obj -> side1Net_result[1];
					shapeInput[2] = obj -> side2Net_result[0];
					shapeInput[3] = obj -> side2Net_result[1];
					shapeInput[4] = 0.001;
					shapeInput[5] = 0.001;
					shapeInput[6] = 0.001;
					shapeInput[7] = 0.001;
				//}
				//SMALL SHAPE
				/*else{
					shapeInput[0] = 0.001;
					shapeInput[1] = 0.001;
					shapeInput[2] = 0.001;
					shapeInput[3] = 0.001;
					shapeInput[4] = obj -> side1Net_result[0];
					shapeInput[5] = obj -> side1Net_result[1];
					shapeInput[6] = obj -> side2Net_result[0];
					shapeInput[7] = obj -> side2Net_result[1];
				}*/
				
				double *result3;
				result3 = computeShape("ShapeNet",shapeInput);
				
				//copy result to obj struct and determine max probability for shape index
				double maxShapeProb = 0;
				for(i=0;i<SHAPEOUTPUT;i++){
					obj -> shapeNet_result[i] = result3[i];
					//printf("shapeNet[%d]: %lf\n",i,obj -> shapeNet_result[i]);
					
					if(obj -> shapeNet_result[i] > maxShapeProb){
						maxShapeProb = obj -> shapeNet_result[i];
						obj -> metaLabel = i;
					}
				}
				
				printf("BigCube: %lf\n",obj -> shapeNet_result[0]);
				printf("BigCylinder: %lf\n",obj -> shapeNet_result[1]);
				printf("BigSphere: %lf\n",obj -> shapeNet_result[2]);
				printf("SmallCube: %lf\n",obj -> shapeNet_result[3]);
				printf("SmallCylinder: %lf\n",obj -> shapeNet_result[4]);
				printf("SmallSphere: %lf\n",obj -> shapeNet_result[5]);
				
				printf("DISTANCE TO OBJ: %d\n",obj->dist2obj);
				printf("MIDPOINT: %d\n",obj->mid_point);
				
				
				//Determine the highest probability result
				int object_detectedInd = 0;
				double highest_prob = obj -> shapeNet_result[0];
				for(i=0;i<3;i++){
					if(obj -> shapeNet_result[i] > highest_prob){
						highest_prob = obj -> shapeNet_result[i];
						object_detectedInd = i;
					}
				}
				
				//Object matches user selection
				if(userLabelID == object_detectedInd){
					//Relocate object that was found
					relocate(obj->mid_point,obj->dist2obj,obj->objheight);
					//Free obj before leaving
					free(obj);
					break;
				}
			}
			
			else{
				free(obj);
			}
			//assign obj to objects array
			//objects[curr_objInd] = obj;
			//curr_objInd++;
			
			//printf("----------a + b: %d----------\n",a_size + b_size);
			//printf("----------a : %d----------\n",a_size);
			//printf("----------b: %d----------\n",b);			
			
			//Move back to sweep scan position
			y = STARTINGY;
			z = STARTINGZ;
			coord2pulse(y,z,0,2000);
			servo_command1(BASE_CH,m-10,2000);
			//return 0;
		}
	}

	/*Object Matching*/
	//Loop through scanned and identified objects and see if they match what the user requested
	/*for(m=0;m<curr_objInd;m++){
		//printf("Obj[%d]: %d\n",m,objects[m] -> metaLabel);
		
		if(objects[m] -> metaLabel == 0){
			if(m==0){
				printf("First object was a CUBE\n");
			}
			else if(m==1){
				printf("Second object was a CUBE\n");
			}
			else if(m==2){
				printf("Third object was a CUBE\n");
			}
		}
		else if(objects[m] -> metaLabel == 1){
			if(m==0){
				printf("First object was a CYLINDER\n");
			}
			else if(m==1){
				printf("Second object was a CYLINDER\n");
			}
			else if(m==2){
				printf("Third object was a CYLINDER\n");
			}
		}
		else if(objects[m] -> metaLabel == 2){
			if(m==0){
				printf("First object was a SPHERE\n");
			}
			else if(m==1){
				printf("Second object was a SPHERE\n");
			}
			else if(m==2){
				printf("Third object was a SPHERE\n");
			}
		}
	}*/
	
	/*Object Relocation*/
	/*for(i=0;i<curr_objInd;i++){
		if(objects[i]->metaLabel == userLabelID){
			reloc(objects[i]->mid_point,objects[i]->dist2obj);
		}
	}*/
	
	
	
	return 1;
}//End main


void relocate (int mid_point, int y, int z){
	servo_command1(BASE_CH,mid_point,3000);
	coord2pulse(STARTINGY,STARTINGZ,0,3000);
	servo_command1(CLAW_CH,1,3000);
	
		
	if (mid_point >= 1500 ){
		
		coord2pulse(STARTINGY,z-50,0,RELOCTIME); //
		setvalue(MAGN, 1);	
		coord2pulse(STARTINGY,z-50,-90,RELOCTIME);
		
		coord2pulse(y-5,z-38,-90,RELOCTIME);
		coord2pulse(y-5,z-48,-90,RELOCTIME);
		coord2pulse(y-100,z+50,-90,RELOCTIME);
		//coord2pulse(130,195,-90,RELOCTIME);

	 	servo_command1(BASE_CH,2500,RELOCTIME);

		coord2pulse(120,20,-90,RELOCTIME);
		setvalue(MAGN, 0);
		coord2pulse(120,150,-90,RELOCTIME); 
		servo_command1(BASE_CH,mid_point,RELOCTIME);
		coord2pulse(STARTINGY,STARTINGZ,0,RELOCTIME);

	}
	
	
	if (mid_point < 1500 ){
		coord2pulse(STARTINGY,z-50,0,RELOCTIME); //
		setvalue(MAGN, 1);	
		coord2pulse(STARTINGY,z-50,-90,RELOCTIME);
		
		coord2pulse(y-5,z-38,-90,RELOCTIME);
		coord2pulse(y-5,z-48,-90,RELOCTIME);
		coord2pulse(y-100,z+50,-90,RELOCTIME);
		//coord2pulse(130,195,-90,2000);

	 	servo_command1(BASE_CH,200,RELOCTIME);

		coord2pulse(120,20,-90,RELOCTIME);
		setvalue(MAGN, 0);
		coord2pulse(120,150,-90,RELOCTIME); 
		servo_command1(BASE_CH,mid_point,RELOCTIME);
		coord2pulse(STARTINGY,STARTINGZ,0,RELOCTIME);

	}
	
	return;
}

//Option to train all network
void train_all(void){
	newNet("ShapeNet",SEED,SHAPEINPUT,SHAPEHIDDEN,SHAPEOUTPUT);
	newNet("Side1Net",SEED,SIDE1INPUT,SIDE1HIDDEN,SIDE1OUTPUT);
	newNet("Side2Net",SEED,SIDE2INPUT,SIDE2HIDDEN,SIDE2OUTPUT);

	fullTrain("ShapeNet",1,5,SHAPEINPUT,SHAPEHIDDEN,SHAPEOUTPUT);
	fullTrain("Side1Net",1,10,SIDE1INPUT,SIDE1HIDDEN,SIDE1OUTPUT);
	fullTrain("Side2Net",1,10,SIDE2INPUT,SIDE2HIDDEN,SIDE2OUTPUT);
	return;
}