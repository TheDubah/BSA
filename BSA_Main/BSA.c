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
#include "regression.h" //-> Used for polynomial regression

#define PINGCOUNT 1
#define SCAN1BUFFER 10
#define SCAN2BUFFER 5
#define SCAN3BUFFER 5
#define BORDER 200
#define STARTBASE 1700
#define ENDPULSE 1100
#define STARTINGY 130
#define STARTINGZ 25
#define DIST2OBJ 70
#define BIGDELTA 1000
#define SCALEFACTOR 10000

#define COEFFICIENTS 3

#define SCAN1TRIM 100

//Neural Network Macros
#define SEED 900



//main program/operation flow
int main(void){
	
	//newNet("ShapeNet",SEED,SHAPEINPUT,SHAPEHIDDEN,SHAPEOUTPUT);
	//newNet("Side1Net",SEED,SIDE1INPUT,SIDE1HIDDEN,SIDE1OUTPUT);
	//newNet("Side2Net",SEED,SIDE2INPUT,SIDE2HIDDEN,SIDE2OUTPUT);
	//newNet("Side3Net",SEED,SIDE3INPUT,SIDE3HIDDEN,SIDE3OUTPUT);

	//fullTrain("ShapeNet",1,5,SHAPEINPUT,SHAPEHIDDEN,SHAPEOUTPUT);
	//fullTrain("Side1Net",1,10,SIDE1INPUT,SIDE1HIDDEN,SIDE1OUTPUT);
	//fullTrain("Side2Net",1,10,SIDE2INPUT,SIDE2HIDDEN,SIDE2OUTPUT);
	//fullTrain("Side3Net",1,10,SIDE2INPUT,SIDE3HIDDEN,SIDE3OUTPUT);
	//return 1;

	/*double input1[9] = {1.0, 0.01, 0.01, 1.0, 0.01, 0.01, 1.0, 0.01, 0.01};
	double input2[9] = {0.01, 1.00, 0.01, 1.00, 0.01, 0.01, 1.00, 0.01, 0.01};
	
	double *result1,*result2;
	result1 = computeShape("ShapeNet",input1);
	result2 = computeShape("ShapeNet",input2);
	int l;
	for(l=0;l<SHAPEOUTPUT;l++){
		printf("%d: %lf\n",l,result1[l]);
	}
	
	for(l=0;l<SHAPEOUTPUT;l++){
		printf("%d: %lf\n",l,result2[l]);
	}
	
	return 1;*/
	
	//For sensors setup
	setup();
	
	//Starting y-z position (can be changed)
	int y = STARTINGY;
	int z = STARTINGZ;
	coord2pulse(y,z,0,4000);
	
	//Variables used later, names give some idea but read further
	int i,j,m;
	double left_ping;//, right_ping;
	double left_buffer[SCAN1BUFFER];//, right_buffer[SCAN1BUFFER]
	int left_valid;//, right_valid;
	int object_started = 0;
	int object_start, object_end;
	
	
	//Smooth start
	servo_command1(BASE_CH,STARTBASE,4000);
	coord2pulse(y,z,0,4000);
	
	double scan1[500];
	double scan2[500];
	//double scan1R[500];
	//double scan2R[500];
	
	double scan1Filt[500];
	double scan2Filt[500];
	
	int a = 0; //index for saving into scan1 array
	int b;
	
	//object struct array to hold objects
	int curr_objInd = 0;
	Object *objects[3];
	
	
	//initialize buffer with values that don't false trigger object start
	for(i=0;i<SCAN1BUFFER;i++){
		left_buffer[i] = 9999;
	}
	
	/*The major for-loop for the stage1 scan that goes ~180 deg to scan for objects*/
	//for(i=0;i<700;i=i+1){
	for(m=STARTBASE;m>ENDPULSE;m--){
		printf("m=%d\n",m);
		
		//STARTBASE is the constant starting pulse for the base servo. Can be adjusted with MACRO above.
		servo_command1(BASE_CH,m,1);
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
		if(object_started == 1){
			scan1[a] = left_ping;
			//scan1R[a] = right_ping;
			printf("%d: %lf\n",a,scan1[a]);
			a++;
		}
		
		//These two if statements are used to look back at array in this iteration of loop to see if we have enough valid pings (4) to safely
		// mark an edge of an object. If object_started is 0 (initial state) then it is marked with 1 and the object_start base pulse is saved.
		if(left_valid>=(SCAN1BUFFER/2) && object_started==0){
		//if(left_valid>=(SCAN1BUFFER/2) && right_valid>=(SCAN1BUFFER/2) && object_started==0){	
			object_started = 1;
			object_start = m-(SCAN1BUFFER/2);
			printf("------------------Object Start @ m=%d------------------\n",m-(SCAN1BUFFER/2));
			
			//Save the xyz coordinates of the starting edge to objectStart_xyz by copying.
			/*for(j=0;j<(SCAN1BUFFER/2)-1;j++){
				objectStart_xyz[j] = curr_xyz[j];
			}*/
			
			
		}

		// If object_started is 1 and there are less than 4 valid pings, this means you are no longer looking at object and must now mark
		// object end. object_started is 0 and base pulse is saved in object_end.
		if(left_valid<(SCAN1BUFFER/2) && object_started==1){
		//if(left_valid<(SCAN1BUFFER/2) && right_valid<(SCAN1BUFFER/2) && object_started==1){
			object_started = 0;
			object_end = m-(SCAN1BUFFER/2);
			printf("------------------Object End @ m=%d------------------\n",m-(SCAN1BUFFER/2));
			
			//Save the xyz coordinates of the falling off edge to objectEnd_xyz by copying.
			/*for(j=0;j<(SCAN1BUFFER/2)-1;j++){
				objectEnd_xyz[j] = curr_xyz[j];
			}*/
			
			
			//Create object struct
			Object *obj = (Object *)malloc(sizeof(Object));
			
			int mid_point = ((object_end - object_start) / 2) + object_start;
			obj -> mid_point = mid_point;
			
			//Move to midpoint
			servo_command1(BASE_CH,mid_point,2000);
			
			char side4Log[20];
			printf("\nPlease enter side for logs \n");
			fgets(side4Log, 20, stdin);
			
			
			//FILTERING
			int unique[50];
			int isUnique;
			int mode = 0;
			int max_repeat = 0;
			int repeat = 0;
			int uIndex = 0;
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
			int scan1FiltMax = 0;
			for(i=0;i<a;i++){
				if( (scan1[i] < (mode + 50)) && (scan1[i] > (mode - 50)) ){
					scan1Filt[scan1FiltMax] = scan1[i];
					scan1FiltMax++;
				}
			}
			
			
			writeRawScan("Scan1",scan1Filt,scan1FiltMax);
			writeRawLog("Scan1_Log",scan1,a,side4Log);
			writeRawLog("Scan1_Log",scan1Filt,scan1FiltMax,side4Log);
			
			/*CURVE FITTING*/
			regression(COEFFICIENTS, scan1Filt,scan1FiltMax,obj);
			
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
			
			int deltaStart = (scan1FiltMax/2);
			
			//Calculate deltas for input into side network
			double scan1Delta[SIDE1INPUT];
			int deltaIndex = 0;
			int trueDelta = BIGDELTA/SIDE1INPUT;
			//for(i=vertStart;i<vertStart+101;i++){
			for(i=deltaStart;i<deltaStart+BIGDELTA;i=i+trueDelta){
				//delta = [i+delta] - [i]
				scan1Delta[deltaIndex] = (((obj -> coeff[2])*pow(i+trueDelta,2)) + ((obj -> coeff[1])*pow(i+trueDelta,1)) + (obj -> coeff[0])) - (((obj -> coeff[2])*pow(i,2)) + ((obj -> coeff[1])*pow(i,1)) + (obj -> coeff[0]));
				printf("Delta: %lf\n", scan1Delta[deltaIndex]);
				deltaIndex++;
			}
			
			double scan1DeltaScaled[SIDE1INPUT];
			for(i=0;i<SIDE1INPUT;i++){
				scan1DeltaScaled[i] = scan1Delta[i] / SCALEFACTOR;
			}
			
			//writeTrainInput("Side1Net",scan1DeltaScaled,SIDE1INPUT);
			
			writeRawLog("Scan1Inputs_Log",scan1Delta,SIDE1INPUT,side4Log);
			
			double *result1;
			result1 = computeSide1("Side1Net",scan1DeltaScaled);
			for(i=0;i<SIDE1OUTPUT;i++){
				obj -> side1Net_result[i] = result1[i];
				//printf("sideNet1[%d]: %lf\n",i,obj -> side1Net_result[i]);
			}
			
			printf("Side1 FLAT: %lf\n",result1[0]);
			printf("Side1 CURVE: %lf\n",result1[1]);

			
			/*Get close to object within certain distance*/
			double y_ping = LgetCM(PINGCOUNT) * 10; //to get mm from cm ping
			//double y_pingR = RgetCM(PINGCOUNT) * 10;
			while(y_ping > DIST2OBJ){
				y_ping = LgetCM(PINGCOUNT) * 10; //to get mm for cm
				printf("Y_PING: %lf\n",y_ping);
				y++;
				coord2pulse(y,z,0,1);
			}
			
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
			int scan2_valid = SCAN2BUFFER;
			while(((scan2_valid > 0) && (flag == 0)) || (b<20) ){
				y_ping = LgetCM(PINGCOUNT) * 10;
				
				scan2[b] = y_ping;
				
				if(y_ping > BORDER){
					scan2_valid--;
				}
				
				//y_pingR = RgetCM(PINGCOUNT) * 10;
				//scan2R[b] = y_pingR;
				
				flag = coord2pulse(y,z++,0,1);
				b++;
			}
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
			int scan2FiltMax = 0;
			for(i=0;i<b;i++){
				if( (scan2[i] < (side2_mode + 20)) && (scan2[i] > (side2_mode - 20)) ){
					scan2Filt[scan2FiltMax] = scan2[i];
					scan2FiltMax++;
				}
			}
			
			for(i=0;i<scan2FiltMax;i++){
				printf("FILTERED: %lf\n",scan2Filt[i]);
			}
			
			regression(COEFFICIENTS, scan2Filt,scan2FiltMax,obj);
			
			deltaStart = (scan2FiltMax/2);
			
			//Calculate deltas for input into side network
			double scan2Delta[SIDE2INPUT];
			deltaIndex = 0;
			trueDelta = BIGDELTA/SIDE2INPUT;
			//for(i=vertStart;i<vertStart+101;i++){
			for(i=deltaStart;i<deltaStart+BIGDELTA;i=i+trueDelta){
				scan2Delta[deltaIndex] = (((obj -> coeff[2])*pow(i+trueDelta,2)) + ((obj -> coeff[1])*pow(i+trueDelta,1)) + (obj -> coeff[0])) - (((obj -> coeff[2])*pow(i,2)) + ((obj -> coeff[1])*pow(i,1)) + (obj -> coeff[0]));
				printf("Delta: %lf\n", scan2Delta[deltaIndex]);
				deltaIndex++;
			}
			
			double scan2DeltaScaled[SIDE2INPUT];
			for(i=0;i<SIDE2INPUT;i++){
				scan2DeltaScaled[i] = scan2Delta[i] / SCALEFACTOR;
			}
			
			writeRawScan("Scan2",scan2Filt,scan2FiltMax);
			writeRawLog("Scan2_Log",scan2,b,side4Log);
			//writeTrainInput("Side2Net",scan2DeltaScaled,SIDE2INPUT);
			
			double *result2;
			result2 = computeSide2("Side2Net",scan2DeltaScaled);
			for(i=0;i<SIDE2OUTPUT;i++){
				obj -> side2Net_result[i] = result2[i];
				//printf("sideNet2[%d]: %lf\n",i,obj -> side2Net_result[i]);
			}
			
			
			printf("Side2 FLAT: %lf\n",result2[0]);
			printf("Side2 CURVE: %lf\n",result2[1]);
			
			
			//Identifying shape
			double shapeInput[SHAPEINPUT];
			shapeInput[0] = obj -> side1Net_result[0];
			shapeInput[1] = obj -> side1Net_result[1];
			shapeInput[2] = obj -> side2Net_result[0];
			shapeInput[3] = obj -> side2Net_result[1];
			
			double *result3;
			result3 = computeShape("ShapeNet",shapeInput);
			
			//copy result to obj struct and determine max probability for shape index
			double maxShapeProb = 0;
			for(i=0;i<SHAPEOUTPUT;i++){
				obj -> shapeNet_result[i] = result3[i];
				printf("shapeNet[%d]: %lf\n",i,obj -> shapeNet_result[i]);
				
				if(obj -> shapeNet_result[i] > maxShapeProb){
					maxShapeProb = obj -> shapeNet_result[i];
					obj -> metaLabel = i;
				}
			}
			
			//assign obj to objects array
			objects[curr_objInd] = obj;
			curr_objInd++;
			
			return 0;
		}
		
	}
	
	//Print the xyz coordinates for the start and stop of object in scan1
	//printf("--Object Start XYZ--\n");
	//printf("X: %lf\n",objectStart_xyz[0]);
	//printf("Y: %lf\n",objectStart_xyz[1]);
	//printf("Z: %lf\n",objectStart_xyz[2]);
	
	//printf("--Object End XYZ--\n");
	//printf("X: %lf\n",objectEnd_xyz[0]);
	//printf("Y: %lf\n",objectEnd_xyz[1]);
	//printf("Z: %lf\n",objectEnd_xyz[2]);
	
	
	
	
	
	return 1;
}//End main
