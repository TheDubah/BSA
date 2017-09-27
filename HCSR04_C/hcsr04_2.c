#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
 

 
#define TRIG 23
#define ECHOL 24
#define ECHOR 22
 
void setup() {
        wiringPiSetup();
        pinMode(TRIG, OUTPUT);
        pinMode(ECHOR, INPUT);
	pinMode(ECHOL, INPUT);
 
        //TRIG pin must start LOW
        digitalWrite(TRIG, LOW);
        delay(30);
}
 
double LgetCM() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(30);
        digitalWrite(TRIG, LOW);
 
        //Wait for echo start
        while(digitalRead(ECHOL) == LOW);
 
        //Wait for echo end
        long LstartTime = micros();
	
        while(digitalRead(ECHOL) == HIGH);
        long LtravelTime = micros() - LstartTime;
 
        //Get distance in cm
	
        double Ldistance = LtravelTime * 0.01715;
	Ldistance = Ldistance - .31; //error compensation
        return Ldistance;
}
 
double RgetCM(){
 	//Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(30);
        digitalWrite(TRIG, LOW);

        //Wait for echo start
        while(digitalRead(ECHOR) == LOW);

        //Wait for echo end
        long RstartTime = micros();

        while(digitalRead(ECHOR) == HIGH);
        long RtravelTime = micros() - RstartTime;

        //Get distance in cm

        double Rdistance = RtravelTime * 0.01715;
        Rdistance = Rdistance - .31; //error compensation
        return Rdistance;

}
int main(void) {
        setup();
	int i;
	double Raverage[5], Laverage[5];
	double Lresult, Rresult,Ravg = 0, Lavg=0;
	//LEFT SIDE
	for (i = 0; i < 5; i++){
		Lresult = LgetCM();
		Laverage[i] = Lresult;
        	//printf("LDistance: %.2lf cm\n", Lresult);
		delay(10);
 	}
	for (i = 0; i < 5; i++){
		Lavg = Lavg + Laverage[i];
	}
	Lavg = Lavg / 5;
	printf("LAverage distance: %.2lf\n", Lavg);

	//RIGHT SIDE
	for (i = 0; i < 5; i++){
                Rresult = RgetCM();
                Raverage[i] = Rresult;
                //printf("RDistance: %.2lf cm\n", Rresult);
                delay(10);
        }
        for (i = 0; i < 5; i++){
                Ravg = Ravg + Raverage[i];
        }
        Ravg = Ravg / 5;
        printf("RAverage distance: %.2lf\n", Ravg);

        return 0;
}
