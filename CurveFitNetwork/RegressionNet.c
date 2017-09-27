//STANDARD LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <math.h>
#include <time.h>

#define COEFF 3
#define INPUTS 10

void newNet_Reg(char *networkID, int seed, int coefficients);
void deleteNet(char *networkID);
void train_reg(char *networkID, int coefficients, double *training, int trainSamples, double learning_rate, int iterations);
double sigmoid(double x);

int main(void){
	//newNet_Reg("CurveFit",117,COEFF);
	
	double inputs[INPUTS] = {1.0,-2.0,-3.0,-2.0,1.0,6.0,13.0,22.0,33.0,46.0};
	
	train_reg("CurveFit",COEFF, inputs, INPUTS, 1, 100);
	
	return 0;
}

void newNet_Reg(char *networkID, int seed, int coefficients){
	
	FILE *coefFile;
	
	char inputFileName[50] = "Coefficients/cf_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	//create file with NetworkID name
	coefFile = fopen(inputFileName,"w");
	if(coefFile == NULL){
		fprintf(stderr, "Error opening COEF file\n");
		return;
	}
	
	//initialilize with seed
	srand(seed);
	
	int i;
	double random;
	for(i=0;i<coefficients;i++){
		//use random num for seed for next iteration
		srand(rand());
		//write random values to weight file
		random = ((((double)rand()) / RAND_MAX) * 2) - 1;
		fprintf(coefFile, "%.8f ", random);
	}
	fprintf(coefFile, "\n");
	
	fclose(coefFile);
	
	return;
}


void deleteNet(char *networkID){
	
	int deleteStat;
	
	char inputFileName[50] = "Coefficients/cf_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	deleteStat = remove(inputFileName);
	if(deleteStat != 0){
		printf("Error: unable to delete COEF\n");
		return;
	}
	
	return;
}

void train_reg(char *networkID, int coefficients, double *training, int trainSamples, double learning_rate, int iterations){
	
	/*Load coeff from file*/
	FILE *coefFile;
	double coeff[coefficients];
	
	char inputFileName[50] = "Coefficients/cf_";
	strcat(inputFileName,networkID);
	strcat(inputFileName,".txt");
	
	//copy coefficients from file
	coefFile = fopen(inputFileName,"w");
	if(coefFile == NULL){
		fprintf(stderr, "Error opening COEF file\n");
		return;
	}
	
	int i;
	for(i=0;i<coefficients;i++){
		fscanf(coefFile,"%lf",&coeff[i]);
	}
	
	fclose(coefFile);
	
	
	int j,k;
	double sum, error;
	for(k=0;k<iterations;k++){
		
		printf("----------------k=%d--------------------\n", k);
		for(i=0;i<coefficients;i++){
			printf("Coeff[%d]: %lf\n", i, coeff[i]);
		}
		printf("---------------------------------------\n");
		
		
		for(i=0;i<trainSamples;i++){
			sum = 0;
			for(j=0;j<coefficients;j++){
				sum += coeff[j] * pow(i,j);
			}
			
			error = training[i] - sum;
			
			for(j=0;j<coefficients;j++){
				coeff[j] += learning_rate * (error * sigmoid(sum) * (1 - sigmoid(sum))) * training[i];
			}
		}
	}
	
	//write back new coefficients
	coefFile = fopen(inputFileName,"w");
	if(coefFile == NULL){
		fprintf(stderr, "Error opening COEF file\n");
		return;
	}
	
	for(i=0;i<coefficients;i++){
		fprintf(coefFile, "%.8f ", coeff[i]);
	}
	
	return;
}

//Activation function for passing on
double sigmoid(double x){
	return (1/(1+exp(-x)));
}