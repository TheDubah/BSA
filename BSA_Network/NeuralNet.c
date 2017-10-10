#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <math.h>
#include <time.h>


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
double *computeShape(char *networkID, double *input, int inputNodes, int hiddenNodes, int outputNodes){
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
	double *result = malloc(sizeof(outputNodes));
	for(i=0;i<outputNodes;i++){
		result[i] = final_output[i][0];
	}
	
	return result;
}

void normalize_byPtr(double *input, int inputLength){
	
	double max = input[0];
	double min = input[0];
	
	//Finding max and min values from input array
	int i;
	for(i=0;i<inputLength;i++){
		if(input[i] > max){
			max = input[i];
		}
		
		if(input[i] < min){
			min = input[i];
		}
	}
	
	//Normalizing between -1 and 1
	for(i=0;i<inputLength;i++){
		input[i] = (2 * ( (input[i] - min) / (max - min) )) - 1;
	}
	
	return;
}

int main(){
	
	//double input[20] = {-47.96285807,-42.93207642,-37.90129478,-32.87051314,-27.83973150,-22.80894986,-17.77816821,-12.74738657,-7.71660493,-2.68582329,2.34495835,7.37574000,12.40652164,17.43730328,22.46808492,27.49886656,32.52964821,37.56042985,42.59121149,47.62199313};
	double input[20] = {-105.40582885,-94.30509355,-83.20435825,-72.10362296,-61.00288766,-49.90215237,-38.80141707,-27.70068178,-16.59994648,-5.49921119,5.60152411,16.70225941,27.80299470,38.90373000,50.00446529,61.10520059,72.20593588,83.30667118,94.40740648,105.50814177};
	
	normalize_byPtr(input,20);
	
	int i;
	for(i=0;i<20;i++){
		printf("%.8lf ",input[i]);
	}
	printf("\n");
	
	
	//MAKE SURE TO SET TRAINING FILE AFTER THIS
	
	
	//newNet("Test",1586, 12,9,4);
	
	
	//train("Test",1);
	//fullTrain("Test",1,1, 12,9,4);
	
	/*double input[12] = {1.0, 0.1, 0.1, 0.1, 1.0, 0.1, 1.0, 0.1, 1.0, 0.1, 0.01, 0.01};
	
	double *result;
	result = computeShape("Test",input, 12,9,4);
	int i;
	for(i=0;i<4;i++){
		printf("%d: %lf\n",i,result[i]);
	}*/
	
	//deleteNet("Test");
	
	return 1;
}