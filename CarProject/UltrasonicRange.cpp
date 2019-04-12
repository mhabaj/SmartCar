//gcc main.cpp -o mainude -L/usr/local/lib -lwiringPi -lpthread

#include <wiringPi.h>
#include <stdio.h>
#include <sys/time.h>

using namespace std;

class Sonar {

private:
	const int puceEnvoie = 23;
	const int puceReception = 24;
	const int MAX_DISTANCE = 60; //50 à cause des pertes et des faux-positives.
	const int timeOut = MAX_DISTANCE * 60;// calculate timeout according to the maximum measured distanc
	long pingTime;
	float distance;



public:
	Sonar() {
	}

	int pulseIn(int pin, int level, int timeout)
	{
		struct timeval tn, t0, t1;
		long micros;
		gettimeofday(&t0, NULL);
		micros = 0;
		while (digitalRead(pin) != level)
		{
			gettimeofday(&tn, NULL);
			if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
			micros += (tn.tv_usec - t0.tv_usec);
			if (micros > timeout) return 0;
		}
		gettimeofday(&t1, NULL);
		while (digitalRead(pin) == level)
		{
			gettimeofday(&tn, NULL);
			if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
			micros = micros + (tn.tv_usec - t0.tv_usec);
			if (micros > timeout) return 0;
		}
		if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
		micros = micros + (tn.tv_usec - t1.tv_usec);
		return micros;
	}

	double getSonar() {
		digitalWrite(this->puceEnvoie, HIGH);
		delayMicroseconds(1000);
		digitalWrite(this->puceEnvoie, LOW);
		pingTime = pulseIn(this->puceReception, HIGH, this->timeOut);
		if (this->pingTime <= 1) {
			return -1.0; //trop proche ou trop loin
		}
		else {
			distance = pingTime * 340.0 / 2.0 / 10000.0;
			return distance;
		}

	}

	int setupSonar() {

		if (wiringPiSetupGpio() == -1) {
			printf("Erreur wiringPi Setup !");
			return 1;
		}
		pinMode(puceEnvoie, OUTPUT);
		pinMode(puceReception, INPUT);


	}



};



/*

int main() {


	double distance = 0.0;

	Sonar s1;
	s1.setupSonar();


	while (1) {
		distance = s1.getSonar();
		printf("Distance mesuree : %.2f cm\n", distance);
		delay(10);
	}
	return 1;
}*/



