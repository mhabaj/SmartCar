#define WIN32_LEAN_AND_MEAN

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <ctime>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#define DEFAULT_BUFLEN 56
using namespace std;
using namespace cv;
const string Fenetre = "Class Reconnaissance objets.";


int ScannerStatus = 1 ; // 1 pour rien , 3 pour Stop, 4 feu Rouge


class CarServerSocket {

private:
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	char* DEFAULT_PORT;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	char sendBuffer[DEFAULT_BUFLEN];


public:

	CarServerSocket(string DEFAULT_PORT) {
		char * cstr = new char[DEFAULT_PORT.length() + 1];
		strcpy_s(cstr, 6, DEFAULT_PORT.c_str());
		this->DEFAULT_PORT = cstr;
	}

	int  initSoc() {


		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("Erreur initialisation Socket WSA : %d\n", iResult);
			return 1;
		}

		//les params de la socket:
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Initialisaation address local du serveur et le PORT.
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("erreur dans la fonction getaddrinfo: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Creation de socket:
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Initialisation du protocole TCP du Socket:
		iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("erreur bind(): %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("Erreur listen(): %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		// Lors qu'un client tent de se connecter:
		if (ClientSocket = accept(ListenSocket, NULL, NULL)) printf("\nclient initialisee \n");
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		// On met la socket en Attente
		closesocket(ListenSocket);
		cout << "Chargement.." << endl;
		Sleep(2000);
		return 0;
	}

	string msgRecu() {

		string msg = "";
		this->iResult = recv(this->ClientSocket, this->recvbuf, this->recvbuflen, 0);

		if (this->iResult > 0)
		{


			for (int i = 0; i < this->iResult; i++)
			{
				msg += this->recvbuf[i];
			}
			return msg;



		}

		else if (iResult == 0)
			printf("femerutre de la connexion.....\n");

		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();

		}

		exit(1);
	}

	void msgEnvoi(string msgEnvoie) {
		//le message sera 1 ou 2 ou 3 donc char[1]
		//char envoiebuff[1] = { msg };

		int n = msgEnvoie.length();
		strcpy_s(this->sendBuffer, msgEnvoie.c_str());
		// on Envoie le message
		iSendResult = send(ClientSocket, this->sendBuffer, n, 0);
		this_thread::sleep_for(chrono::milliseconds(35));
		//closesocket(ClientSocket);

	}

	int returniResult() {
		return this->iResult;
	}

	~CarServerSocket() {

		closesocket(ClientSocket);
		WSACleanup();
		cout << "\n SocketDetruit \n" << endl;
	}
};





class ObjectScanner : public DetectionBasedTracker::IDetector
{

private:
	vector<Rect> PanneauStopVec, FeuTraffic;
	string ClassifierTrainingStop ="C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml" ; //URL VERS LES DONNEES DE LENTRAINEMENT stop.
	string ClassifierTrainingFeuRouge = "C:/Users/mhaba/OneDrive/Desktop/PanneauFeuxRouge.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT feu rouge.

	
	VideoCapture VideoStream;
	Ptr<CascadeClassifier> Detector;



public:
	int retScannerStatus() {
		//return this->ScannerStatus;
	}
	VideoCapture lancerCam()
	{
		VideoCapture camTemp; //pour webcam locale
		//camTemp.open(0);
		camTemp.open("http://192.168.43.109:8080/video");
		//camTemp.open("http://192.168.43.103:8081/");
		//camTemp.open(this->videoSourceURL); //(pour la video par internet)
		if (!camTemp.isOpened())
		{
			cout << "ERREUR CHARGEMENT FLUX VIDEO" << endl;
			exit(0);
		}
		return camTemp;
	}

	ObjectScanner() {
		//namedWindow(Fenetre);
		//this->VideoStream = lancerCam();
	};
	ObjectScanner(Ptr<CascadeClassifier> detector) : IDetector(), Detector(detector)
	{
		CV_Assert(detector);
	}
	//detectMultiScale permet de retourner les objets trouvees sous une liste de rectangles.
	void detect(const Mat &Image, std::vector<Rect> &objects)
	{
		Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);

	}
	bool isVideoStreamOpened(VideoCapture VideoStream) {

		if (!VideoStream.isOpened())
		{
			printf("Erreur Camera\n");
			return false;
		}
		else
			return true;
	}
	void resetDetection() {

		this->PanneauStopVec.clear();
		this->FeuTraffic.clear();
		//Sleep(20);
	}
	int sceneScan() {
		this->VideoStream = lancerCam();
		namedWindow(Fenetre);


		if (isVideoStreamOpened(VideoStream)) {
			//on include le .XML de l'entrainement
			std::string fichierXmlCascadeStop = samples::findFile(this->ClassifierTrainingStop);
			std::string fichierXmlCascadeRouge = samples::findFile(this->ClassifierTrainingFeuRouge);

			//pointeur sur objet CascadeClassifier :
			Ptr<CascadeClassifier> cascadeStop = makePtr<CascadeClassifier>(fichierXmlCascadeStop);
			Ptr<CascadeClassifier> cascadeFeuxRouge = makePtr<CascadeClassifier>(fichierXmlCascadeRouge);

			Ptr<DetectionBasedTracker::IDetector> detectStop = makePtr<ObjectScanner>(cascadeStop);
			Ptr<DetectionBasedTracker::IDetector> detectFeuxRouge = makePtr<ObjectScanner>(cascadeFeuxRouge);


			cascadeStop = makePtr<CascadeClassifier>(fichierXmlCascadeStop);
			cascadeFeuxRouge = makePtr<CascadeClassifier>(fichierXmlCascadeRouge);

			Ptr<DetectionBasedTracker::IDetector> DetecteurStop = makePtr<ObjectScanner>(cascadeStop);
			Ptr<DetectionBasedTracker::IDetector> DetecteurFeuxRouge = makePtr<ObjectScanner>(cascadeFeuxRouge);
			DetectionBasedTracker::Parameters params;
			DetectionBasedTracker DetectorStop(detectStop, DetecteurStop, params);
			DetectionBasedTracker DetectorFeuxRouge(detectFeuxRouge, DetecteurFeuxRouge, params);

			Mat RFrame; 
			Mat WFrame;
			Mat gaussBlur;
			
				while (waitKey(30) < 0)
				 {
					VideoStream >> RFrame;
					cvtColor(RFrame, WFrame, COLOR_BGR2GRAY);
					//traitement pour feux rouge

					DetectorStop.process(WFrame);
					DetectorStop.getObjects(PanneauStopVec);
					//traitement pour feux rouges
					//GaussianBlur(WFrame, gaussBlur, Size(25, 25), 0);
					DetectorFeuxRouge.process(WFrame);
					DetectorFeuxRouge.getObjects(FeuTraffic);

					for (size_t i = 0; i < PanneauStopVec.size(); i++)
					{
						rectangle(RFrame, PanneauStopVec[i], Scalar(0, 100, 0));
					}
					for (size_t i = 0; i < FeuTraffic.size(); i++)
					{
						rectangle(RFrame, FeuTraffic[i], Scalar(0, 100, 0));
					}
					imshow(Fenetre, RFrame);
					 if (this->PanneauStopVec.size() > 0)
					{

						 resetDetection();

							 ScannerStatus = 3;
					}
					 else if (this->FeuTraffic.size() > 0)
					 {

						 resetDetection();

						 ScannerStatus = 4;
					 }
					 else if (this->PanneauStopVec.size() <= 0 && this->FeuTraffic.size() <= 0) {
							resetDetection();
							ScannerStatus = 1;							
						 }
					 if (waitKey(10) == 27)
					 {
						 break;
					 }
				}
			
			
			resetDetection();
			DetectorStop.stop();
			DetectorFeuxRouge.stop();

			
		}
		return 0;
	}
	


	virtual ~ObjectScanner()
	{}
};






class Vehicule {

private:
	CarServerSocket* carSoc;
	ObjectScanner* objDetection;

public:
	//1 forward, 2 backward, 3 right, 4 left, 5 stop
	Vehicule(CarServerSocket& carSoc, ObjectScanner& objDetection);
	void forward();
	void backward();
	void right();
	void left();
	void stop();
	int returnIfObstacle();
	void stopSignManeuver();
	void redTrafficLightManeuver();
	int goSmart();
};
Vehicule::Vehicule(CarServerSocket& carSoc, ObjectScanner& objDetection)
{
	this->carSoc = &carSoc;
	this->objDetection = &objDetection;
}
void  Vehicule::forward()
{
	this->carSoc->msgEnvoi("1");
}
void Vehicule::backward()
{
	this->carSoc->msgEnvoi("2");
}
void Vehicule::right()
{
	this->carSoc->msgEnvoi("3");
}
void Vehicule::left()
{
	this->carSoc->msgEnvoi("4");
}
void Vehicule::stop()
{
	this->backward(); //pour arreter les roues rapidement
	this->carSoc->msgEnvoi("5");
}
int Vehicule::returnIfObstacle() {
	//return 1 si obstacle IR, 2 si Sonar, 0 sinon 
	string s = this->carSoc->msgRecu();
	//char s_array[DEFAULT_BUFLEN];
	//char ss_array[9] = "obstacle";
	//strcpy_s(s_array, s.c_str());	

	string infoRecv = s.substr(s.length()-1, 1);
	cout << "INFO RECIEVED: " << infoRecv << endl;
	 if (infoRecv == "1") {
		printf("Obstacle Detectee \n");
		return 1;
	}
	else return 0;
}

void Vehicule::stopSignManeuver()
{
	Sleep(300);
	this->stop();
	Sleep(2000);
	while (returnIfObstacle() == 1) {
		cout << "Obstacle detectee, On reste sur place" << endl;
	}
	this->forward();

	Sleep(200);
}

void Vehicule::redTrafficLightManeuver()
{
	this->stop();
	while (ScannerStatus == 4) {Sleep(50);}
	while (returnIfObstacle() == 1) {
		cout << "Obstacle detectee, On reste sur place" << endl;
	}
	this->forward();
	Sleep(50);
}



int Vehicule::goSmart()
{
	cout << "Preparing components" << endl;
	Sleep(3000);

	while (1)
	{	
		int isObstacle = returnIfObstacle();
		int z = -2;
	
		 if (isObstacle != 1)
		{
					
			 if (ScannerStatus == 3 || ScannerStatus == 1 || ScannerStatus == 4) {
				 z = ScannerStatus;
			 }
			
				switch (z)
				{
					case 1: this->forward(); cout << "RAS" << endl; break;
					case 3:  cout << "STOP DETECTEE" << endl; this->stopSignManeuver(); break;
					case 4: this->stop(); cout << "FEU DETECTEE" << endl; break;
					case 5: this->stop(); cout << "TOURNER A GAUCHE" << endl; break;
					default: this->stop(); break;
				}
			
		 }
		 else {
			 this->backward();
			 Sleep(2000);
		 }
		
	}
}


int main() {


	//on creer le socket d'envoie
	string portSend = "27016";
	CarServerSocket SocVoiture(portSend);
	SocVoiture.initSoc();

	
	ObjectScanner s1;
	Vehicule v1(SocVoiture, s1);

	std::thread v(&Vehicule::goSmart, v1);
	std::thread t(&ObjectScanner::sceneScan, s1);
	
	
	v.join();
	t.join();
	
	
	
	//v1.goSmart();
	

	//on crée la detection de mouvement
	
	

	return 0;
}




//main pour tester sockets
/*  TEST SERVEUR:
int main(void)
{
	///////////:RECEPTION
	string portRecv = "27015";
	CarServerSocket SocRecv(portRecv);
	//string portSend = "27016";
	//CarServerSocket SocSend(portSend);
	while (SocRecv.initSoc() == 0)
	{
		string s;
		while (1)
		{
			s = SocRecv.msgRecu();
			cout << "String recu:  " << s << endl;
		}
	}
	SocRecv.~CarServerSocket();
/////////////////ENVOIE://///////////////////
	string portSend = "27016";
	char mymsg[10] = "blabla";
	CarServerSocket SocSend(portSend);
	int i = 0;
	while (SocSend.initSoc() == 0) {
		while (i < 600) {
		SocSend.msgEnvoi(mymsg);
		i++;
		}
	}
	return 0;
}
*/





//main pour tester detection objts.
/*
int main()
{
	//string StopTrainPATH = "C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml";

	ObjectScanner StopDetect;
	//StopDetect.setupObjectScan();
	int s;
	while (1) {

		s= StopDetect.sceneScan();
		cout << "VOILAAAAAAAAAAAAAAAAAAAAAAAAA" << s << endl;
		//Sleep(5000);

	}

		
return 0;
}

/* //pour testet Sockets
int main(void)
{
	string portRecv = "27015";
	CarServerSocket SocRecv(portRecv);
	//string portSend = "27016";
	//CarServerSocket SocSend(portSend);
	while (SocRecv.initSoc() == 0)
	{
		string s;
		while (1)
		{
			s = SocRecv.msgRecu();
			cout << "String recu:  " << s << endl;
		}
	}
	SocRecv.~CarServerSocket();
	return 0;
}
*/