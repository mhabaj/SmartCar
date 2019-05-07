
#define DEFAULT_BUFLEN 3


#include "VehiculeServ.hpp"
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


static int ScannerStatus = -1;
////////////////////////////
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
	// ! est un delimiteur
	msgEnvoie += "!";
	int n = (int) msgEnvoie.length();
	strcpy_s(this->sendBuffer, msgEnvoie.c_str());
	
	// on Envoie le message
	iSendResult = send(ClientSocket, this->sendBuffer, n, 0);
	//this_thread::sleep_for(chrono::milliseconds(10));
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

		string fichierXmlCascadeStop = samples::findFile(this->ClassifierTrainingStop);
		string fichierXmlCascadeRouge = samples::findFile(this->ClassifierTrainingFeuRouge);
		string fichierXmlVirageDroite = samples::findFile(this->ClassifierTrainingVirageDroite);
		//string fichierXmlVirageGauche = samples::findFile(this->ClassifierTrainingVirageGauche);



		 cascadeStop = makePtr<CascadeClassifier>(fichierXmlCascadeStop);
		 cascadeFeuxRouge = makePtr<CascadeClassifier>(fichierXmlCascadeRouge);
		 cascadeVirageDroite = makePtr<CascadeClassifier>(fichierXmlVirageDroite);
		 //cascadeVirageGauche = makePtr<CascadeClassifier>(fichierXmlVirageGauche);

		 Ptr<DetectionBasedTracker::IDetector> detectStopPrincipale = makePtr<ObjectScanner>(cascadeStop);
		 Ptr<DetectionBasedTracker::IDetector> detectFeuxRougePrincipale = makePtr<ObjectScanner>(cascadeFeuxRouge);
		 Ptr<DetectionBasedTracker::IDetector> detectVirageDroitePrincipale = makePtr<ObjectScanner>(cascadeVirageDroite);
		 //Ptr<DetectionBasedTracker::IDetector> detectVirageGauchePrincipale = makePtr<ObjectScanner>(cascadeVirageGauche);


		cascadeStop = makePtr<CascadeClassifier>(fichierXmlCascadeStop);
		cascadeFeuxRouge = makePtr<CascadeClassifier>(fichierXmlCascadeRouge);
		cascadeVirageDroite = makePtr<CascadeClassifier>(fichierXmlVirageDroite);
		//cascadeVirageGauche = makePtr<CascadeClassifier>(fichierXmlVirageGauche);

		Ptr<DetectionBasedTracker::IDetector> DetecteurStopTrack = makePtr<ObjectScanner>(cascadeStop);
		Ptr<DetectionBasedTracker::IDetector> DetecteurFeuxRougeTrack = makePtr<ObjectScanner>(cascadeFeuxRouge);
		Ptr<DetectionBasedTracker::IDetector> DetecteurVirageDroiteTrack = makePtr<ObjectScanner>(cascadeVirageDroite);
		//Ptr<DetectionBasedTracker::IDetector> DetecteurVirageGaucheTrack = makePtr<ObjectScanner>(cascadeVirageGauche);
		
		DetectionBasedTracker::Parameters params;
		
		DetectionBasedTracker DetectorStop(detectStopPrincipale, DetecteurStopTrack, params);
		DetectionBasedTracker DetectorFeuxRouge(detectFeuxRougePrincipale, DetecteurFeuxRougeTrack, params);
		DetectionBasedTracker DetectorVirageDroite(detectVirageDroitePrincipale, DetecteurVirageDroiteTrack, params);
		//DetectionBasedTracker DetectorVirageGauche(detectVirageGauchePrincipale, DetecteurVirageGaucheTrack, params);

		Mat FrameInitiale;
		Mat FrameModifiee;

		while (waitKey(30) < 0)
		{
			VideoStream >> FrameInitiale;
			cvtColor(FrameInitiale, FrameModifiee, COLOR_BGR2GRAY);
			////////////Feu Rouge:
			DetectorFeuxRouge.process(FrameModifiee);
			DetectorFeuxRouge.getObjects(FeuTrafficVec);
			/////////////////Virags Droite:
			DetectorVirageDroite.process(FrameModifiee);
			DetectorVirageDroite.getObjects(VirageDroiteVec);
			///////////////Virages Gauche:
			//DetectorVirageGauche.process(FrameModifiee);
			//DetectorVirageGauche.getObjects(VirageGaucheVec);
			///////////////PanneauStop:
			DetectorStop.process(FrameModifiee);
			DetectorStop.getObjects(PanneauStopVec);



			for (size_t i = 0; i < PanneauStopVec.size(); i++)
			{
				rectangle(FrameInitiale, PanneauStopVec[i], Scalar(10, 100, 0));
			}
			for (size_t i = 0; i < FeuTrafficVec.size(); i++)
			{
				rectangle(FrameInitiale, FeuTrafficVec[i], Scalar(50, 400, 10));
			}
			for (size_t i = 0; i < VirageDroiteVec.size(); i++)
			{
				rectangle(FrameInitiale, VirageDroiteVec[i], Scalar(50, 400, 10));
			}
			for (size_t i = 0; i < VirageGaucheVec.size(); i++)
			{
				rectangle(FrameInitiale, VirageGaucheVec[i], Scalar(50, 400, 10));
			}
			imshow("Reconnaissance objets", FrameInitiale);

			if (this->PanneauStopVec.size() > 0)
			{

				resetDetection();
				//cout << "\033[1;31m \n------------\STOP DETECTE\n---------- \033[0m\n" << endl;
				ScannerStatus = stopDetected;
			}
			else if (this->FeuTrafficVec.size() > 0)
			{

				resetDetection();
				//cout << "\033[1;31m \n------------\TRAFFIC LIGHT DETECTE\n---------- \033[0m\n" << endl;
				ScannerStatus = trafficLightsDetected;
			}
			else if (this->VirageDroiteVec.size() > 0)
			{

				resetDetection();
				//cout << "\033[1;31m \n------------\VIRAGE DROIT DETECTE\n---------- \033[0m\n" << endl;
				ScannerStatus = turnRightDetected;
			}else if (this->VirageGaucheVec.size() > 0)
			{

				resetDetection();
				//cout << "\033[1;31m \n------------\VIRAGE GAUCHE DETECTE\n---------- \033[0m\n" << endl;
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
		//DetectorVirageGauche.stop();

	}
	return 0;
}


VehiculeServ::VehiculeServ(CarServerSocket& carSoc)
{
	this->carSoc = &carSoc;
}
void  VehiculeServ::forward()
{
	this->carSoc->msgEnvoi(forwardAction);
}
void VehiculeServ::backward()
{
	this->carSoc->msgEnvoi(BackwardAction);
}
void VehiculeServ::right()
{
	this->carSoc->msgEnvoi(rightAction);
}
void VehiculeServ::left()
{
	this->carSoc->msgEnvoi(leftAction);
}
void VehiculeServ::stop()
{
	this->carSoc->msgEnvoi(stopAction);
}
int VehiculeServ::returnIfObstacle() {
	string s = this->carSoc->msgRecu();
	string infoRecv = s.substr(s.length()-1, 1);
	cout << "INFO RECIEVED: " << infoRecv << endl;
	 if (infoRecv == "1") {
		printf("Obstacle Detectee \n");
		return 1;
	}
	else return 0;
}
void VehiculeServ::stopSignManeuver()
{
	Sleep(300);
	this->stop();
	Sleep(2000);
	while (returnIfObstacle() == 1) {
		cout << "Obstacle detectee, On reste sur place" << endl;
	}
	while (ScannerStatus == stopDetected) {
		this->forward(); //on avance jusqu'à depasser le panneau .
	}
	Sleep(2000);
}
void VehiculeServ::redTrafficLightManeuver()
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
void VehiculeServ::turnRightManeuver()
{	
	this->stop();
	this->right();
	Sleep(1000);
}
void VehiculeServ::turnLeftManeuver()
{
	this->stop();
	this->left();
	Sleep(1000);
}
int VehiculeServ::goSmart()
{
	cout << "Video Sync.." << endl;
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
				case stopDetected: cout << "\033[1;31m \n-----------\nSTOP DETECTE\n---------- \033[0m\n" << endl;  this->stopSignManeuver(); break;
				case trafficLightsDetected:  cout << "\033[1;31m \n------------\nFEU DETECTEE\n---------- \033[0m\n" << endl; this->redTrafficLightManeuver(); break;
				case turnRightDetected:  cout << "\033[1;31m \n------------\n VIRAGE A DROITE DETECTEE \n---------- \033[0m\n" << endl; turnRightManeuver(); break;
				case turnLeftDetected:  cout << "\033[1;31m \n------------\n VIRAGE A GAUCHE DETECTEE \n---------- \033[0m\n" << endl; turnLeftManeuver(); break;
				
				default: this->stop(); break;
				}
			
		 }else {
			 this->stop();
			 cout << "\033[1;31m \n------------\nOBSTACLE DETECTEE\n---------- \033[0m\n" << endl;
		 }
		
	}
}
void VehiculeServ::start() {

	///////////////////Server Socket/////////////////	
	string portSend = "27016";
	//CarServerSocket SocVoiture(portSend);
	//SocVoiture.initSoc();
	////////////////////Object detect module////////////
	ObjectScanner s1;
	thread t(&ObjectScanner::sceneScan, s1);
	//////////////////Decision module////////////////
	//VehiculeServ v1(SocVoiture);
	//thread v(&VehiculeServ::goSmart, v1);

	//v.join();
	t.join();



}


int main() {

	VehiculeServ::start();

	
	return 0;
}



/*
//main pour tester sockets
 // TEST SERVEUR:
int main(void)
{
	///////////:RECEPTION
	//string portRecv = "27015";
	//CarServerSocket SocRecv(portRecv);
	/*string portSend = "27016";
	CarServerSocket SocSend(portSend);
	string s = "3";
	while (SocSend.initSoc() == 0)
	{
		while (1)
		{
			 SocSend.msgEnvoi(s);
			cout << "String recu:  " << s << endl;
		}
	}
	SocSend.~CarServerSocket();
	*/
/*
/////////////////ENVOIE://///////////////////
	string portSend = "27016";
	char mymsg[] = "bla!";
	CarServerSocket SocSend(portSend);
	int i = 0;
	while (SocSend.initSoc() == 0) {
		while (i < 6000000) {
		SocSend.msgEnvoi(mymsg);
		i++;
		}
	}
	return 0;
}






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

