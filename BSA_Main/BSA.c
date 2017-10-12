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
#define STARTPULSE 1700
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
	
	//INITIAL START POSITION FOR THE BASE (can be adjusted with MACRO above)
	
	double objectStart_xyz[3];
	double objectEnd_xyz[3];
	double *curr_xyz;
	curr_xyz = coord2pulse_XYZ(STARTPULSE,y,z,0,4000);
	
	double scan1[500];
	double scan2[500];
	double scan3[500];
	double scan1R[500];
	double scan2R[500];
	double scan3R[500];
	
	double scan1Filt[500];
	double scan2Filt[500];
	
	int a = 0; //index for saving into scan1 array
	int b,c;
	
	char g; //used for press enter to continue
	
	for(i=0;i<SCAN1BUFFER;i++){
		left_buffer[i] = 9999;
	}
	
	/*The major for-loop for the stage1 scan that goes ~180 deg to scan for objects*/
	//for(i=0;i<700;i=i+1){
	for(m=STARTPULSE;m>ENDPULSE;m--){
		printf("m=%d\n",m);
		
		//STARTPULSE is the constant starting pulse for the base servo. Can be adjusted with MACRO above.
		curr_xyz = coord2pulse_XYZ(m,y,z,0,1);
		
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
			for(j=0;j<(SCAN1BUFFER/2)-1;j++){
				objectStart_xyz[j] = curr_xyz[j];
			}
			
			
		}

		// If object_started is 1 and there are less than 4 valid pings, this means you are no longer looking at object and must now mark
		// object end. object_started is 0 and base pulse is saved in object_end.
		if(left_valid<(SCAN1BUFFER/2) && object_started==1){
		//if(left_valid<(SCAN1BUFFER/2) && right_valid<(SCAN1BUFFER/2) && object_started==1){
			object_started = 0;
			object_end = m-(SCAN1BUFFER/2);
			printf("------------------Object End @ m=%d------------------\n",m-(SCAN1BUFFER/2));
			
			//Save the xyz coordinates of the falling off edge to objectEnd_xyz by copying.
			for(j=0;j<(SCAN1BUFFER/2)-1;j++){
				objectEnd_xyz[j] = curr_xyz[j];
			}
			
			
			//Create object struct
			Object *obj = (Object *)malloc(sizeof(Object));
			
			int mid_point = ((object_end - object_start) / 2) + object_start;
			obj -> mid_point = mid_point;
			
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
			//int scan2_valid = SCAN2BUFFER;
			
			
			//STEPHANES FUNCTION FOR TESTING ONLY		
			while(b<50){
				y_ping = LgetCM(PINGCOUNT) * 10;
				scan2[b] = y_ping;
				coord2pulse(y,z++,0,1);
				printf("%lf\n",scan2[b]);
				b++;
			}
			
			
			
	/*		
			while((scan2_valid > 0) && (flag == 0)){
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
	*/
	
	
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
			
			//copy result to obj struct
			for(i=0;i<SHAPEOUTPUT;i++){
				obj -> shapeNet_result[i] = result3[i];
				printf("shapeNet[%d]: %lf\n",i,obj -> shapeNet_result[i]);
			}
			
			return 0;
			
			//Subtract out of bound pings from buffer checks
			b = b - SCAN2BUFFER;
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			double z_ping = 0;
			double z_pingR = 0;
			coord2pulse(y,z,-90,2000);
			
			c = 0;
			//z += 20; //Testing higher height
			y += DIST2OBJ; //move to close in gap
			coord2pulse(y,z,-90,2000);
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			flag = 0; //reset flag for next scan
			int scan3_valid = SCAN3BUFFER;
			while((scan3_valid > 0) && (flag == 0)){
				z_ping = LgetCM(PINGCOUNT) * 10;
				
				if(z_ping > z){
					scan3_valid--;
				}
				
				scan3[c] = z_ping;
				
				z_pingR = RgetCM(PINGCOUNT) * 10;
				scan3R[c] = z_pingR;
				
				flag = coord2pulse((y++),z,-90,1);
				c++;
			}
			
			if(flag==1){
				printf("Arm reached limit!\n");
			}
			
			//Subtract out of bound pings from buffer checks
			c = c - SCAN3BUFFER;
			
			//Move the claw back (y-axis)
			y = STARTINGY;
			coord2pulse(y,z+30,-90,2000);
			//Move claw facing forward
			coord2pulse(y,z,0,2000);
			//Move the claw down (z-axis)
			z = STARTINGZ;
			coord2pulse(y,z,0,2000);
			curr_xyz = coord2pulse_XYZ( m,y,z,0,1000 );
			
			for(i=0;i<a;i++){
				printf("scan1[%d]: %lf\n",i,scan1[i]);
			}
			
			//Write raw scan1 data to file
			//writeRawScan("Scan1",scan1,a);
			//writeRawScan("Scan1R",scan1R,a);
			
			/*for(i=0;i<a;i++){
				printf("scan1R[%d]: %lf\n",i,scan1R[i]);
			}*/
			
			for(i=0;i<b;i++){
				printf("scan2[%d]: %lf\n",i,scan2[i]);
			}
			
			writeRawScan("Scan2",scan2,b);
			
			/*for(i=0;i<b;i++){
				printf("scan2R[%d]: %lf\n",i,scan2R[i]);
			}*/
			
			for(i=0;i<c;i++){
				printf("scan3[%d]: %lf\n",i,scan3[i]);
			}
			
			writeRawScan("Scan3",scan3,c);
			
			/*for(i=0;i<c;i++){
				printf("scan3R[%d]: %lf\n",i,scan3R[i]);
			}*/
			
			double scan1_hold[SIDE1INPUT];
			double scan2_hold[SIDE2INPUT];
			double scan3_hold[SIDE3INPUT];
			double scan1R_hold[SIDE1INPUT];
			double scan2R_hold[SIDE2INPUT];
			double scan3R_hold[SIDE3INPUT];
			
			//Scan 1 ends before SCAN1TRIM to take off extra at the end for left sensor falling off edge
			// while right one still pings
			int trueSize = a;
			int trim = a/10; //trim 10% of pings off both sides
			int inputNodes = SIDE1INPUT;
			
			int listStart = trim/2;
			int listEnd = (trueSize - (trim/2) - SCAN1TRIM);
			
			int newSize = listEnd - listStart;
			int half = newSize / 2;
			int step = newSize / inputNodes;
			
			int k = 0;
			for(i=listStart;i<=listEnd;i=i+step){
				printf("scan1[%d]_%d: %lf\n",i,SIDE1INPUT,scan1[i]);
				scan1_hold[k] = scan1[i];
				scan1R_hold[k] = scan1R[i];
				k++;
			}
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			for(i=0;i<SIDE1INPUT;i++){
				printf("scan1R[%d]_%d: %lf\n",i,SIDE1INPUT,scan1R_hold[i]);
			}
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			
			trueSize = b;
			trim = b/10;
			inputNodes = SIDE2INPUT;
			
			newSize = trueSize - trim;
			step = newSize / inputNodes;
			half = newSize / 2;
			listStart = half - (step * (inputNodes/2)) + (trim/2);
			listEnd = listStart + (step * (inputNodes-1));
			
			k = 0;
			for(i=listStart;i<=listEnd;i=i+step){
				printf("scan2[%d]_%d: %lf\n",i,SIDE2INPUT,scan2[i]);
				scan2_hold[k] = scan2[i];
				scan2R_hold[k] = scan2R[i];
				k++;
			}
			
			for(i=0;i<SIDE2INPUT;i++){
				printf("scan2R[%d]_%d: %lf\n",i,SIDE2INPUT,scan2R_hold[i]);
			}
			
			printf("\nPress Enter To Continue \n");
			scanf("%c",&g);
			

			trueSize = c;
			trim = c/10;
			inputNodes = SIDE3INPUT;
			
			newSize = trueSize - trim;
			step = newSize / inputNodes;
			half = newSize / 2;
			listStart = half - (step * (inputNodes/2)) + (trim/2);
			listEnd = listStart + (step * (inputNodes-1));
			
			k = 0;
			for(i=listStart;i<=listEnd;i=i+step){
				printf("scan3[%d]_%d: %lf\n",i,SIDE3INPUT,scan3[i]);
				scan3_hold[k] = scan3[i];
				scan3R_hold[k] = scan3R[i];
				k++;
			}
			
			for(i=0;i<SIDE3INPUT;i++){
				printf("scan3R[%d]_%d: %lf\n",i,SIDE3INPUT,scan3R_hold[i]);
			}
			
			
			double scan1_deriv[SIDE1INPUT];
			double scan2_deriv[SIDE2INPUT];
			double scan3_deriv[SIDE3INPUT];
			double scan1R_deriv[SIDE1INPUT];
			double scan2R_deriv[SIDE2INPUT];
			double scan3R_deriv[SIDE3INPUT];
			

			for(i=0;i<SIDE1INPUT-1;i++){
				scan1_deriv[i] = scan1_hold[i+1] - scan1_hold[i];
				if(scan1_deriv[i] == 0.0){
					scan1_deriv[i] = 0.01;
				}
				printf("scan1_deriv[%d]: %lf\n",i,scan1_deriv[i]);
			}
			scan1_deriv[SIDE1INPUT-1] = scan1_deriv[SIDE1INPUT-2];
			printf("scan1_deriv[%d]: %lf\n",SIDE1INPUT-1,scan1_deriv[SIDE1INPUT-1]);
			printf("\n");
			
			writeRawScan("Scan1Deriv",scan1_deriv,SIDE1INPUT);

			
			for(i=0;i<SIDE1INPUT-1;i++){
				scan1R_deriv[i] = scan1R_hold[i+1] - scan1R_hold[i];
				if(scan1R_deriv[i] == 0.0){
					scan1R_deriv[i] = 0.01;
				}
				//printf("scan1R_deriv[%d]: %lf\n",i,scan1R_deriv[i]);
			}
			scan1R_deriv[SIDE1INPUT-1] = scan1R_deriv[SIDE1INPUT-2];
			//printf("scan1R_deriv[%d]: %lf\n",SIDE1INPUT-1,scan1R_deriv[SIDE1INPUT-1]);
			printf("\n");
			
			for(i=0;i<SIDE2INPUT-1;i++){
				scan2_deriv[i] = scan2_hold[i+1] - scan2_hold[i];
				if(scan2_deriv[i] == 0.0){
					scan2_deriv[i] = 0.01;
				}
				printf("scan2_deriv[%d]: %lf\n",i,scan2_deriv[i]);
			}
			scan2_deriv[SIDE2INPUT-1] = scan2_deriv[SIDE2INPUT-2];
			printf("scan2_deriv[%d]: %lf\n",SIDE2INPUT-1,scan2_deriv[SIDE2INPUT-1]);
			printf("\n");
			
			
			for(i=0;i<SIDE2INPUT-1;i++){
				scan2R_deriv[i] = scan2R_hold[i+1] - scan2R_hold[i];
				if(scan2R_deriv[i] == 0.0){
					scan2R_deriv[i] = 0.01;
				}
				//printf("scan2R_deriv[%d]: %lf\n",i,scan2R_deriv[i]);
			}
			scan2R_deriv[SIDE2INPUT-1] = scan2R_deriv[SIDE2INPUT-2];
			//printf("scan2R_deriv[%d]: %lf\n",SIDE2INPUT-1,scan2R_deriv[SIDE2INPUT-1]);
			printf("\n");
			
			for(i=0;i<SIDE3INPUT-1;i++){
				scan3_deriv[i] = scan3_hold[i+1] - scan3_hold[i];
				if(scan3_deriv[i] == 0.0){
					scan3_deriv[i] = 0.01;
				}
				printf("scan3_deriv[%d]: %lf\n",i,scan3_deriv[i]);
			}
			scan3_deriv[SIDE3INPUT-1] = scan3_deriv[SIDE3INPUT-2];
			printf("scan3_deriv[%d]: %lf\n",SIDE3INPUT-1,scan2_deriv[SIDE3INPUT-1]);
			printf("\n");
			
			
			for(i=0;i<SIDE3INPUT-1;i++){
				scan3R_deriv[i] = scan3R_hold[i+1] - scan3R_hold[i];
				if(scan3R_deriv[i] == 0.0){
					scan3R_deriv[i] = 0.01;
				}
				//printf("scan3R_deriv[%d]: %lf\n",i,scan3R_deriv[i]);
			}
			scan3R_deriv[SIDE3INPUT-1] = scan3R_deriv[SIDE3INPUT-2];
			//printf("scan3R_deriv[%d]: %lf\n",SIDE3INPUT-1,scan2R_deriv[SIDE3INPUT-1]);
			printf("\n");
			
			
			//Check if user wishes to write TrainData to File
			printf("\nDo you wish to write this training session to file? \n");
			scanf("%c",&g);
			
			//Only write to file if user says yes
			if((g == 'y') || (g == 'Y')){
				writeTrainInput("Side1Net",scan1_deriv,SIDE1INPUT);
				writeTrainInput("Side2Net",scan2_deriv,SIDE2INPUT);
				writeTrainInput("Side3Net",scan3_deriv,SIDE3INPUT);
			}
			
			
			/*Propagate all 3 scans through SideNet and copy to obj*/
			double *result = computeSide1("Side1Net",scan1_deriv);
			for(i=0;i<SIDE1OUTPUT;i++){
				obj -> side1Net_result[i] = result[i];
				printf("sideNet1[%d]: %lf\n",i,obj -> side1Net_result[i]);
			}
			printf("\n");
			
			result = computeSide2("Side2Net",scan2_deriv);
			for(i=0;i<SIDE2OUTPUT;i++){
				obj -> side2Net_result[i] = result[i];
				printf("sideNet2[%d]: %lf\n",i,obj -> side2Net_result[i]);
			}
			printf("\n");
			
			/*result = computeSide3("Side3Net",scan3_deriv);
			for(i=0;i<SIDE3OUTPUT;i++){
				obj -> side3Net_result[i] = result[i];
				printf("sideNet3[%d]: %lf\n",i,obj -> side3Net_result[i]);
			}*/
			printf("\n");
			printf("\n");
			
			
			/*Assemble input for ShapeNet*/
			double shapeNet_In[SHAPEINPUT];
			k=0;
			for(i=0;i<SIDE1OUTPUT;i++){
				shapeNet_In[k] = obj -> side1Net_result[i];
				k++;
			}
			
			for(i=0;i<SIDE2OUTPUT;i++){
				shapeNet_In[k] = obj -> side2Net_result[i];
				k++;
			}
			
			/*for(i=0;i<SIDE3OUTPUT;i++){
				shapeNet_In[k] = obj -> side3Net_result[i];
				k++;
			}*/
			
			/*Propagate input through ShapeNet and copy to obj*/
			result = computeShape("ShapeNet",shapeNet_In);
			for(i=0;i<SHAPEOUTPUT;i++){
				obj -> shapeNet_result[i] = result[i];
				printf("shapeNet[%d]: %lf\n",i,obj -> shapeNet_result[i]);
			}
			
			//Slowly reset back to sweep scanning mode
			coord2pulse(m,z,0,3000);
			
			/*KILL PROGRAM AFTER 1 OBJECT SCAN*/
			return 1;
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
	
	
	
	
	
	return 1;
}//End main
