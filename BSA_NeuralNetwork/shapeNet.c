#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>
#include <time.h>

#define SHAPEINPUTS 12
#define SHAPEHIDDEN 9
#define SHAPEOUTPUTS 4


struct Object {
	double *input;
	double *result;
};

double sigmoid(double x){
	return (1/(1+exp(-x)));
}

void newNet(int seed){
	//create new net and file
	FILE *wihFile,*whoFile;
	
	wihFile = fopen("wihShape.txt","w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	//initialilize with seed
	srand(seed);
	
	int i,j;
	double random;
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUTS;j++){
			//use random num for seed for next iteration
			srand(rand());
			//write to file
			random = ((((double)rand()) / RAND_MAX) * 2) - 1;
			fprintf(wihFile, "%.8f ", random);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	whoFile = fopen("whoShape.txt","w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	//same init for w_ho
	for(i=0;i<SHAPEOUTPUTS;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			//use random num for seed for next iteration
			srand(rand());
			//write to file
			random = ((((double)rand()) / RAND_MAX) * 2) - 1;
			fprintf(whoFile, "%.8f ", random);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
}

double train_err(double learning_rate){
		
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SHAPEHIDDEN][SHAPEINPUTS];
	double w_ho[SHAPEOUTPUTS][SHAPEHIDDEN];
	
	
	wihFile = fopen("wihShape.txt","r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return -99.9;
	}
	
	int i,j,k;
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUTS;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	whoFile = fopen("whoShape.txt","r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return -99.9;
	}
	
	for(i=0;i<SHAPEOUTPUTS;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TRAIN WEIGHTS WITH TRAINING DATA*/
	FILE *train;
	train = fopen("shapeTraining.txt","r");
	if(train == NULL){
		fprintf(stderr, "Error opening TRAIN file\n");
		return -99.9;
	}
	
	
	double inputs[SHAPEINPUTS][1];
	double expected_out[SHAPEOUTPUTS][1];
	
	double highest_error = 0;
	double abs_error;
	double error_percent;
	
	while(fscanf(train,"%lf",&inputs[0][0]) != EOF){
		//grab inputs into input array
		for(i=1;i<SHAPEINPUTS;i++){
			fscanf(train,"%lf",&inputs[i][0]);
		}
		
		//grab expected outputs into array
		for(i=0;i<SHAPEOUTPUTS;i++){
			fscanf(train,"%lf",&expected_out[i][0]);
		}
		
		
		/** BEGIN COMPUTE **/
		/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
		double hidden_inputs[SHAPEHIDDEN][1];
		double dotsum;
		
		for(i=0;i<SHAPEHIDDEN;i++){
			dotsum = 0.0;
			for(k=0;k<SHAPEINPUTS;k++){
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
		double output_in[SHAPEOUTPUTS][1];
		
		for(i=0;i<SHAPEOUTPUTS;i++){
			dotsum = 0.0;
			for(k=0;k<SHAPEHIDDEN;k++){
				dotsum += w_ho[i][k] * hidden_output[k][0];
			}
			output_in[i][0] = dotsum;
		}
		
		
		/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
		double final_output[SHAPEOUTPUTS][1];
		
		for(i=0;i<SHAPEOUTPUTS;i++){
			final_output[i][0] = sigmoid(output_in[i][0]);
		}
		/** END COMPUTE **/
		
		
		/*CALCULATE OUTPUT ERROR WITH TRAINING DATA*/
		double output_error[SHAPEOUTPUTS][1];
		
		printf("NETWORK ERROR\n");
		for(i=0;i<SHAPEOUTPUTS;i++){
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
		double w_ho_T[SHAPEHIDDEN][SHAPEOUTPUTS];
		for(i=0;i<SHAPEOUTPUTS;i++){
			for(j=0;j<SHAPEHIDDEN;j++){
				w_ho_T[j][i] = w_ho[i][j];
			}
		}
		
		//calculate dot product with transpose w_ho and output_error
		double hidden_errors[SHAPEHIDDEN][1];
		
		for(i=0;i<SHAPEHIDDEN;i++){
			dotsum = 0.0;
			for(k=0;k<SHAPEOUTPUTS;k++){
				dotsum += w_ho_T[i][k] * output_error[k][0];
			}
			hidden_errors[i][0] = dotsum;
		}
		
		
		/*ADJUST WEIGHTS WITH ERRORS*/
		for(i=0;i<SHAPEOUTPUTS;i++){
			for(j=0;j<SHAPEHIDDEN;j++){
				w_ho[i][j] += learning_rate * (output_error[i][0] * sigmoid(final_output[i][0]) * (1 - sigmoid(final_output[i][0]))) * hidden_output[j][0];
			}
		}
		
		//w_ih adjustment
		for(i=0;i<SHAPEHIDDEN;i++){
			for(j=0;j<SHAPEINPUTS;j++){
				w_ih[i][j] += learning_rate * (hidden_errors[i][0] * sigmoid(hidden_output[i][0]) * (1 - sigmoid(hidden_output[i][0]))) * inputs[j][0];
			}
		}
		
	}
	
	/*SAVE WEIGHTS BACK TO FILE WITH .8lf*/
	wihFile = fopen("wihShape.txt","w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return -99.9;
	}
	
	//save new weights to wihFile
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUTS;j++){
			fprintf(wihFile, "%.8f ", w_ih[i][j]);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	whoFile = fopen("whoShape.txt","w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return -99.9;
	}
	
	//save new weights to whoFile
	for(i=0;i<SHAPEOUTPUTS;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			fprintf(whoFile, "%.8f ", w_ho[i][j]);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	
	return highest_error;
}

void train(double learning_rate){
		
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SHAPEHIDDEN][SHAPEINPUTS];
	double w_ho[SHAPEOUTPUTS][SHAPEHIDDEN];
	
	
	wihFile = fopen("wihShape.txt","r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	int i,j,k;
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUTS;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	whoFile = fopen("whoShape.txt","r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	for(i=0;i<SHAPEOUTPUTS;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TRAIN WEIGHTS WITH TRAINING DATA*/
	FILE *train;
	train = fopen("shapeTraining.txt","r");
	if(train == NULL){
		fprintf(stderr, "Error opening TRAIN file\n");
		return;
	}
	
	
	double inputs[SHAPEINPUTS][1];
	double expected_out[SHAPEOUTPUTS][1];
	
	while(fscanf(train,"%lf",&inputs[0][0]) != EOF){
		//grab inputs into input array
		for(i=1;i<SHAPEINPUTS;i++){
			fscanf(train,"%lf",&inputs[i][0]);
		}
		
		//grab expected outputs into array
		for(i=0;i<SHAPEOUTPUTS;i++){
			fscanf(train,"%lf",&expected_out[i][0]);
		}
		
		
		/** BEGIN COMPUTE **/
		/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
		double hidden_inputs[SHAPEHIDDEN][1];
		double dotsum;
		
		for(i=0;i<SHAPEHIDDEN;i++){
			dotsum = 0.0;
			for(k=0;k<SHAPEINPUTS;k++){
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
		double output_in[SHAPEOUTPUTS][1];
		
		for(i=0;i<SHAPEOUTPUTS;i++){
			dotsum = 0.0;
			for(k=0;k<SHAPEHIDDEN;k++){
				dotsum += w_ho[i][k] * hidden_output[k][0];
			}
			output_in[i][0] = dotsum;
		}
		
		
		/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
		double final_output[SHAPEOUTPUTS][1];
		
		for(i=0;i<SHAPEOUTPUTS;i++){
			final_output[i][0] = sigmoid(output_in[i][0]);
		}
		/** END COMPUTE **/
		
		
		/*CALCULATE OUTPUT ERROR WITH TRAINING DATA*/
		double output_error[SHAPEOUTPUTS][1];
		
		printf("NETWORK ERROR\n");
		for(i=0;i<SHAPEOUTPUTS;i++){
			output_error[i][0] = expected_out[i][0] - final_output[i][0];
			
			printf("%lf %%\n",(output_error[i][0] / expected_out[i][0])*100);
		}
		printf("\n");
		
		/*CALCULATE HIDDEN ERROR SPLIT BY WEIGHTS*/
		//transpose of w_ho
		double w_ho_T[SHAPEHIDDEN][SHAPEOUTPUTS];
		for(i=0;i<SHAPEOUTPUTS;i++){
			for(j=0;j<SHAPEHIDDEN;j++){
				w_ho_T[j][i] = w_ho[i][j];
			}
		}
		
		//calculate dot product with transpose w_ho and output_error
		double hidden_errors[SHAPEHIDDEN][1];
		
		for(i=0;i<SHAPEHIDDEN;i++){
			dotsum = 0.0;
			for(k=0;k<SHAPEOUTPUTS;k++){
				dotsum += w_ho_T[i][k] * output_error[k][0];
			}
			hidden_errors[i][0] = dotsum;
		}
		
		
		/*ADJUST WEIGHTS WITH ERRORS*/
		for(i=0;i<SHAPEOUTPUTS;i++){
			for(j=0;j<SHAPEHIDDEN;j++){
				w_ho[i][j] += learning_rate * (output_error[i][0] * sigmoid(final_output[i][0]) * (1 - sigmoid(final_output[i][0]))) * hidden_output[j][0];
			}
		}
		
		//w_ih adjustment
		for(i=0;i<SHAPEHIDDEN;i++){
			for(j=0;j<SHAPEINPUTS;j++){
				w_ih[i][j] += learning_rate * (hidden_errors[i][0] * sigmoid(hidden_output[i][0]) * (1 - sigmoid(hidden_output[i][0]))) * inputs[j][0];
			}
		}
		
	}
	
	/*SAVE WEIGHTS BACK TO FILE WITH .8lf*/
	wihFile = fopen("wihShape.txt","w");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	//save new weights to wihFile
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUTS;j++){
			fprintf(wihFile, "%.8f ", w_ih[i][j]);
		}
		fprintf(wihFile, "\n");
	}
	
	fclose(wihFile);
	
	whoFile = fopen("whoShape.txt","w");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	//save new weights to whoFile
	for(i=0;i<SHAPEOUTPUTS;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			fprintf(whoFile, "%.8f ", w_ho[i][j]);
		}
		fprintf(whoFile, "\n");
	}
	
	fclose(whoFile);
	
	return;
}

void computeShape(struct Object *obj){
	/*LOAD WEIGHTS FROM FILE*/
	FILE *wihFile,*whoFile;
	double w_ih[SHAPEHIDDEN][SHAPEINPUTS];
	double w_ho[SHAPEOUTPUTS][SHAPEHIDDEN];
	
	
	wihFile = fopen("wihShape.txt","r");
	if(wihFile == NULL){
		fprintf(stderr, "Error opening WIH file\n");
		return;
	}
	
	int i,j,k;
	for(i=0;i<SHAPEHIDDEN;i++){
		for(j=0;j<SHAPEINPUTS;j++){
			//load value for wih.txt
			fscanf(wihFile,"%lf",&w_ih[i][j]);
		}
	}
	
	fclose(wihFile);
	
	whoFile = fopen("whoShape.txt","r");
	if(whoFile == NULL){
		fprintf(stderr, "Error opening WHO file\n");
		return;
	}
	
	for(i=0;i<SHAPEOUTPUTS;i++){
		for(j=0;j<SHAPEHIDDEN;j++){
			//load value for who.txt
			fscanf(whoFile,"%lf",&w_ho[i][j]);
		}
	}
	
	fclose(whoFile);
	
	
	/*TURN INPUTS INTO 2D ARRAY*/
	double inputs[SHAPEINPUTS][1];
	
	for(i=0;i<SHAPEINPUTS;i++){
		inputs[i][0] = obj->input[i];
	}
	
	/*COMPUTE HIDDEN INPUTS WITH WEIGHTS (DOT PRODUCT)*/
	double hidden_inputs[SHAPEHIDDEN][1];
	double dotsum;
	
	for(i=0;i<SHAPEHIDDEN;i++){
		dotsum = 0.0;
		for(k=0;k<SHAPEINPUTS;k++){
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
	double output_in[SHAPEOUTPUTS][1];
	
	for(i=0;i<SHAPEOUTPUTS;i++){
		dotsum = 0.0;
		for(k=0;k<SHAPEHIDDEN;k++){
			dotsum += w_ho[i][k] * hidden_output[k][0];
		}
		output_in[i][0] = dotsum;
	}
	
	
	/*COMPUTE FINAL OUTPUT WITH ACTIVATION FUNC*/
	double final_output[SHAPEOUTPUTS][1];
	
	for(i=0;i<SHAPEOUTPUTS;i++){
		final_output[i][0] = sigmoid(output_in[i][0]);
	}
	
	/*ASSIGN BACK TO OBJECT IN NORMAL 1D ARRAY*/
	for(i=0;i<SHAPEOUTPUTS;i++){
		obj->result[i] = final_output[i][0];
	}
	
	return;
	
}

void fullTrain(double learning_rate, double percent_error){
	int i = 0;
	double error = 100.0;
	
	while(error > percent_error){
		printf("-------- i=%d --------\n",i);
		i++;
		error = train_err(learning_rate);
		printf("Highest Percent Error: %lf %%\n",error);
	}
}

int main(){
	
	newNet(117);
	
	fullTrain(1,1);
	
	/*int i = 0;
	for(i=0;i<5000;i++){
		printf("-------- i=%d --------\n",i);
		train(1.0);
	}*/
	
	
	

	
	/*struct Object obj;
	double inputs[SHAPEINPUTS] = {1,0.01,0.01,1,0.01,0.01,1,0.01,0.01};
	double resu[SHAPEOUTPUTS];
	obj.input = inputs;
	obj.result = resu;
	
	computeShape(&obj);
	
	int i;
	for(i=0;i<SHAPEOUTPUTS;i++){
		printf("result: %lf\n",obj.result[i]);
	}*/
	
	return 1;
}