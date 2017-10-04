#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAXCOEFFICIENTS 5

typedef struct object {
	double coeff[MAXCOEFFICIENTS];
	double A[MAXCOEFFICIENTS][MAXCOEFFICIENTS];
	double Ainv[MAXCOEFFICIENTS][MAXCOEFFICIENTS];
} Object;


void inv(Object *obj){
	
	return;
}


void regression(int coefficients, double *input, int inputSamples, Object *obj){
	
	//init obj coefficent array and A and inverse A matrices to zeros
	int i,j;
	for(i=0;i<MAXCOEFFICIENTS;i++){
		obj -> coeff[i] = 0;
		for(j=0;j<MAXCOEFFICIENTS;j++){
			obj -> Ainv[i][j] = 0;
		}
	}
	
	//init A matrix to zeros
	for(i=0;i<coefficients;i++){
		for(j=0;j<coefficients;j++){
			obj -> A[i][j] = 0;
		}
	}
	
	
	//setup A matrix: summation of all inputs raised to power raiseIndex. inputs being the indexes k where they range from 0 to inputSamples.
	int raiseIndex, n,m,k;
	for(n=0;n<coefficients;n++){
		raiseIndex = n; //different starting point for each row in matrix (+1)
		for(m=0;m<coefficients;m++){
			if(n==0 && m==0){
				obj -> A[n][m] = inputSamples;
			}
			else{
				//summation of inputs raised to raiseIndex
				for(k=0;k<inputSamples;k++){
					obj -> A[n][m] += pow(k,raiseIndex);
				}
			}
			raiseIndex++; //increment for each column
		}
	}
	
	
	//Setting up B
	double Bee[coefficients];
	
	//init Bee to 0
	for(i=0;i<coefficients;i++){
		Bee[i] = 0;
	}
	
	//Summation of index k raised to power n multiplied by inputs corresponding to index: inputs[k]
	for(n=0;n<coefficients;n++){
		if(n==0){
			//getting summation of inputs for Bee[0]
			double ySum = 0;
			for(i=0;i<inputSamples;i++){
				ySum += input[i];
			}
			Bee[n] = ySum;
		}
		else{
			for(k=0;k<inputSamples;k++){
				Bee[n] += pow(k,n) * input[k];
			}
		}
	}
	
	//get inverse of A
	inv(obj);
	
	//dot product: Coeff = inverse A (dot) Bee
	double dotSum;
	for(n=0;n<coefficients;n++){
		dotSum = 0;
		for(m=0;m<coefficients;m++){
			dotSum += obj->Ainv[n][m] * Bee[m];
		}
		obj -> coeff[n] = dotSum;
	}
	return;
}


int main(void){

	int coefficients = 3;

	Object *obj = (Object *)malloc(sizeof(Object));
	
	int i;
	for(i=0;i<MAXCOEFFICIENTS;i++){
		obj -> coeff[i] = 0;
	}
	
	
	double scan1[10] = {10,5,3,2,1,2,3,5,10,15};

	regression(3, scan1, 9, obj);
	
	int j;
	for(i=0;i<coefficients;i++){
		for(j=0;j<coefficients;j++){
			printf("%lf ",obj -> Ainv[i][j]);
		}
		printf("\n");
	}
	
	printf("\n--Coefficients--\n");
	
	for(i=0;i<MAXCOEFFICIENTS;i++){
		printf("%lf ",obj -> coeff[i]);
	}
	printf("\n");
	
	free(obj);

	return 1;
}
