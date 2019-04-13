#define WIN32_LEAN_AND_MEAN

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
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
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#define DEFAULT_BUFLEN 8
using namespace std;
using namespace cv;



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
		this_thread::sleep_for(chrono::milliseconds(50));
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

class ObjectScanner
{

private:

	const string CascadePanneauStop_XML = "C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml";
	const string CascadeFeuVert_XML = "C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string CascadeFeuRouge_XML = "C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	//const string videoSourceURL = "http://192.168.0.26:8080/video/jpeg";
	//const string videoSourceURL = "http://192.168.43.103:8081/";
	CascadeClassifier CascadePanneauStop, CascadeFeuVert, CascadeFeuRouge;
	VideoCapture VideoSource;
	vector<Rect> PanneauStopVec, FeuVertVec, FeuRougeVec;
	bool isStop = false;
	bool isGreen = false;
	bool isRed = false;

public:
	void resetDetection();
	VideoCapture lancerCam();
	ObjectScanner();
	int sceneScan();  // 3 si Stop, 4 si Vert, 5 si Rouge
	void returnFoundObjects(Mat frame, Mat Gray_Transform);

};
ObjectScanner::ObjectScanner()
{
	this->VideoSource = lancerCam();

}
int ObjectScanner::sceneScan()
{

	// ----Integrer les cascades:
	if (!this->CascadePanneauStop.load(this->CascadePanneauStop_XML))
	{
		cout << "ERREUR CHARGEMENT CASCADE SIGNAL STOP" << endl;
		return -1;
	};

	Mat frame;
	Mat frame_resized;
	Mat Gray_Transform;
	while (this->VideoSource.read(frame) && this->PanneauStopVec.size() <= 0
		&& this->FeuRougeVec.size() <= 0 && this->FeuVertVec.size() <= 0)
	{
		if (frame.empty())
		{
			cout << "ERREUR TRAME VIDE" << endl;
			break;
		}

		//---- On modifie la taille de l'image (pour les performances) et on applique les Classifiers 
		resize(frame, frame_resized, Size(200, 150));

		cout << this->PanneauStopVec.size() << "," << this->FeuVertVec.size() <<
			"," << this->FeuRougeVec.size() << endl;



		returnFoundObjects(frame_resized, Gray_Transform);

		// Ctrl C pour quitter
		if (waitKey(10) == 27)
		{
			break;
		}
	
	if (this->PanneauStopVec.size() > 0)
	{
		resetDetection();
		return 3;
	}
	//if (this->FeuVertVec.size() > 0)
	//{
	//	resetDetection();

	//	return 4;
	//}
	//if (this->FeuRougeVec.size() > 0)
	//{
	//	resetDetection();

	//	return 5;
	//}
	
	return 1;
}

}
void ObjectScanner::returnFoundObjects(Mat frame, Mat Gray_Transformed)
{
	//LE SOURCE DE LAG : cvtColor FONCTION POUR TRANSFORMER LIMAGE EN NIVEAUX DE GRIS
//this->VideoSource >> frame;
	cvtColor(frame, Gray_Transformed, COLOR_RGB2GRAY);

	// DETECT MULTISCALE POUR LES STOPS 
	this->CascadePanneauStop.detectMultiScale(Gray_Transformed, this->PanneauStopVec);

	for (size_t i = 0; i < this->PanneauStopVec.size(); i++)
	{
		//this->isStop = false;
		rectangle(frame, this->PanneauStopVec[i], Scalar(0, 100, 0));

	}

	// DETECT MULTISCALE POUR LES FEUX VERTS 
	/*
	this->CascadePanneauStop.detectMultiScale(Gray_Transformed, this->FeuVertVec);
	for (size_t i = 0; i < this->FeuVertVec.size(); i++)
	{
		rectangle(frame, this->FeuVertVec[i], Scalar(0, 100, 0));
	}
	// DETECT MULTISCALE POUR LES FEUX ROUGES
	this->CascadePanneauStop.detectMultiScale(Gray_Transformed, this->FeuRougeVec);
	for (size_t i = 0; i < this->FeuRougeVec.size(); i++)
	{
		rectangle(frame, this->FeuRougeVec[i], Scalar(0, 100, 0));
	}
	*/
	imshow("", frame);

}
void ObjectScanner::resetDetection() {
	this->PanneauStopVec.clear();
	this->FeuVertVec.clear();
	this->FeuRougeVec.clear();
	this->isStop = false;
	this->isGreen = false;
	this->isRed = false;
	//Sleep(80);

}
VideoCapture ObjectScanner::lancerCam()
{
	//----Lancer le flux video


	VideoCapture camTemp; //pour webcam locale
	camTemp.open(0);
	//camTemp.open(this->videoSourceURL); //(pour la video par internet)
	if (!camTemp.isOpened())
	{
		cout << "ERREUR CHARGEMENT FLUX VIDEO" << endl;
		exit(0);
	}
	return camTemp;
}


class Vehicule {

private:
	CarServerSocket* SocSend;
	ObjectScanner objDetection;

public:
	//1 forward, 2 backward, 3 right, 4 left, 5 stop
	Vehicule(CarServerSocket& carSoc);
	void forward();
	void backward();
	void right();
	void left();
	void stop();
	void goSmart();
};
Vehicule::Vehicule(CarServerSocket& carSoc)
{
	this->SocSend = &carSoc;
}
void  Vehicule::forward()
{
	this->SocSend->msgEnvoi("1");
}
void Vehicule::backward()
{
	this->SocSend->msgEnvoi("2");
}
void Vehicule::right()
{
	this->SocSend->msgEnvoi("3");
}
void Vehicule::left()
{
	this->SocSend->msgEnvoi("4");
}
void Vehicule::stop()
{
	this->SocSend->msgEnvoi("5");
}
void Vehicule::goSmart()
{		
	while (1) {
		int foundObj = this->objDetection.sceneScan();
		cout << "foundObj data : " << foundObj << endl;
				switch (foundObj) {
					case 1 : this->forward(); cout << "RAS" << endl; break;
					case 3 : this->stop(); cout << "STOP DETECTEE" << endl; break;
					case 4 : this->right(); cout << "TOURNER A DROITE" << endl; break;
					case 5 : this->left(); cout << "TOURNER A GAUCHE" << endl; break;

					default: this->stop(); break;
				}
		
	}

}


int main() {


	//on creer le socket d'envoie
	string portSend = "27016";
	CarServerSocket SocVoiture(portSend);
	SocVoiture.initSoc();
	//on creer la voiture
	Vehicule v1(SocVoiture);

	//on crée la detection de mouvement
	v1.goSmart();

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
	ObjectScanner obj1;
	int i = 0;
	int foundObj;
	while (1) {
		 foundObj = obj1.sceneScan();
		if (foundObj == 3) {
			cout << "STOP DETECTEE" << endl;
		}
		else if (foundObj == 4) {
			cout << "FEUX VERT" << endl;
		}
		else if (foundObj == 5) {
			cout << "FEUX ROUGE" << endl;
		}
		i++;
		//obj1.resetDetection();
	}
return 0;
}
*/
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