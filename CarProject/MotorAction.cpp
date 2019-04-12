/*
//POUR COMPILER: gcc main.cpp -o mainude -L/usr/local/lib -lwiringPi -lpthread


#include <wiringPi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <csignal>
#include <chrono>
#include <thread>





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


void motorForward() {
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
	digitalWrite(motorPin1, HIGH);

	printf("Forward...\n");
}
void motorBackward() {
	digitalWrite(motorPin1, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
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
	digitalWrite(leftPin, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(rightPin, HIGH);
	this_thread::sleep_for(chrono::milliseconds(500));
	digitalWrite(motorPin1, HIGH);


	printf("RightForward...\n");
}
void motorLeftForward() {
	digitalWrite(rightPin, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, HIGH);
	this_thread::sleep_for(chrono::milliseconds(500));
	digitalWrite(motorPin1, HIGH);

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


/*
int main(void) {

	motorInitialisation();
	while(1){
   // motorForward();
	//sleep(3);
	motorStop();
	motorBackward();
	sleep(2);
	motorStop();
	motorLeftForward();
	sleep(2);
	//motorStop();
	//sleep(1);
	motorRightForward();
	sleep(2);


	motorStop();
	sleep(10);

	//motorBackward();
	//sleep(5);

}

	return 0;
}
*/


