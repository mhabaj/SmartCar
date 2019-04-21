#define DEFAULT_BUFLEN 56


#include "Vehicule.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <ctime>



using namespace std;
using namespace cv;


const int NothingDetected = 1;
const int stopDetected = 3;
const int trafficLightsDetected = 4;
const int turnRightDetected = 5;
const int turnLeftDetected = 6;
////////////////////////////////////
const string forwardAction = "1";
const string BackwardAction = "2";
const string rightAction = "3";
const string leftAction = "4";
const string stopAction = "5";

static int ScannerStatus = -1; 


//Sockets:
string CarServerSocket::msgRecu() {

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
void CarServerSocket::msgEnvoi(string msgEnvoie) {
	//le message sera 1 ou 2 ou 3 ..etc donc char[1]
	//char envoiebuff[1] = { msg };

	int n = (int) msgEnvoie.length();
	strcpy_s(this->sendBuffer, msgEnvoie.c_str());
	// on Envoie le message
	iSendResult = send(ClientSocket, this->sendBuffer, n, 0);
	this_thread::sleep_for(chrono::milliseconds(35));
	//closesocket(ClientSocket);
}

//Analyse video:
VideoCapture ObjectScanner::lancerCam()
{
	VideoCapture camTemp; 
	camTemp.open(this->videoSourceURL); 
	if (!camTemp.isOpened())
	{
		cout << "ERREUR CHARGEMENT FLUX VIDEO" << endl;
		exit(0);
	}
	return camTemp;
}
ObjectScanner::ObjectScanner()
{
}
void ObjectScanner::detect(const Mat & Image, std::vector<Rect>& objects)
{
	//detectMultiScale permet de retourner les objets trouvees sous une liste de rectangles.

	Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);

}
bool ObjectScanner::isVideoStreamOpened(VideoCapture VideoStream)
{
	if (!VideoStream.isOpened())
	{
		printf("Erreur Camera\n");
		return false;
	}
	else
		return true;
}
void ObjectScanner::resetDetection()
{
	this->PanneauStopVec.clear();
	this->FeuTrafficVec.clear();
	this->VirageDroiteVec.clear();
}
int ObjectScanner::sceneScan()
{
	this->VideoStream = lancerCam();
	namedWindow("Reconnaissance objets");


	if (isVideoStreamOpened(VideoStream)) {

		std::string fichierXmlCascadeStop = samples::findFile(this->ClassifierTrainingStop);
		std::string fichierXmlCascadeRouge = samples::findFile(this->ClassifierTrainingFeuRouge);
		std::string fichierXmlVirageDroite = samples::findFile(this->ClassifierTrainingVirageDroite);
		std::string fichierXmlVirageGauche = samples::findFile(this->ClassifierTrainingVirageGauche);

		Ptr<CascadeClassifier> cascadeStop = makePtr<CascadeClassifier>(fichierXmlCascadeStop);
		Ptr<CascadeClassifier> cascadeFeuxRouge = makePtr<CascadeClassifier>(fichierXmlCascadeRouge);
		Ptr<CascadeClassifier> cascadeVirageDroite = makePtr<CascadeClassifier>(fichierXmlVirageDroite);
		Ptr<CascadeClassifier> cascadeVirageGauche = makePtr<CascadeClassifier>(fichierXmlVirageGauche);

		Ptr<DetectionBasedTracker::IDetector> detectStop = makePtr<ObjectScanner>(cascadeStop);
		Ptr<DetectionBasedTracker::IDetector> detectFeuxRouge = makePtr<ObjectScanner>(cascadeFeuxRouge);
		Ptr<DetectionBasedTracker::IDetector> detectVirageDroite = makePtr<ObjectScanner>(cascadeVirageDroite);
		Ptr<DetectionBasedTracker::IDetector> detectVirageGauche = makePtr<ObjectScanner>(cascadeVirageGauche);


		cascadeStop = makePtr<CascadeClassifier>(fichierXmlCascadeStop);
		cascadeFeuxRouge = makePtr<CascadeClassifier>(fichierXmlCascadeRouge);
		cascadeVirageDroite = makePtr<CascadeClassifier>(fichierXmlVirageDroite);
		cascadeVirageGauche = makePtr<CascadeClassifier>(fichierXmlVirageGauche);

		Ptr<DetectionBasedTracker::IDetector> DetecteurStop = makePtr<ObjectScanner>(cascadeStop);
		Ptr<DetectionBasedTracker::IDetector> DetecteurFeuxRouge = makePtr<ObjectScanner>(cascadeFeuxRouge);
		Ptr<DetectionBasedTracker::IDetector> DetecteurVirageDroite = makePtr<ObjectScanner>(cascadeVirageDroite);
		Ptr<DetectionBasedTracker::IDetector> DetecteurVirageGauche = makePtr<ObjectScanner>(cascadeVirageGauche);
		
		DetectionBasedTracker::Parameters params;
		
		DetectionBasedTracker DetectorStop(detectStop, DetecteurStop, params);
		DetectionBasedTracker DetectorFeuxRouge(detectFeuxRouge, DetecteurFeuxRouge, params);
		DetectionBasedTracker DetectorVirageDroite(detectVirageDroite, DetecteurVirageDroite, params);
		DetectionBasedTracker DetectorVirageGauche(detectVirageGauche, DetecteurVirageGauche, params);

		Mat RFrame;
		Mat WFrame;

		while (waitKey(30) < 0)
		{
			VideoStream >> RFrame;
			cvtColor(RFrame, WFrame, COLOR_BGR2GRAY);
			////////////Feu Rouge:
			DetectorFeuxRouge.process(WFrame);
			DetectorFeuxRouge.getObjects(FeuTrafficVec);
			/////////////////Virags Droite:
			DetectorVirageDroite.process(WFrame);
			DetectorVirageDroite.getObjects(VirageDroiteVec);
			///////////////Virages Gauche:
			DetectorVirageGauche.process(WFrame);
			DetectorVirageGauche.getObjects(VirageGaucheVec);
			///////////////PanneauStop:
			DetectorStop.process(WFrame);
			DetectorStop.getObjects(PanneauStopVec);



			for (size_t i = 0; i < PanneauStopVec.size(); i++)
			{
				rectangle(RFrame, PanneauStopVec[i], Scalar(10, 100, 0));
			}
			for (size_t i = 0; i < FeuTrafficVec.size(); i++)
			{
				rectangle(RFrame, FeuTrafficVec[i], Scalar(50, 400, 10));
			}
			for (size_t i = 0; i < VirageDroiteVec.size(); i++)
			{
				rectangle(RFrame, VirageDroiteVec[i], Scalar(50, 400, 10));
			}
			for (size_t i = 0; i < VirageGaucheVec.size(); i++)
			{
				rectangle(RFrame, VirageGaucheVec[i], Scalar(50, 400, 10));
			}
			imshow("Reconnaissance objets", RFrame);

			if (this->PanneauStopVec.size() > 0)
			{

				resetDetection();

				ScannerStatus = stopDetected;
			}
			else if (this->FeuTrafficVec.size() > 0)
			{

				resetDetection();

				ScannerStatus = trafficLightsDetected;
			}
			else if (this->VirageDroiteVec.size() > 0)
			{

				resetDetection();

				ScannerStatus = turnRightDetected;
			}else if (this->VirageGaucheVec.size() > 0)
			{

				resetDetection();

				ScannerStatus = turnLeftDetected;
			}
			else if (this->PanneauStopVec.size() <= 0 && this->FeuTrafficVec.size() <= 0
				&& this->VirageDroiteVec.size()<=0 && this->VirageGaucheVec.size()<=0) 
			{
				resetDetection();
				ScannerStatus = NothingDetected;
			}
			if (waitKey(10) == 27)
			{
				break;
			}
		}


		resetDetection();
		DetectorStop.stop();
		DetectorFeuxRouge.stop();
		DetectorVirageDroite.stop();
		DetectorVirageGauche.stop();

	}
	return 0;
}


Vehicule::Vehicule(CarServerSocket& carSoc, ObjectScanner& objDetection)
{
	this->carSoc = &carSoc;
	this->objDetection = &objDetection;
}
void  Vehicule::forward()
{
	this->carSoc->msgEnvoi(forwardAction);
}
void Vehicule::backward()
{
	this->carSoc->msgEnvoi(BackwardAction);
}
void Vehicule::right()
{
	this->carSoc->msgEnvoi(rightAction);
}
void Vehicule::left()
{
	this->carSoc->msgEnvoi(leftAction);
}
void Vehicule::stop()
{
	this->backward(); //pour arreter les roues rapidement
	Sleep(30);
	this->carSoc->msgEnvoi(stopAction);
}
int Vehicule::returnIfObstacle() {
	string s = this->carSoc->msgRecu();
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
	Sleep(100);
	while (ScannerStatus == trafficLightsDetected) {Sleep(50);}
	while (returnIfObstacle() == 1) {
		cout << "Obstacle detectee, On reste sur place" << endl;
	}
	this->forward();
	Sleep(50);
}
void Vehicule::turnRightManeuver()
{
	
	this->stop();
	Sleep(10);
	this->right();
	Sleep(1000);
	this->stop();


}
void Vehicule::turnLeftManeuver()
{
	this->stop();
	Sleep(10);
	this->left();
	Sleep(1000);
	this->stop();
}
int Vehicule::goSmart()
{
	cout << "Preparing Video.." << endl;
	Sleep(15000); //On laisse le temps pour l'initialisation et syncro video.
	int status = -2;
	int isObstacle = 1;

	while (1)
	{	
		 isObstacle = returnIfObstacle();
	
		 if (isObstacle != 1)
		{
					
			 if (ScannerStatus == NothingDetected 
				 || ScannerStatus == stopDetected
				 || ScannerStatus == trafficLightsDetected 
				 || ScannerStatus == turnRightDetected
				 || ScannerStatus == turnLeftDetected
				 ) 
			 {
				 status = ScannerStatus;
			 }
			
				switch (status)
				{
				case NothingDetected:  cout << "\033[1;31m \n------------\nRAS\n---------- \033[0m\n" << endl; this->forward(); break;
				case stopDetected: cout << "\033[1;31m \n------------\STOP DETECTE\n---------- \033[0m\n" << endl;  this->stopSignManeuver(); break;
				case trafficLightsDetected:  cout << "\033[1;31m \n------------\FEU DETECTEE\n---------- \033[0m\n" << endl; this->redTrafficLightManeuver(); break;
				case turnRightDetected:  cout << "\033[1;31m \n------------\n VIRAGE A DROITE DETECTEE \n---------- \033[0m\n" << endl; turnRightManeuver(); break;
				case turnLeftDetected:  cout << "\033[1;31m \n------------\n VIRAGE A GAUCHE DETECTEE \n---------- \033[0m\n" << endl; turnLeftManeuver(); break;
				
				default: this->stop(); break;
				}
			
		 }else {
			 this->stop();
			 cout << "Obstacle Detectee" << endl;
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
	
	thread v(&Vehicule::goSmart, v1);
	thread t(&ObjectScanner::sceneScan, s1);
	
	
	v.join();
	t.join();
	
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

