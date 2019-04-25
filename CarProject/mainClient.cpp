

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



class Motor {
	//Parametres Wiring Pi moteurs:
#define motorPin1	5	//forward
#define motorPin2	6	//backward
#define leftPin	12	//gauche
#define rightPin 13	//droite
public:
	static void motorStop();
	static void CleanGPIO(int signum);
	static void resetGPIO();
	static void motorForward();
	static void motorBackward();
	static void motorRightForward();
	static void motorLeftForward();
	static int motorInitialisation();
	static void TakeAction(string action);
};
void Motor::motorStop()
{

	digitalWrite(motorPin1, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);


	printf("Stop...\n");
}
void Motor::CleanGPIO(int signum)
{
	motorStop();
	cout << "Arret en cours" << endl;

	pinMode(motorPin1, INPUT);
	pinMode(motorPin2, INPUT);
	pinMode(leftPin, INPUT);
	pinMode(rightPin, INPUT);
	sleep(2);
	cout << "nettoyage reussi \n" << endl;
	exit(signum);
}
void Motor::resetGPIO()
{
	pinMode(motorPin1, INPUT);
	pinMode(motorPin2, INPUT);
	pinMode(leftPin, INPUT);
	pinMode(rightPin, INPUT);
	cout << "Reset reussi" << endl;
}
void Motor::motorForward()
{
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
	digitalWrite(motorPin1, HIGH);

	printf("Forward...\n");
}
void Motor::motorBackward()
{
	digitalWrite(motorPin1, LOW);
	digitalWrite(leftPin, LOW);
	digitalWrite(rightPin, LOW);
	digitalWrite(motorPin2, HIGH);

	printf("Backward...\n");
}
void Motor::motorRightForward()
{
	digitalWrite(leftPin, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(rightPin, HIGH);
	this_thread::sleep_for(chrono::milliseconds(500));
	digitalWrite(motorPin1, HIGH);


	printf("RightForward...\n");
}
void Motor::motorLeftForward()
{

	digitalWrite(rightPin, LOW);
	digitalWrite(motorPin2, LOW);
	digitalWrite(leftPin, HIGH);
	this_thread::sleep_for(chrono::milliseconds(500));
	digitalWrite(motorPin1, HIGH);

	printf("LEFTForward...\n");
}
int Motor::motorInitialisation()
{
	if (wiringPiSetupGpio() == -1) { //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return -1;
	}

	signal(SIGINT, CleanGPIO);
	resetGPIO();
	pinMode(motorPin1, OUTPUT);//set mode for the pin
	pinMode(motorPin2, OUTPUT);
	pinMode(leftPin, OUTPUT);
	pinMode(rightPin, OUTPUT);
	return 1;
}
void Motor::TakeAction(string action)
{
	int n = std::stoi(action);
	cout << "ACTION RECUUUUUUUUUUUUUUUUUU" << n << endl;
	switch (n) {
	case 1: Motor::motorForward();  break;
	case 2: Motor::motorBackward(); break;
	case 3: Motor::motorRightForward(); break;
	case 4: Motor::motorLeftForward(); break;
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
	CarClientSocket(int portnum);
	bool initSoc();

	void msgEnvoie(string s);

	string msgRecv();
	bool retIsConnected();
};
CarClientSocket::CarClientSocket(int portnum){
	this->portnum = portnum;


}
bool CarClientSocket::initSoc()
{
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Erreur socket");
		this->isConnected = false;
		exit(0);
	}
	else
		printf("socket = %d connexion possible sur le port %d\n", sd, portnum);


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
	return true;
}
void CarClientSocket::msgEnvoie(string s)
{
	strcpy(this->msg, s.c_str());
	if ((ret = send(sd, msg, s.length(), 0)) == -1) {
		perror("Erreur sendto");
	}
	else {
		printf("sendto = %d; msg = %s\n", ret, msg);
		this_thread::sleep_for(chrono::milliseconds(10));

	}
}
string CarClientSocket::msgRecv()
{
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
		printf(" Communication avec serveur interrompu \n");
		shutdown(sd, SHUT_RDWR);
		close(sd);
		Motor::CleanGPIO(0);

	}
	exit(1);
}
bool CarClientSocket::retIsConnected()
{
	return this->isConnected;
}









class IRSensor {
private:
	int irReception = 4;  //12 cm
	CarClientSocket* SocketIR;
public:
	IRSensor(CarClientSocket& SocketIR);
	int irSetup();
	int isObstacle();
	void sendIRSensorData(int irData);
};
IRSensor::IRSensor(CarClientSocket& SocketIR) {

	this->SocketIR = &SocketIR;

}
int IRSensor::irSetup()
{
	if (wiringPiSetupGpio() == -1) {
		printf("Erreur WIring Pi IRSensor!\n");
		return 1;
	}

	pinMode(irReception, INPUT);
}
int IRSensor::isObstacle() {
	delay(35);

	if (digitalRead(irReception) == 0) {
		delay(25);
		if (digitalRead(irReception) == 0) {
			printf("Detected Barrier !\n");
			return 1;
		}
		else {
			return 0;
		}
	}
}
void IRSensor::sendIRSensorData(int irData) {


	SocketIR->msgEnvoie(to_string(irData));

}








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
	Sonar(CarClientSocket& SocketSonar);
	int pulseIn(int pin, int level, int timeout);
	double getSonar();
	int setupSonar();
	int retSonarData();
	void sendSonarData(int SonarData);
};
Sonar::Sonar(CarClientSocket& SocketSonar) {
	this->SocketSonar = &SocketSonar;
}
int Sonar::pulseIn(int pin, int level, int timeout)
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
double Sonar::getSonar()
{
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
int Sonar::setupSonar()
{
	if (wiringPiSetupGpio() == -1) {

		("Erreur wiringPi Setup !");
		return 1;
	}
	pinMode(puceEnvoie, OUTPUT);
	pinMode(puceReception, INPUT);
}
int Sonar::retSonarData()
{
	double sonarvalue = getSonar();
	if (sonarvalue <= 40 && sonarvalue > -1) {
		return 1;
		//SocketSonar->msgEnvoie("1");
	}
	else {
		return 0;

		//SocketSonar->msgEnvoie("0");
	}
}
void Sonar::sendSonarData(int SonarData)
{
	SocketSonar->msgEnvoie(to_string(SonarData));

}







class Vehicule {
private:
	CarClientSocket* socketIO;
	IRSensor* irSensor;
	Sonar* sonar;
	bool status;
public:
	Vehicule(CarClientSocket& socketIO,
		IRSensor& irSensor, Sonar& sonar);
	int prepareComponents();
	void startup();
	void doMovements();
	bool sendInfo();
	~Vehicule();
};
bool Vehicule::sendInfo() {
	int isObstacleReact = irSensor->isObstacle();
	int isSonarReact = sonar->retSonarData();
	if (isObstacleReact == 1) {
		irSensor->sendIRSensorData(1);
		return true;
	}
	else if (isSonarReact == 1) {
		sonar->sendSonarData(1);
	}
	else {
		irSensor->sendIRSensorData(0);

	}
}
Vehicule::~Vehicule()
{
	Motor::resetGPIO();
	delete socketIO;
	delete irSensor;
	delete sonar;

	cout << "Vehicule detruit" << endl;



}
void Vehicule::doMovements() {
	string actionTemp = socketIO->msgRecv();
	string action = actionTemp.substr(actionTemp.length() - 1, 1);
	cout << "action demandee: " << action << endl;
	Motor::TakeAction(action);

}
void Vehicule::startup() {

	if (this->prepareComponents() == 1) {
		while (1) {
			this->sendInfo();
			this->doMovements();
		}
	}
	else {
		cout << "PrepareComponents Error" << endl;
		exit(0);
	}
}
int Vehicule::prepareComponents() {
	cout << "preparing Components.. " << endl;

	Motor::motorInitialisation();
	irSensor->irSetup();
	sonar->setupSonar();
	if (socketIO->initSoc() == true) {
		this->status = true;
		sleep(2);

		cout << "Ready. " << endl;
		return 1;
	}
	else return 0;



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

int main(void)
{

	CarClientSocket socketIO(27016);
	//Sonar sonar(socketIO);
		socketIO.initSoc();

	IRSensor ir1(socketIO);
	ir1.irSetup();

	//while (1) {

		ir1.isObstacle();

	//}



	return 0;
}

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

