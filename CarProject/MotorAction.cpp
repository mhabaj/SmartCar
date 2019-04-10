//POUR COMPILER: gcc main.cpp -o mainude -L/usr/local/lib -lwiringPi -lpthread


#include <wiringPi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <csignal>



#define pinbase 64			

////////////////////////////////////
#define motorPin1	5	//forward
#define motorPin2	6	//backward
//////////////////////////////////
#define rightPin 13     //right
#define leftPin	12		//left

using namespace std;




void motorForward() {
	digitalWrite(motorPin1, HIGH);
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
	printf("Forward...\n");
}
void motorBackward() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, HIGH);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
	printf("Backward...\n");

}
void motorStop() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(rightPin, LOW);
	digitalWrite(leftPin, LOW);
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
	digitalWrite(rightPin, LOW);
	digitalWrite(leftPin, HIGH);


	printf("LEFTForward...\n");
}

void motorRightBackward() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, HIGH);
	digitalWrite(rightPin, HIGH);
	digitalWrite(leftPin, LOW);


	printf("RightBackward...\n");
}

void motorLeftForward() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, HIGH);
	digitalWrite(rightPin, LOW);
	digitalWrite(leftPin, HIGH);


	printf("LEFTForward...\n");
}

int motorInitialisation() {

	if (wiringPiSetupGpio() == -1) { 
		printf("setup wiringPi failed !");
		return 1;
	}

	signal(SIGINT, CleanGPIO);
	pinMode(motorPin1, OUTPUT);
	pinMode(motorPin2, OUTPUT);
	pinMode(rightPin, OUTPUT);
	pinMode(leftPin, OUTPUT);
}

void CleanGPIO(int signum) {
	pinMode(motorPin1, INPUT);
	pinMode(motorPin2, INPUT);
	pinMode(leftPin, INPUT);
	pinMode(rightPin, INPUT);
	printf("nettoyage reussi");
	exit(signum);
}
/*
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
*/
