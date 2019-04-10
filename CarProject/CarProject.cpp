
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock.h>
#pragma comment(lib, "WS2_32.lib")


using namespace std;
using namespace cv;


class ObjectScanner 
{

private:

	const string CascadePanneauStop_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string CascadeFeuVert_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string CascadeFeuRouge_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	//const string videoSourceURL = "http://192.168.0.26:8080/video/jpeg";
	//const string videoSourceURL = "http://192.168.43.103:8081/";
	CascadeClassifier CascadePanneauStop, CascadeFeuVert, CascadeFeuRouge;
	cv::VideoCapture VideoSource;
	vector<Rect> PanneauStopVec, FeuVertVec, FeuRougeVec;
	bool isStop;
	bool isGreen ;
	bool isRed ;

public:
	ObjectScanner();
	int sceneScan();  // 3 si Stop, 4 si Vert, 5 si Rouge
	void returnFoundObjects(Mat frame, Mat Gray_Transform);
	bool returnIsStop();

};

ObjectScanner::ObjectScanner()
{
}

int ObjectScanner::sceneScan()
{

	// ----Integrer les cascades:
	if (!this->CascadePanneauStop.load(this->CascadePanneauStop_XML))
	{
		cout << "ERREUR CHARGEMENT CASCADE SIGNAL STOP" << endl;
		return -1;
	};
	

	//----Lancer le flux video

	//this->VideoSource.open(this->videoSourceURL); //(pour la video par internet)
	this->VideoSource.open(0);   //pour webcam locale

	if (!this->VideoSource.isOpened())
	{
		cout << "ERREUR CHARGEMENT FLUX VIDEO" << endl;
		return -1;
	}

	Mat frame;
	Mat frame_resized;
	Mat Gray_Transform;
	while (this->VideoSource.read(frame)&& this->PanneauStopVec.size() <= 0
		&& this->FeuRougeVec.size() <= 0 && this->FeuVertVec.size() <=0)
	{
		if (frame.empty())
		{
			cout << "ERREUR TRAME VIDE" << endl;
			break;
		}

		//---- On modifie la taille de l'image (pour les performances) et on applique les Classifiers 
		resize(frame, frame_resized,Size(320, 220));

		cout << this->PanneauStopVec.size()<<","<< this->FeuVertVec.size() << 
			"," << this->FeuRougeVec.size() << endl;
		
		

		returnFoundObjects(frame_resized, Gray_Transform);
		
		// Ctrl C pour quitter
		if (waitKey(10) == 27)
		{
			break; 
		}
	}
	if(this->PanneauStopVec.size() > 0)
	{
		return 3;
	}
	if (this->FeuVertVec.size() > 0)
	{
		return 4;
	}
	if (this->FeuRougeVec.size() > 0)
	{
		return 5;
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

class Vehicule {

private:

	float speed;
	int Motor_1, Motor_2;
	int servo;


public:
	//10 forward, 11 backward, 12 right, 13 left, 0 stop
	Vehicule();
	int forward();
	int backward();
	int right();
	int left();
	int stop();
	void getSpeed(float speed);
	
};

Vehicule::Vehicule()
{
}

int  Vehicule::forward()
{
	return 10;
}

int Vehicule::backward()
{
	return 11;
}

int Vehicule::right()
{
	return 12;
}

int Vehicule::left()
{
	return 13;
}

int Vehicule::stop()
{
	return 0;
}

void Vehicule::getSpeed(float speed)
{
	this->speed = speed;
}





/*
class CarDecision {
private:
	Vehicule v1;
	DetectionParCascade SignalStop,SignalFeux;
public :

	CarDecision(Vehicule v1, DetectionParCascade SignalStop, DetectionParCascade SignalFeux);

};
*/
/*
CarDecision::CarDecision(Vehicule v1, DetectionParCascade SignalStop, DetectionParCascade SignalFeux) {
	this->v1 = v1;
	this->SignalStop = SignalStop;
	this->SignalFeux = SignalFeux;
}
*/
/* //main pour tester detection objts.
int main()
{



	ObjectScanner obj1;

	int foundObj = obj1.sceneScan();
	if (foundObj ==3) {

		cout << "STOP DETECTEE" << endl;
	}
	if (foundObj == 4) {

		cout << "FEUX VERT" << endl;
	}
	if (foundObj == 5) {

		cout << "FEUX ROUGE" << endl;
	}



return 0;
}*/

