/*exporting gpio pin.
setting gpio pins (direction, polarity, pull up, pull down etc)
options: up/down for pul resistors, in/out for direction, high/low for value whatever else is needed etc.
*/
#include <string.h>
#include <time.h> 
void export(int gpio_num);
void unexport(int gpio_num);
void setdirection(int gpio_num, char *option);
void setvalue(int gpio_num, int val);
int getvalue(int gpio_num); 




void export(int gpio_num){ //*This works and has been tested
	char itoa[3]; //create buffer string for integer of gpio number
 
	sprintf(itoa, "%d", gpio_num); //convert the integer parameter to a string
 
	char filepath[] = "/sys/class/gpio/export"; //filepath to gpio export file
	FILE* pin = fopen(filepath, "w"); //open the export file
	fputs(itoa, pin); //write to it to export the gpio number specified.
	fclose(pin); //close the file
}
 
void unexport(int gpio_num){//if needed 
	
	char itoa[3]; //create buffer string for integer of gpio number
 
	sprintf(itoa, "%d", gpio_num); //convert the integer parameter to a string
 
	char filepath[] = "/sys/class/gpio/unexport"; //filepath to gpio unexport file
	FILE* pin = fopen(filepath, "w"); //open the export file
	fputs(itoa, pin); //write to it to export the gpio number specified.
	fclose(pin); //close the file
 
 
}
 
void setdirection(int gpio_num, char *option){
 
	char value[4]; //value before for option
        char property[] = "direction";
        char itoa[3]; //integer buffer strings
        sprintf(itoa, "%d", gpio_num); //convert integer to string
        char exportpath[40]; //filepath for gpio folder
        char filepath[] = "/sys/class/gpio/gpio"; //manipulate the strings to m$
        strcpy(exportpath, filepath);
        strcpy(value, option);
        strcat(exportpath, itoa);
        strcat(exportpath, "/");
        strcat(exportpath, property); //add the property
        FILE* pin = fopen(exportpath, "w"); //open the file
	int i;
	int j = 0;
        if(pin == NULL){
	        for(i = 0; exportpath[i] != '\0'; i++){
			j++;
		}
                printf("FILENAME: %s\n", exportpath);
                exit(1);
        }
        fprintf(pin, "%s", option); //write to the file
        fclose(pin); //close the file
	
}
 
void setvalue(int gpio_num, int val){
 
	char value[2]; //value before for option
	char property[] = "value"; 
	char itoa[3]; //integer buffer strings
	sprintf(itoa, "%d", gpio_num); //convert integer to string
	sprintf(value,"%d", val);
	char exportpath[40]; //filepath for gpio folder
	char filepath[] = "/sys/class/gpio/gpio"; //manipulate the strings to make exportpath
	strcpy(exportpath, filepath);
	strcat(exportpath, itoa);
	strcat(exportpath, "/");
	strcat(exportpath, property); //add the property
	FILE* pin = fopen(exportpath, "w"); //open the file
	if(pin == NULL){
                printf("FILENAME: %s\n", exportpath);
                exit(1);
        }
        fprintf(pin, "%s", value); //write to the file
	fclose(pin); //close the file
}

int getvalue(int gpio_num){
	char value[1], itoa[3];
	sprintf(itoa, "%d", gpio_num);
	char exportpath[40];
	char filepath[] = "/sys/class/gpio/gpio";
	strcpy(exportpath, filepath);
	strcat(exportpath, itoa);
	strcat(exportpath, "/");
	strcat(exportpath, "value");
	FILE* pin = fopen(exportpath, "r"); //open the file
	if(pin == NULL){
		printf("PATH: %s\n", exportpath);
		exit(1);
	}
	fscanf(pin, "%s", value);
	fclose(pin);
	return atoi(value);
}

