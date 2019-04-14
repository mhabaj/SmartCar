

// Commandes compilation:
//gcc main.cpp -o mainExec -L/usr/local/lib -lwiringPi -lpthread    ---> IR Sensor
////gcc main.cpp -o mainExec -L/usr/local/lib -lwiringPi -lpthread   ---> Ultrasonic Sensor.
//gcc main.cpp - o mainExec - L / usr / local / lib - lwiringPi - lpthread  ---> MotorAction
//COMPILER  MAINCLIENT: g++ main.cpp -o mainExec -L/usr/local/lib -lwiringPi -lpthread


//////////////////////Debut programme:///////////////
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
#include <iostream>
#include <wiringPi.h>
#include <sys/time.h>
#include <math.h>
#include <sys/types.h>
#include <csignal>

using namespace std;

//Server Connection settings:
const char hostname[] = "192.168.43.244";



class CarClientSocket {
private:

	int sd;  // n° fd de la socket
	int ret;
	struct hostent *host;
	struct sockaddr_in addrsrv;
	char msg[128];
	bool isConnected;
	int portnum;

public:
	CarClientSocket(int portnum) {
		this->portnum = portnum;

	}
	void initSoc() {


		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("Erreur socket");
			this->isConnected = false;
			exit(0);
		}
		else
			printf("socket = %d connectee sur le port %d\n", sd, portnum);


		if ((host = gethostbyname(hostname)) == NULL) {
			herror("gethostbyname");
			this->isConnected = false;
			exit(-1);
		}

		bcopy(host->h_addr, (char *)&addrsrv.sin_addr, host->h_length);
		addrsrv.sin_family = AF_INET;
		addrsrv.sin_port = htons(portnum);

		connect(sd, (const struct sockaddr*)&addrsrv, sizeof(addrsrv));
		this->isConnected = true;

	}

	void msgEnvoie(string s) {
		strcpy(this->msg, s.c_str());
		if ((ret = send(sd, msg, s.length(), 0)) == -1) {
			perror("Erreur sendto");
		}
		else {
			printf("sendto = %d; msg = %s\n", ret, msg);
			this_thread::sleep_for(chrono::milliseconds(40));

		}
	}

	string msgRecv() {
		string msgRecu = "";
		if ((ret = recv(sd, msg, sizeof(msg), 0)) == -1) {
			perror("Erreur recvfrom");
		}
		if (this->ret > 0) {

			for (int i = 0; i < this->ret; i++) {
				msgRecu += this->msg[i];
			}
			return msgRecu;

		}
		else if (ret == 0) printf("femerutre de la connexion.....\n");

		else {
			printf("Erreur recv (COnnexion perdu avec le serveur?");
			shutdown(sd, SHUT_RDWR);
			close(sd);
		}
		exit(1);

	}
	bool retIsConnected() {
		return this->isConnected;
	}
};

class IRSensor {
private:

	int irReception = 18;  //12 cm
	CarClientSocket* SocketIR;


public:
	IRSensor(CarClientSocket& SocketIR) {

		this->SocketIR = &SocketIR;

	}
	int irSetup() {
		if (wiringPiSetupGpio() == -1) {
			printf("Erreur WIring Pi IRSensor!\n");
			return 1;
		}

		pinMode(irReception, INPUT);
	}

	string isObstacle() {
		delay(35);

		if (digitalRead(irReception) == 0) {
			delay(25);
			if (digitalRead(irReception) == 0) {
				printf("Detected Barrier !\n");
				return "obstacle";
			}
			else return "ras";
		}
	}

	void sendIRSensorData() {

		SocketIR->msgEnvoie((isObstacle()));

	}
};

class Sonar {

private:
	const int puceEnvoie = 23;
	const int puceReception = 24;
	const int MAX_DISTANCE = 60; //50 à cause des pertes et des faux-positives.
	const int timeOut = MAX_DISTANCE * 60;// calculate timeout according to the maximum measured distanc
	long pingTime;
	float distance;
	CarClientSocket* SocketSonar;



public:
	Sonar(CarClientSocket& SocketSonar) {
		this->SocketSonar = &SocketSonar;
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

	void sendSonarData() {
		double sonarvalue = getSonar();
		if (sonarvalue <= 12 && sonarvalue > -1) {
			SocketSonar->msgEnvoie("1");
		}
		else {
			SocketSonar->msgEnvoie("ras");

		}





	}

};



class Motor {

	//Parametres Wiring Pi moteurs:
#define motorPin1	5	//forward
#define motorPin2	6	//backward
#define leftPin	12	//gauche
#define rightPin 13	//droite
public:

	static void CleanGPIO(int signum) {
		pinMode(motorPin1, INPUT);
		pinMode(motorPin2, INPUT);
		pinMode(leftPin, INPUT);
		pinMode(rightPin, INPUT);
		cout << "nettoyage reussi" << endl;
		exit(signum);
	}
	static void motorForward() {
		digitalWrite(motorPin2, LOW);
		digitalWrite(leftPin, LOW);
		digitalWrite(rightPin, LOW);
		digitalWrite(motorPin1, HIGH);

		printf("Forward...\n");
	}
	static void motorBackward() {
		digitalWrite(motorPin1, LOW);
		digitalWrite(leftPin, LOW);
		digitalWrite(rightPin, LOW);
		digitalWrite(motorPin2, HIGH);

		printf("Backward...\n");

	}
	static void motorStop() {
		digitalWrite(motorPin1, LOW);
		digitalWrite(motorPin2, LOW);
		digitalWrite(leftPin, LOW);
		digitalWrite(rightPin, LOW);


		printf("Stop...\n");
	}
	static void motorRightForward() {
		digitalWrite(leftPin, LOW);
		digitalWrite(motorPin2, LOW);
		digitalWrite(rightPin, HIGH);
		this_thread::sleep_for(chrono::milliseconds(500));
		digitalWrite(motorPin1, HIGH);


		printf("RightForward...\n");
	}
	static void motorLeftForward() {
		digitalWrite(rightPin, LOW);
		digitalWrite(motorPin2, LOW);
		digitalWrite(leftPin, HIGH);
		this_thread::sleep_for(chrono::milliseconds(500));
		digitalWrite(motorPin1, HIGH);

		printf("LEFTForward...\n");
	}
	static int motorInitialisation() {

		if (wiringPiSetupGpio() == -1) { //when initialize wiring failed,print messageto screen
			printf("setup wiringPi failed !");
			return -1;
		}

		signal(SIGINT, CleanGPIO);
		pinMode(motorPin1, OUTPUT);//set mode for the pin
		pinMode(motorPin2, OUTPUT);
		pinMode(leftPin, OUTPUT);
		pinMode(rightPin, OUTPUT);
		return 1;
	}

	static void TakeAction(string action) {
		int n = std::stoi(action);

		switch (n) {
		case 1: Motor::motorForward();  break;
		case 2: Motor::motorBackward(); break;
		case 4: Motor::motorLeftForward(); break;
		case 3: Motor::motorRightForward(); break;
		case 5: Motor::motorStop(); break;
		default: Motor::motorStop(); cout << "Commande deplacement invalide" << endl; break;


		}
		///////OU BIEN///////
		if (n == 1) {
			Motor::motorForward();

		}
		else if (n == 2) {
			Motor::motorBackward();

		}
		else if (n == 3) {
			Motor::motorRightForward();

		}
		else if (n == 4) {
			Motor::motorLeftForward();

		}
		else if (n == 5) {
			Motor::motorStop();

		}
		else {
			Motor::motorStop(); cout << "Commande deplacement invalide" << endl;
		}

	}
};







class Vehicule {

private:

	CarClientSocket* socketIO;
	IRSensor* irSensor;
	Sonar* sonar;
	bool status;


public:

	Vehicule(CarClientSocket& socketIO,
		IRSensor& irSensor, Sonar& sonar);

	void prepareComponents();
	void startup();

	void doMovements();


};

void Vehicule::doMovements() {
	string action = socketIO->msgRecv();

	cout << action << endl;
	Motor::TakeAction(action);

}

void Vehicule::startup() {

	this->prepareComponents();
	while (1) {
		this->sonar->sendSonarData();
		//this->irSensor->sendIRSensorData();
		this->doMovements();
	}
}





void Vehicule::prepareComponents() {

	Motor::motorInitialisation();
	irSensor->irSetup();
	sonar->setupSonar();
	socketIO->initSoc();
	this->status = true;
	cout << "Components Ready " << endl;

}
Vehicule::Vehicule(CarClientSocket & socketIO, IRSensor & irSensor, Sonar & sonar)
{

	this->socketIO = &socketIO;
	this->irSensor = &irSensor;
	this->sonar = &sonar;

}


int main(void) {




	CarClientSocket socketIO(27016);
	Sonar sonar(socketIO);
	IRSensor ir1(socketIO);
	Vehicule v1(socketIO, ir1, sonar);

	v1.startup();

	return 0;
}


//main pour tester motor.
/*


int main(void) {

	Motor::motorInitialisation();
	while (1) {
		// motorForward();
		 //sleep(3);
		Motor::motorStop();
		Motor::motorBackward();
		sleep(2);
		Motor::motorStop();
		Motor::motorLeftForward();
		sleep(2);
		//motorStop();
		//sleep(1);
		Motor::motorRightForward();
		sleep(2);


		Motor::motorStop();
		sleep(10);

		//motorBackward();
		//sleep(5);

	}

	return 0;
}
*/
//main UltraSonic Sensor:
/*
int main() {

	CarClientSocket socketIO(27016);
	Sonar s1(socketIO);
	double distance = 0.0;

	s1.setupSonar();


	while (1) {
		distance = s1.getSonar();
		printf("Distance mesuree : %.2f cm\n", distance);
	}
	return 1;
}


//main pour tester IR sensor
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
//Main pour tester sockets.
/*
//main test client
int main(void) {
	CarClientSocket c1;
	int i = 1;
	c1.initSoc();
	while (c1.retIsConnected()) {


		//c1.msgEnvoie("test????");
		cout << i << "message recu : " << c1.msgRecv() << endl;
		i++;
	}
	return 0;
}
*/


