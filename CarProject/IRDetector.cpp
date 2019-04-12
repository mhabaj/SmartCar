/*
//gcc main.cpp -o mainude -L/usr/local/lib -lwiringPi -lpthread

#include <wiringPi.h>
#include <stdio.h>

#define ObstaclePin 18  //12 cm


class IRSensor {
private:

	int irReception = 18;  //12 cm

public:
	IRSensor() {
	}
	int irSetup() {
		if (wiringPiSetupGpio() == -1) {
			printf("Erreur WIring Pi IRSensor!\n");
			return 1;
		}

		pinMode(irReception, INPUT);
	}

	bool isObstacle() {
		delay(35);

		if (digitalRead(irReception) == 0) {
			delay(25);
			if (digitalRead(irReception) == 0) {
				printf("Detected Barrier !\n");
				return true;
			}
			else return false;
		}
		else return false;
	}
};

/*
int main(void)
{

	IRSensor s1;
	s1.irSetup();

	while (1) {

		s1.isObstacle();

	}



	return 0;
}
*/