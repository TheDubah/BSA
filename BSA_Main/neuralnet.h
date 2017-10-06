#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <math.h>
#include <time.h>

#define SIDE1INPUT 10
#define SIDE1HIDDEN 500
#define SIDE1OUTPUT 2

#define SIDE2INPUT 20
#define SIDE2HIDDEN 12
#define SIDE2OUTPUT 2

#define SIDE3INPUT 20
#define SIDE3HIDDEN 12
#define SIDE3OUTPUT 2

#define SHAPEINPUT 6
#define SHAPEHIDDEN 6
#define SHAPEOUTPUT 3


/*These variables are part of all function parameters and are no longer MACROS in this file:
	inputNodes = number of nodes in input layer of network (based on number of inputs)
	hiddenNodes = number of nodes in hidden layer of network
	outputNodes = number of nodes in output layer of network
	
	MACROS were abandoned to accomodate different size networks and full integration with BSA
	project code*/

//Activation function for passing on
double sigmoid(double x){
	return (1/(1+exp(-x)));
}


/*Initialilize new network with new empty training file. WARNING: THIS WILL DELETE ANY EXISTING TRAINING FILE
	WITH NETWORKID NAME. Make sure to run the first time on its own to create the weight files in proper folders.
	Folders 'Weights' and 'Training' need to be present in current directory to hold files that will be created.
	NetworkID is the name of this Neural Network. Seed is used to randomly initialize weights.*/
void newNetComplete(char *networkID, int seed, int inputNodes, int hiddenNodes, int outputNodes){
	//create new net and file
	FILE *wihFile,*whoFile,*trainFile;
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	
	/*INPUT TO HIDDEN WEIGHTS*/
	//create file with NetworkID name
	wihFile = fopen(inputFileName,"w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	//initialilize with seed
	srand(seed);
	
	int i,j;
	double random;
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			//use random num for seed for next iteration
			srand(rand());
			//write random values to weight file
			random = ((((double)rand()) / RAND_MAX) * 2) - 1;
			fprintf(wihFile, "%.8f ", random);
		}
		fprintf(wihFile, "\n");
	}
	
	//close input to hidden weight file
	fclose(wihFile);
	
	
	/*HIDDEN TO OUTPUT WEIGHTS*/
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	//create file with NetworkID name
	whoFile = fopen(outputFileName,"w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	//same init for w_ho as w_ih
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			//use random num for seed for next iteration
			srand(rand());
			//write to file
			random = ((((double)rand()) / RAND_MAX) * 2) - 1;
			fprintf(whoFile, "%.8f ", random);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	
	/*Create training file*/
	/*WARNING: THIS WILL OVERWRITE TRAINING DATA*/
	char trainFileName[50] = "Training/train_";
	strcat(trainFileName,networkID);
	strcat(trainFileName,".txt");
	
	trainFile = fopen(trainFileName,"w");
	if(trainFile == NULL){
		fprintf(stderr, "Error opening TRAIN file\n");
		return;
	}
	
	fclose(trainFile);
	
	return;
}

/*newNet() only accesses the weight files and not the training files therefore can be used to reinitalize to 
	random weights without tampering training file. This can be used multiple times.*/

void newNet(char *networkID, int seed, int inputNodes, int hiddenNodes, int outputNodes){
	//create new net and file
	FILE *wihFile,*whoFile;
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	//initialilize with seed
	srand(seed);
	
	int i,j;
	double random;
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			//use random num for seed for next iteration
			srand(rand());
			//write to file
			random = ((((double)rand()) / RAND_MAX) * 2) - 1;
			fprintf(wihFile, "%.8f ", random);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	//same init for w_ho
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			//use random num for seed for next iteration
			srand(rand());
			//write to file
			random = ((((double)rand()) / RAND_MAX) * 2) - 1;
			fprintf(whoFile, "%.8f ", random);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	return;
}

/*deleteNet deletes all files in Weights and Training under 'networkID' name including training files.
	Should only be used for housekeeping.*/
void deleteNet(char *networkID){
	
	int deleteStat;
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	deleteStat = remove(inputFileName);
	if(deleteStat != 0){
		printf("Error: unable to delete wihFile\n");
		return;
	}
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	deleteStat = remove(outputFileName);
	if(deleteStat != 0){
		printf("Error: unable to delete whoFile\n");
		return;
	}
	
	char trainFileName[50] = "Training/train_";
	strcat(trainFileName,networkID);
	strcat(trainFileName,".txt");
	
	deleteStat = remove(trainFileName);
	if(deleteStat != 0){
		printf("Error: unable to delete trainFile\n");
		return;
	}
	
	return;
}


/*Basic train function that trains using 'networkID' weights and 'networkID' training file.
	learning_rate used to multiply how fast the learning occurs. Recommended setting is 1
	and anything above will result in faster learning but with more nuanced training data
	this can lead to inaccurate gradient descent (error keeps bouncing between negative x
	positive x around the correct answer).
	
	Training is done by doing all of the forwared feeding computation with current network
	weights (EXACTLY AS DONE IN computeShape) and then taking error from expected output obtained
	from training file to backpropagate errors to network's nodes via weight contribution to network.
	
	This funciton also iterates over a complete training file once.*/
void train(char *networkID, double learning_rate, int inputNodes, int hiddenNodes, int outputNodes){
		
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[hiddenNodes][inputNodes];
	double w_ho[outputNodes][hiddenNodes];

	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	int i,j,k;
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TRAIN WEIGHTS WITH TRAINING DATA*/
	FILE *train;
	
	char trainFileName[50] = "Training/train_";
	strcat(trainFileName,networkID);
	strcat(trainFileName,".txt");
	
	train = fopen(trainFileName,"r");
	if(train == NULL){
		fprintf(stderr, "Error opening TRAIN file\n");
		return;
	}
	
	
	double inputs[inputNodes][1];
	double expected_out[outputNodes][1];
	
	while(fscanf(train,"%lf",&inputs[0][0]) != EOF){
		//grab inputs into input array
		for(i=1;i<inputNodes;i++){
			fscanf(train,"%lf",&inputs[i][0]);
		}
		
		//grab expected outputs into array
		for(i=0;i<outputNodes;i++){
			fscanf(train,"%lf",&expected_out[i][0]);
		}
		
		
		/** BEGIN COMPUTE **/
		/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
		double hidden_inputs[hiddenNodes][1];
		double dotsum;
		
		for(i=0;i<hiddenNodes;i++){
			dotsum = 0.0;
			for(k=0;k<inputNodes;k++){
				dotsum += w_ih[i][k] * inputs[k][0];
			}
			hidden_inputs[i][0] = dotsum;
		}
		

		/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
		double hidden_output[hiddenNodes][1];
		
		for(i=0;i<hiddenNodes;i++){
			hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
		}
		
		
		/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
		double output_in[outputNodes][1];
		
		for(i=0;i<outputNodes;i++){
			dotsum = 0.0;
			for(k=0;k<hiddenNodes;k++){
				dotsum += w_ho[i][k] * hidden_output[k][0];
			}
			output_in[i][0] = dotsum;
		}
		
		
		/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
		double final_output[outputNodes][1];
		
		for(i=0;i<outputNodes;i++){
			final_output[i][0] = sigmoid(output_in[i][0]);
		}
		/** END COMPUTE **/
		
		
		/*CALCULATE OUTPUT ERROR WITH TRAINING DATA*/
		double output_error[outputNodes][1];
		
		printf("NETWORK ERROR\n");
		for(i=0;i<outputNodes;i++){
			output_error[i][0] = expected_out[i][0] - final_output[i][0];
			
			printf("%lf %%\n",(output_error[i][0] / expected_out[i][0])*100);
		}
		printf("\n");
		
		/*CALCULATE HIDDEN ERROR SPLIT BY WEIGHTS*/
		//transpose of w_ho
		double w_ho_T[hiddenNodes][outputNodes];
		for(i=0;i<outputNodes;i++){
			for(j=0;j<hiddenNodes;j++){
				w_ho_T[j][i] = w_ho[i][j];
			}
		}
		
		//calculate dot product with transpose w_ho and output_error
		double hidden_errors[hiddenNodes][1];
		
		for(i=0;i<hiddenNodes;i++){
			dotsum = 0.0;
			for(k=0;k<outputNodes;k++){
				dotsum += w_ho_T[i][k] * output_error[k][0];
			}
			hidden_errors[i][0] = dotsum;
		}
		
		
		/*ADJUST WEIGHTS WITH ERRORS*/
		//This should be referenced in text for more details as this is the key for the neural network.
		for(i=0;i<outputNodes;i++){
			for(j=0;j<hiddenNodes;j++){
				w_ho[i][j] += learning_rate * (output_error[i][0] * sigmoid(final_output[i][0]) * (1 - sigmoid(final_output[i][0]))) * hidden_output[j][0];
			}
		}
		
		//w_ih adjustment
		for(i=0;i<hiddenNodes;i++){
			for(j=0;j<inputNodes;j++){
				w_ih[i][j] += learning_rate * (hidden_errors[i][0] * sigmoid(hidden_output[i][0]) * (1 - sigmoid(hidden_output[i][0]))) * inputs[j][0];
			}
		}
		
	}
	
	/*SAVE WEIGHTS BACK TO FILE WITH .8lf*/
	wihFile = fopen(inputFileName,"w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	//save new weights to wihFile
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			fprintf(wihFile, "%.8f ", w_ih[i][j]);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	whoFile = fopen(outputFileName,"w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	//save new weights to whoFile
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			fprintf(whoFile, "%.8f ", w_ho[i][j]);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	return;
}

/*Same as train() function but returns the highest error in percent from one iteration over the training file.*/
double train_err(char *networkID, double learning_rate, int inputNodes, int hiddenNodes, int outputNodes){
		
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[hiddenNodes][inputNodes];
	double w_ho[outputNodes][hiddenNodes];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return -99.9;
	}
	
	int i,j,k;
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return -99.9;
	}
	
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TRAIN WEIGHTS WITH TRAINING DATA*/
	FILE *train;
	
	char trainFileName[50] = "Training/train_";
	strcat(trainFileName,networkID);
	strcat(trainFileName,".txt");
	
	train = fopen(trainFileName,"r");
	if(train == NULL){
		fprintf(stderr, "Error opening TRAIN file\n");
		return -99.9;
	}
	
	
	double inputs[inputNodes][1];
	double expected_out[outputNodes][1];
	
	double highest_error = 0;
	double abs_error;
	double error_percent;
	
	while(fscanf(train,"%lf",&inputs[0][0]) != EOF){
		//grab inputs into input array
		for(i=1;i<inputNodes;i++){
			fscanf(train,"%lf",&inputs[i][0]);
		}
		
		//grab expected outputs into array
		for(i=0;i<outputNodes;i++){
			fscanf(train,"%lf",&expected_out[i][0]);
		}
		
		
		/** BEGIN COMPUTE **/
		/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
		double hidden_inputs[hiddenNodes][1];
		double dotsum;
		
		for(i=0;i<hiddenNodes;i++){
			dotsum = 0.0;
			for(k=0;k<inputNodes;k++){
				dotsum += w_ih[i][k] * inputs[k][0];
			}
			hidden_inputs[i][0] = dotsum;
		}
		

		/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
		double hidden_output[hiddenNodes][1];
		
		for(i=0;i<hiddenNodes;i++){
			hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
		}
		
		
		/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
		double output_in[outputNodes][1];
		
		for(i=0;i<outputNodes;i++){
			dotsum = 0.0;
			for(k=0;k<hiddenNodes;k++){
				dotsum += w_ho[i][k] * hidden_output[k][0];
			}
			output_in[i][0] = dotsum;
		}
		
		
		/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
		double final_output[outputNodes][1];
		
		for(i=0;i<outputNodes;i++){
			final_output[i][0] = sigmoid(output_in[i][0]);
		}
		/** END COMPUTE **/
		
		
		/*CALCULATE OUTPUT ERROR WITH TRAINING DATA*/
		double output_error[outputNodes][1];
		
		printf("NETWORK ERROR\n");
		for(i=0;i<outputNodes;i++){
			output_error[i][0] = expected_out[i][0] - final_output[i][0];
			
			error_percent = (output_error[i][0] / expected_out[i][0])*100;
			
			if(error_percent < 0){
				abs_error = -error_percent;
			}
			else{
				abs_error = error_percent;
			}
			
			if(highest_error < abs_error){
				highest_error = abs_error;
			}
			
			printf("%lf %%\n",error_percent);
		}
		printf("\n");
		
		/*CALCULATE HIDDEN ERROR SPLIT BY WEIGHTS*/
		//transpose of w_ho
		double w_ho_T[hiddenNodes][outputNodes];
		for(i=0;i<outputNodes;i++){
			for(j=0;j<hiddenNodes;j++){
				w_ho_T[j][i] = w_ho[i][j];
			}
		}
		
		//calculate dot product with transpose w_ho and output_error
		double hidden_errors[hiddenNodes][1];
		
		for(i=0;i<hiddenNodes;i++){
			dotsum = 0.0;
			for(k=0;k<outputNodes;k++){
				dotsum += w_ho_T[i][k] * output_error[k][0];
			}
			hidden_errors[i][0] = dotsum;
		}
		
		
		/*ADJUST WEIGHTS WITH ERRORS*/
		for(i=0;i<outputNodes;i++){
			for(j=0;j<hiddenNodes;j++){
				w_ho[i][j] += learning_rate * (output_error[i][0] * sigmoid(final_output[i][0]) * (1 - sigmoid(final_output[i][0]))) * hidden_output[j][0];
			}
		}
		
		//w_ih adjustment
		for(i=0;i<hiddenNodes;i++){
			for(j=0;j<inputNodes;j++){
				w_ih[i][j] += learning_rate * (hidden_errors[i][0] * sigmoid(hidden_output[i][0]) * (1 - sigmoid(hidden_output[i][0]))) * inputs[j][0];
			}
		}
		
	}
	
	/*SAVE WEIGHTS BACK TO FILE WITH .8lf*/
	wihFile = fopen(inputFileName,"w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return -99.9;
	}
	
	//save new weights to wihFile
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			fprintf(wihFile, "%.8f ", w_ih[i][j]);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	whoFile = fopen(outputFileName,"w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return -99.9;
	}
	
	//save new weights to whoFile
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			fprintf(whoFile, "%.8f ", w_ho[i][j]);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	
	return highest_error;
}




/*SAFER FOR FULLY TRAINING NETWORK*/
/*Biggest difference from previous fullTrain function is to only write to file once the final result is 
	obtained (highest error is below percent_error) instead of file I/O after every iteration of training file.*/
void fullTrain(char *networkID, double learning_rate, double percent_error, int inputNodes, int hiddenNodes, int outputNodes){
	
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[hiddenNodes][inputNodes];
	double w_ho[outputNodes][hiddenNodes];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	int i,j,k;
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	/*LOAD TRAINING DATA*/
	
	FILE *train;
	
	char trainFileName[50] = "Training/train_";
	strcat(trainFileName,networkID);
	strcat(trainFileName,".txt");
	
	train = fopen(trainFileName,"r");
	if(train == NULL){
		fprintf(stderr, "Error opening TRAIN file\n");
		return;
	}
	
	/*TRAIN WEIGHTS WITH TRAINING DATA*/
	
	double inputs[inputNodes][1];
	double expected_out[outputNodes][1];
	
	//initial value is higher than percent_error to enter first while loop which is necessary
	double highest_error = percent_error + 1;
	double abs_error;
	double error_percent;
	int count = 0;
	
	while(percent_error < highest_error){
		//need to set highest_error to 0 so any error larger than it can be set to highest_error
		highest_error = 0;
		while(fscanf(train,"%lf",&inputs[0][0]) != EOF){
			//grab inputs into input array
			for(i=1;i<inputNodes;i++){
				fscanf(train,"%lf",&inputs[i][0]);
			}
			
			//grab expected outputs into array
			for(i=0;i<outputNodes;i++){
				fscanf(train,"%lf",&expected_out[i][0]);
			}
			
			
			/** BEGIN COMPUTE **/
			/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
			double hidden_inputs[hiddenNodes][1];
			double dotsum;
			
			for(i=0;i<hiddenNodes;i++){
				dotsum = 0.0;
				for(k=0;k<inputNodes;k++){
					dotsum += w_ih[i][k] * inputs[k][0];
				}
				hidden_inputs[i][0] = dotsum;
			}
			

			/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
			double hidden_output[hiddenNodes][1];
			
			for(i=0;i<hiddenNodes;i++){
				hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
			}
			
			
			/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
			double output_in[outputNodes][1];
			
			for(i=0;i<outputNodes;i++){
				dotsum = 0.0;
				for(k=0;k<hiddenNodes;k++){
					dotsum += w_ho[i][k] * hidden_output[k][0];
				}
				output_in[i][0] = dotsum;
			}
			
			
			/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
			double final_output[outputNodes][1];
			
			for(i=0;i<outputNodes;i++){
				final_output[i][0] = sigmoid(output_in[i][0]);
			}
			/** END COMPUTE **/
			
			
			/*CALCULATE OUTPUT ERROR WITH TRAINING DATA*/
			double output_error[outputNodes][1];
			
			printf("NETWORK ERROR\n");
			for(i=0;i<outputNodes;i++){
				output_error[i][0] = expected_out[i][0] - final_output[i][0];
				
				error_percent = (output_error[i][0] / expected_out[i][0])*100;
				
				if(error_percent < 0){
					abs_error = -error_percent;
				}
				else{
					abs_error = error_percent;
				}
				
				if(highest_error < abs_error){
					highest_error = abs_error;
				}
				
				printf("%lf %%\n",error_percent);
			}
			printf("\n");
			
			/*CALCULATE HIDDEN ERROR SPLIT BY WEIGHTS*/
			//transpose of w_ho
			double w_ho_T[hiddenNodes][outputNodes];
			for(i=0;i<outputNodes;i++){
				for(j=0;j<hiddenNodes;j++){
					w_ho_T[j][i] = w_ho[i][j];
				}
			}
			
			//calculate dot product with transpose w_ho and output_error
			double hidden_errors[hiddenNodes][1];
			
			for(i=0;i<hiddenNodes;i++){
				dotsum = 0.0;
				for(k=0;k<outputNodes;k++){
					dotsum += w_ho_T[i][k] * output_error[k][0];
				}
				hidden_errors[i][0] = dotsum;
			}
			
			
			/*ADJUST WEIGHTS WITH ERRORS*/
			for(i=0;i<outputNodes;i++){
				for(j=0;j<hiddenNodes;j++){
					//printf("BEFORE_who: %lf\n",w_ho[i][j]);
					w_ho[i][j] += learning_rate * (output_error[i][0] * sigmoid(final_output[i][0]) * (1 - sigmoid(final_output[i][0]))) * hidden_output[j][0];
					//printf("AFTER_who: %lf\n",w_ho[i][j]);
				}
			}
			
			//w_ih adjustment
			for(i=0;i<hiddenNodes;i++){
				for(j=0;j<inputNodes;j++){
					//printf("BEFORE_wih: %lf\n",w_ih[i][j]);
					w_ih[i][j] += learning_rate * (hidden_errors[i][0] * sigmoid(hidden_output[i][0]) * (1 - sigmoid(hidden_output[i][0]))) * inputs[j][0];
					//printf("AFTER_wih: %lf\n",w_ih[i][j]);
				}
			}
			
		}
		
		//reset file pointer to beginning of training file
		rewind(train);
		
		printf("-------- i=%d --------\n",count);
		count++;
		printf("Highest Percent Error: %lf %%\n",highest_error);
	}
	
	/*SAVE WEIGHTS BACK TO FILE WITH .8lf*/
	wihFile = fopen(inputFileName,"w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	//save new weights to wihFile
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			fprintf(wihFile, "%.8f ", w_ih[i][j]);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	whoFile = fopen(outputFileName,"w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	//save new weights to whoFile
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			fprintf(whoFile, "%.8f ", w_ho[i][j]);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	return;
}

/*Used to compute the output by forward feeding the input array by pointer.
	This function initilizes the weights from file associated with the networkID
	and proceeds to pass the input along the network. It returns a pointer to result 
	which will need to be copied if saving is required.*/
double * compute(char *networkID, double *input, int inputNodes, int hiddenNodes, int outputNodes){
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[hiddenNodes][inputNodes];
	double w_ho[outputNodes][hiddenNodes];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return NULL;
	}
	
	int i,j,k;
	for(i=0;i<hiddenNodes;i++){
		for(j=0;j<inputNodes;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return NULL;
	}
	
	for(i=0;i<outputNodes;i++){
		for(j=0;j<hiddenNodes;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TURN INPUTS INTO 2D ARRAY*/
	double inputs[inputNodes][1];
	
	for(i=0;i<inputNodes;i++){
		inputs[i][0] = input[i];
	}
	
	/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double hidden_inputs[hiddenNodes][1];
	double dotsum;
	
	for(i=0;i<hiddenNodes;i++){
		dotsum = 0.0;
		for(k=0;k<inputNodes;k++){
			dotsum += w_ih[i][k] * inputs[k][0];
		}
		hidden_inputs[i][0] = dotsum;
	}
	

	/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
	double hidden_output[hiddenNodes][1];
	
	for(i=0;i<hiddenNodes;i++){
		hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
	}
	
	
	/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double output_in[outputNodes][1];
	
	for(i=0;i<outputNodes;i++){
		dotsum = 0.0;
		for(k=0;k<hiddenNodes;k++){
			dotsum += w_ho[i][k] * hidden_output[k][0];
		}
		output_in[i][0] = dotsum;
	}
	
	
	/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
	double final_output[outputNodes][1];
	
	for(i=0;i<outputNodes;i++){
		final_output[i][0] = sigmoid(output_in[i][0]);
	}
	
	/*ASSIGN TO POINTER TO RETURN*/
	double *result = (double *)malloc(sizeof(outputNodes));
	for(i=0;i<outputNodes;i++){
		result[i] = final_output[i][0];
	}
	
	return result;
}


double *computeSide1(char *networkID, double *input){
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SIDE1HIDDEN][SIDE1INPUT];
	double w_ho[SIDE1OUTPUT][SIDE1HIDDEN];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return NULL;
	}
	
	int i,j,k;
	for(i=0;i<SIDE1HIDDEN;i++){
		for(j=0;j<SIDE1INPUT;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return NULL;
	}
	
	for(i=0;i<SIDE1OUTPUT;i++){
		for(j=0;j<SIDE1HIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TURN INPUTS INTO 2D ARRAY*/
	double inputs[SIDE1INPUT][1];
	
	for(i=0;i<SIDE1INPUT;i++){
		inputs[i][0] = input[i];
	}
	
	/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double hidden_inputs[SIDE1HIDDEN][1];
	double dotsum;
	
	for(i=0;i<SIDE1HIDDEN;i++){
		dotsum = 0.0;
		for(k=0;k<SIDE1INPUT;k++){
			dotsum += w_ih[i][k] * inputs[k][0];
		}
		hidden_inputs[i][0] = dotsum;
	}
	

	/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
	double hidden_output[SIDE1HIDDEN][1];
	
	for(i=0;i<SIDE1HIDDEN;i++){
		hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
	}
	
	
	/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double output_in[SIDE1OUTPUT][1];
	
	for(i=0;i<SIDE1OUTPUT;i++){
		dotsum = 0.0;
		for(k=0;k<SIDE1HIDDEN;k++){
			dotsum += w_ho[i][k] * hidden_output[k][0];
		}
		output_in[i][0] = dotsum;
	}
	
	
	/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
	double final_output[SIDE1OUTPUT][1];
	
	for(i=0;i<SIDE1OUTPUT;i++){
		final_output[i][0] = sigmoid(output_in[i][0]);
	}
	
	/*ASSIGN TO POINTER TO RETURN*/
	static double result[SIDE1OUTPUT];
	for(i=0;i<SIDE1OUTPUT;i++){
		result[i] = final_output[i][0];
	}
	
	return result;
}


double *computeSide2(char *networkID, double *input){
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SIDE2HIDDEN][SIDE2INPUT];
	double w_ho[SIDE2OUTPUT][SIDE2HIDDEN];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return NULL;
	}
	
	int i,j,k;
	for(i=0;i<SIDE2HIDDEN;i++){
		for(j=0;j<SIDE2INPUT;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return NULL;
	}
	
	for(i=0;i<SIDE2OUTPUT;i++){
		for(j=0;j<SIDE2HIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TURN INPUTS INTO 2D ARRAY*/
	double inputs[SIDE2INPUT][1];
	
	for(i=0;i<SIDE2INPUT;i++){
		inputs[i][0] = input[i];
	}
	
	/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double hidden_inputs[SIDE2HIDDEN][1];
	double dotsum;
	
	for(i=0;i<SIDE2HIDDEN;i++){
		dotsum = 0.0;
		for(k=0;k<SIDE2INPUT;k++){
			dotsum += w_ih[i][k] * inputs[k][0];
		}
		hidden_inputs[i][0] = dotsum;
	}
	

	/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
	double hidden_output[SIDE2HIDDEN][1];
	
	for(i=0;i<SIDE2HIDDEN;i++){
		hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
	}
	
	
	/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double output_in[SIDE2OUTPUT][1];
	
	for(i=0;i<SIDE2OUTPUT;i++){
		dotsum = 0.0;
		for(k=0;k<SIDE2HIDDEN;k++){
			dotsum += w_ho[i][k] * hidden_output[k][0];
		}
		output_in[i][0] = dotsum;
	}
	
	
	/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
	double final_output[SIDE2OUTPUT][1];
	
	for(i=0;i<SIDE2OUTPUT;i++){
		final_output[i][0] = sigmoid(output_in[i][0]);
	}
	
	/*ASSIGN TO POINTER TO RETURN*/
	static double result[SIDE2OUTPUT];
	for(i=0;i<SIDE2OUTPUT;i++){
		result[i] = final_output[i][0];
	}
	
	return result;
}


double *computeSide3(char *networkID, double *input){
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SIDE3HIDDEN][SIDE3INPUT];
	double w_ho[SIDE3OUTPUT][SIDE3HIDDEN];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return NULL;
	}
	
	int i,j,k;
	for(i=0;i<SIDE3HIDDEN;i++){
		for(j=0;j<SIDE3INPUT;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return NULL;
	}
	
	for(i=0;i<SIDE3OUTPUT;i++){
		for(j=0;j<SIDE3HIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TURN INPUTS INTO 2D ARRAY*/
	double inputs[SIDE3INPUT][1];
	
	for(i=0;i<SIDE3INPUT;i++){
		inputs[i][0] = input[i];
	}
	
	/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double hidden_inputs[SIDE3HIDDEN][1];
	double dotsum;
	
	for(i=0;i<SIDE3HIDDEN;i++){
		dotsum = 0.0;
		for(k=0;k<SIDE3INPUT;k++){
			dotsum += w_ih[i][k] * inputs[k][0];
		}
		hidden_inputs[i][0] = dotsum;
	}
	

	/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
	double hidden_output[SIDE3HIDDEN][1];
	
	for(i=0;i<SIDE3HIDDEN;i++){
		hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
	}
	
	
	/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double output_in[SIDE3OUTPUT][1];
	
	for(i=0;i<SIDE3OUTPUT;i++){
		dotsum = 0.0;
		for(k=0;k<SIDE3HIDDEN;k++){
			dotsum += w_ho[i][k] * hidden_output[k][0];
		}
		output_in[i][0] = dotsum;
	}
	
	
	/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
	double final_output[SIDE3OUTPUT][1];
	
	for(i=0;i<SIDE3OUTPUT;i++){
		final_output[i][0] = sigmoid(output_in[i][0]);
	}
	
	/*ASSIGN TO POINTER TO RETURN*/
	static double result[SIDE3OUTPUT];
	for(i=0;i<SIDE3OUTPUT;i++){
		result[i] = final_output[i][0];
	}
	
	return result;
}


double *computeShape(char *networkID, double *input){
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SHAPEHIDDEN][SHAPEINPUT];
	double w_ho[SHAPEOUTPUT][SHAPEHIDDEN];
	
	char inputFileName[50] = "Weights/wih_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	wihFile = fopen(inputFileName,"r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return NULL;
	}
	
	int i,j,k;
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUT;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	char outputFileName[50] = "Weights/who_";
	strcat(outputFileName,networkID);
	strcat(outputFileName,".txt");
	
	whoFile = fopen(outputFileName,"r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return NULL;
	}
	
	for(i=0;i<SHAPEOUTPUT;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TURN INPUTS INTO 2D ARRAY*/
	double inputs[SHAPEINPUT][1];
	
	for(i=0;i<SHAPEINPUT;i++){
		inputs[i][0] = input[i];
	}
	
	/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double hidden_inputs[SHAPEHIDDEN][1];
	double dotsum;
	
	for(i=0;i<SHAPEHIDDEN;i++){
		dotsum = 0.0;
		for(k=0;k<SHAPEINPUT;k++){
			dotsum += w_ih[i][k] * inputs[k][0];
		}
		hidden_inputs[i][0] = dotsum;
	}
	

	/*COMPUTE HIDDEN OUTPUT WITH ACTIVATION FUNC (SIGMOID)*/
	double hidden_output[SHAPEHIDDEN][1];
	
	for(i=0;i<SHAPEHIDDEN;i++){
		hidden_output[i][0] = sigmoid(hidden_inputs[i][0]);
	}
	
	
	/*COMPUTE HIDDEN OUTPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double output_in[SHAPEOUTPUT][1];
	
	for(i=0;i<SHAPEOUTPUT;i++){
		dotsum = 0.0;
		for(k=0;k<SHAPEHIDDEN;k++){
			dotsum += w_ho[i][k] * hidden_output[k][0];
		}
		output_in[i][0] = dotsum;
	}
	
	
	/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
	double final_output[SHAPEOUTPUT][1];
	
	for(i=0;i<SHAPEOUTPUT;i++){
		final_output[i][0] = sigmoid(output_in[i][0]);
	}
	
	/*ASSIGN TO POINTER TO RETURN*/
	static double result[SHAPEOUTPUT];
	for(i=0;i<SHAPEOUTPUT;i++){
		result[i] = final_output[i][0];
	}
	
	return result;
}

//Used to write input scans into training files
void writeTrainInput(char *networkID, double *inputData, int dataSize){
	
	FILE *trainFile;
	
	char inputFileName[50] = "Training/train_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	trainFile = fopen(inputFileName,"a");
	if(trainFile == NULL){
		fprintf(stderr, "Error opening training file\n");
		return;
	}
	
	fprintf(trainFile, "\n");
	int i;
	for(i=0;i<dataSize;i++){
		fprintf(trainFile, "%.8f ", inputData[i]);
	}
	
	fclose(trainFile);
	
	return;
}


//Write raw data
void writeRawScan(char *fileName, double *inputData, int dataSize){
	
	FILE *trainFile;
	
	char inputFileName[50] = "RawData/";
	strcat(inputFileName,fileName);
	strcat(inputFileName,".txt");
	
	trainFile = fopen(inputFileName,"w");
	if(trainFile == NULL){
		fprintf(stderr, "Error opening training file\n");
		return;
	}
	
	fprintf(trainFile, "---------------SIDE1INPUT = %d------------------------------\n",SIDE1INPUT);
	int i;
	for(i=0;i<dataSize;i++){
		fprintf(trainFile, "%.8f\n", inputData[i]);
	}
	
	fclose(trainFile);
	
	return;
}

void writeRawScanAppend(char *fileName, double *inputData, int dataSize){
	
	FILE *trainFile;
	
	char inputFileName[50] = "RawData/";
	strcat(inputFileName,fileName);
	strcat(inputFileName,".txt");
	
	trainFile = fopen(inputFileName,"a");
	if(trainFile == NULL){
		fprintf(stderr, "Error opening training file\n");
		return;
	}
	
	int i;
	for(i=0;i<dataSize;i++){
		fprintf(trainFile, "%.8f\n", inputData[i]);
	}
	
	fclose(trainFile);
	
	return;
}


//Write raw data with tags
void writeRawLog(char *fileName, double *inputData, int dataSize, char tag[20]){
	
	FILE *trainFile;
	
	char inputFileName[50] = "RawData/";
	strcat(inputFileName,fileName);
	strcat(inputFileName,".txt");
	
	trainFile = fopen(inputFileName,"a");
	if(trainFile == NULL){
		fprintf(stderr, "Error opening training file\n");
		return;
	}
	
	char seperator[50] = "------------------------------";
	strcat(seperator,tag);
	strcat(seperator,"------------------------------\n");
	
	
	fprintf(trainFile, seperator);
	int i;
	for(i=0;i<dataSize;i++){
		fprintf(trainFile, "%.8f\n", inputData[i]);
	}
	
	fclose(trainFile);
	
	return;
}

/*double maxfromFile(char *fileName, int dataSize){
	FILE *trainFile;
	
	char inputFileName[50] = "RawData/";
	strcat(inputFileName,fileName);
	strcat(inputFileName,".txt");
	
	trainFile = fopen(inputFileName,"r");
	if(trainFile == NULL){
		fprintf(stderr, "Error opening RawUnscaled file\n");
		return -1;
	}
	
	int i;
	double hold;
	double max = 0;
	double min = 0;
	for(i=0;i<dataSize;i++){
		fscanf(trainFile, "%lf\n", &hold);
		if(hold > max){
			max = hold;
		}
		
		if(hold < min){
			min = hold;
		}
	}
	
	fclose(trainFile);
	
	//Grab greatest magnitude from file
	if(-min > max){
		max = -min;
	}
	
	return max;
}*/

/*void scaleTrain(char *fileName, double max, char *networkID, int dataSize){
	FILE *RawFile;
	
	char inputFileName[50] = "RawData/";
	strcat(inputFileName,fileName);
	strcat(inputFileName,".txt");
	
	RawFile = fopen(inputFileName,"r");
	if(RawFile == NULL){
		fprintf(stderr, "Error opening RawUnscaled file\n");
		return;
	}
	
	int i;
	double hold;
	double scaledinputs[dataSize];
	for(i=0;i<dataSize;i++){
		fscanf(RawFile, "%lf\n", &hold);
		scaledinputs[i] = hold / max;
	}
	
	fclose(RawFile);
	
	writeTrainInput("Side1Net",scaledinputs,dataSize);
	
	return;
}*/