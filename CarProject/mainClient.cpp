
/*
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
		delay(35);

		digitalWrite(this->puceEnvoie, HIGH);
		delayMicroseconds(35);
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


#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <thread>


const char hostname[] = "192.168.43.244";
const int portnum = 27015;

using namespace std;

class CarClientSocket {
private:

	int sd;  // n° fd de la socket
	int ret;
	struct hostent *host;
	struct sockaddr_in addrsrv;
	char msg[128];

public:

	void initSoc() {


		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			perror("Erreur socket");
		else
			printf("socket = %d\n", sd);

		if ((host = gethostbyname(hostname)) == NULL) {
			herror("gethostbyname");
			exit(-1);
		}

		bcopy(host->h_addr, (char *)&addrsrv.sin_addr, host->h_length);
		addrsrv.sin_family = AF_INET;
		addrsrv.sin_port = htons(portnum);

		connect(sd, (const struct sockaddr*)&addrsrv, sizeof(addrsrv));

	}


	void msgEnvoie(string s) {



		int n = s.length();
		strcpy(this->msg, s.c_str());
		if ((ret = send(sd, msg, s.length(), 0)) == -1) {
			perror("Erreur sendto");
		}
		else {
			printf("sendto = %d; msg = %s\n", ret, msg);
			this_thread::sleep_for(chrono::milliseconds(30));

		}
	}

	int msgRecv() {
		if ((ret = recv(sd, msg, sizeof(msg), 0)) == -1) {
			perror("Erreur recvfrom");
		}
		else {
			msg[ret] = '\0'; 
			printf("recvfrom = %d; msg = %s\n", ret, msg);
		}

	}
};


/* //main test client
int main(void) {
	CarClientSocket c1;
	c1.initSoc();
	int i = 1;
	while (i < 5) {


		c1.msgEnvoie("test????");
		i++;
	}
	return 0;
}
*/
/*
int main() {


	double distance = 0.0;

	Sonar s1;
	s1.setupSonar();


	while (1) {
		distance = s1.getSonar();
		printf("Distance mesuree : %.2f cm\n", distance);
	}
	return 1;
}

*/




