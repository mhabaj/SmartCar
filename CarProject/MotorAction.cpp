//POUR COMPILER: gcc main.cpp -o mainude -L/usr/local/lib -lwiringPi -lpthread


#include <wiringPi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <csignal>



#define pinbase 64			//any number above 64


#define motorPin1	5	//forward
#define motorPin2	6	//backward
//////////////////////////////////
#define leftPin	12
#define rightPin 13

using namespace std;

void CleanGPIO(int signum) {
	pinMode(motorPin1, INPUT);
	pinMode(motorPin2, INPUT);
	pinMode(leftPin, INPUT);
	pinMode(rightPin, INPUT);
	printf("nettoyage reussi");
	exit(signum);
}


//motor function: determine the direction and speed of the motor according to the ADC
void motorForward() {
	digitalWrite(motorPin1, HIGH);
	digitalWrite(motorPin2, LOW);
	printf("Forward...\n");
}
void motorBackward() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, HIGH);
	printf("Backward...\n");

}
void motorStop() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
	printf("Stop...\n");
}

void motorRightForward() {
	digitalWrite(motorPin1, HIGH);
	digitalWrite(motorPin2, LOW);
	digitalWrite(rightPin, HIGH);
	digitalWrite(leftPin, LOW);


	printf("RightForward...\n");
}
void motorLeftForward() {
	digitalWrite(motorPin1, HIGH);
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, HIGH);
	digitalWrite(rightPin, LOW);


	printf("LEFTForward...\n");
}


int motorInitialisation() {

	if (wiringPiSetupGpio() == -1) { //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1;
	}

	signal(SIGINT, CleanGPIO);
	pinMode(motorPin1, OUTPUT);//set mode for the pin
	pinMode(motorPin2, OUTPUT);
	pinMode(leftPin, OUTPUT);
	pinMode(rightPin, OUTPUT);
}

int main(void) {

	motorInitialisation();
	while (1) {
		motorForward();
		sleep(3);
		motorBackward();
		sleep(3);
		motorRightForward();
		sleep(3);
		motorLeftForward();
		sleep(3);

	}

	return 0;
}

