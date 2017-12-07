#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define BASE_CH 9
#define SHOULDER_CH 7
#define ELBOW_CH 5
#define WRIST_CH 3


int servo_command1(int channel_1, int pulse_1, int time){
	//initialize and open serial connection with USB0 along with relevant flags
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}

	//strings to hold int parameters
	char s_channel[3];
	char s_pulse[5];
	char s_time[6];

	//convert to string
	sprintf(s_channel, "%d", channel_1);
	sprintf(s_pulse, "%d", pulse_1);
	sprintf(s_time, "%d", time);

	//parse command to send
	// FORMAT: #<ch> P<pulse> T<time> OR #xxPxxxxTxxxxx\r
	char command[16] = "#";

	strcat(command,s_channel);
	strcat(command,"P");
	strcat(command,s_pulse);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");

	//write to serial device and also error checking
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}

	//close serial device
	close(ssc32u);

	//sleep until move is complete +1 second for margin
	usleep((time+1000)*1000);

	return 1;
}

int servo_command2(int channel_1, int pulse_1, int channel_2, int pulse_2, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxxTxxxxx\r
	char command[25] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1000)*1000);
	
	return 1;
}

int servo_command3(int channel_1, int pulse_1, int channel_2, int pulse_2, int channel_3, int pulse_3, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_channel_3[3];
	char s_pulse_3[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_channel_3, "%d", channel_3);
	sprintf(s_pulse_3, "%d", pulse_3);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxx#xxPxxxxTxxxxx\r
	char command[33] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"#");
	strcat(command,s_channel_3);
	strcat(command,"P");
	strcat(command,s_pulse_3);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1000)*1000);
	
	return 1;
}

int servo_command4(int channel_1, int pulse_1, int channel_2, int pulse_2, int channel_3, int pulse_3, int channel_4, int pulse_4, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_channel_3[3];
	char s_pulse_3[5];
	char s_channel_4[3];
	char s_pulse_4[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_channel_3, "%d", channel_3);
	sprintf(s_pulse_3, "%d", pulse_3);
	sprintf(s_channel_4, "%d", channel_4);
	sprintf(s_pulse_4, "%d", pulse_4);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxx#xxPxxxx#xxPxxxxTxxxxx\r
	char command[41] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"#");
	strcat(command,s_channel_3);
	strcat(command,"P");
	strcat(command,s_pulse_3);
	strcat(command,"#");
	strcat(command,s_channel_4);
	strcat(command,"P");
	strcat(command,s_pulse_4);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1000)*1000);
	
	return 1;
}

int servo_command5(int channel_1, int pulse_1, int channel_2, int pulse_2, int channel_3, int pulse_3, int channel_4, int pulse_4, int channel_5, int pulse_5, int time){
	int ssc32u;
	if((ssc32u = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
		fprintf(stderr, "Error while opening serial device\n");
		return -1;
	}
	
	char s_channel_1[3];
	char s_pulse_1[5];
	char s_channel_2[3];
	char s_pulse_2[5];
	char s_channel_3[3];
	char s_pulse_3[5];
	char s_channel_4[3];
	char s_pulse_4[5];
	char s_channel_5[3];
	char s_pulse_5[5];
	char s_time[6];
	
	sprintf(s_channel_1, "%d", channel_1);
	sprintf(s_pulse_1, "%d", pulse_1);
	sprintf(s_channel_2, "%d", channel_2);
	sprintf(s_pulse_2, "%d", pulse_2);
	sprintf(s_channel_3, "%d", channel_3);
	sprintf(s_pulse_3, "%d", pulse_3);
	sprintf(s_channel_4, "%d", channel_4);
	sprintf(s_pulse_4, "%d", pulse_4);
	sprintf(s_channel_5, "%d", channel_5);
	sprintf(s_pulse_5, "%d", pulse_5);
	sprintf(s_time, "%d", time);
	
	//#xxPxxxx#xxPxxxx#xxPxxxx#xxPxxxx#xxPxxxxTxxxxx\r
	char command[49] = "#";
	
	strcat(command,s_channel_1);
	strcat(command,"P");
	strcat(command,s_pulse_1);
	strcat(command,"#");
	strcat(command,s_channel_2);
	strcat(command,"P");
	strcat(command,s_pulse_2);
	strcat(command,"#");
	strcat(command,s_channel_3);
	strcat(command,"P");
	strcat(command,s_pulse_3);
	strcat(command,"#");
	strcat(command,s_channel_4);
	strcat(command,"P");
	strcat(command,s_pulse_4);
	strcat(command,"#");
	strcat(command,s_channel_5);
	strcat(command,"P");
	strcat(command,s_pulse_5);
	strcat(command,"T");
	strcat(command,s_time);
	strcat(command,"\r");
	
	int w;
	if((w = write(ssc32u,command,strlen(command)+1)) < 0){
		fprintf(stderr, "Error while writing to serial device\n");
		return -1;
	}
	
	close(ssc32u);
	
	usleep((time+1000)*1000);
	
	return 1;
}

int main(void){
	/*TEST SERVO COMMANDS HERE*/
	
	//servo_command1(WRIST_CH, 2000, 5000);
	
	//servo_command1(WRIST_CH, 2000, 5000);
	//servo_command1(ELBOW_CH, 775, 5000);
	//servo_command1(ELBOW_CH, 775, 5000);
	//servo_command1(ELBOW_CH, 1540, 5000);
	//servo_command2(ELBOW_CH, 1800, SHOULDER_CH, 1450, 5000);
	//servo_command2(ELBOW_CH, 1570, SHOULDER_CH, 1450, 1000);
	//servo_command2(ELBOW_CH, 735, SHOULDER_CH, 1450, 2000);
		//21.376 - 180 deg
		
	//servo_command1(SHOULDER_CH, 1450, 1000);
	//servo_command1(SHOULDER_CH, 2200, 5000);
		//0 - 180 deg
		
	//servo_command1(WRIST_CH, 775, 1000);
	servo_command1(WRIST_CH, 500, 1000);
	//servo_command1(WRIST_CH, 500, 1000);
		//
	
	//servo_command1(BASE_CH, 500, 5000);
	//servo_command1(BASE_CH, 2500, 5000);
	
	
	//servo_command4(BASE_CH, 1500, SHOULDER_CH, 1415, ELBOW_CH, 1360, WRIST_CH, 775, 2000);
	
	return 1;
}
