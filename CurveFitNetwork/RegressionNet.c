#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define MAXCOEFFICIENTS 5

typedef struct object {
	double coeff[MAXCOEFFICIENTS];
	double A[MAXCOEFFICIENTS][MAXCOEFFICIENTS];
	double Ainv[MAXCOEFFICIENTS][MAXCOEFFICIENTS];
} Object;



void inv(int mat_size, Object *obj){

    //FILLING INVERSE MATRIX - 1's in pivots, 0's everywhere else
    int j,k,l,m,row,col;

    for(row = 0; row < mat_size; row++){

        for(col = 0; col<mat_size;col++){

            if(row == col){

                obj -> Ainv[row][col] = 1;

            }

            else{

                obj -> Ainv[row][col] = 0;

            }

        }

    }

   

    double pivot, neg_pivot;

    for(k = 0; k < mat_size;k++){//row

        pivot = obj -> A[k][k];//get value at pivot position

        for(m = 0;m<mat_size;m++){//divide the whole row by that value at pivot position

                obj -> A[k][m] /= pivot;

                obj -> Ainv[k][m] /= pivot;

        }

        for(l = 0;l<mat_size;l++){//if row == row of pivot, skip over

            if(l == k){

                continue;

            }

            else{

                neg_pivot = -1.0*obj -> A[l][k];//get negative value of other row to multiply with '1' to create 0 in that column               

                for(j= 0;j<mat_size;j++){//create 0 in that column's row, and go to next row to create 0

                        obj -> A[l][j] += (obj -> A[k][j]*neg_pivot);

                        obj -> Ainv[l][j] += (obj -> Ainv[k][j]*neg_pivot);

                }

            }

        }

    }
    
	return;
}

//Calulates regression nth polynomial (determined by coefficients) over a set of inputs and uses fields in object struct
// to communicate between function calls.
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
	inv(coefficients,obj);
	
	//dot product: Coeff = inverse A (dot) Bee and set them in object struct
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

	regression(4, scan1, 10, obj);
	
	
	printf("\n--Coefficients--\n");
	
	for(i=0;i<MAXCOEFFICIENTS;i++){
		printf("%lf ",obj -> coeff[i]);
	}
	printf("\n");
	
	free(obj);

	return 1;
}
