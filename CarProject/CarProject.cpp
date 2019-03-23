
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <windows.h>
using namespace std;
using namespace cv;
class ObjectScanner 
{

private:

	const string CascadePanneauStop_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string CascadeFeuVert_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string CascadeFeuRouge_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string videoSourceURL = "http://192.168.0.26:8080/video/jpeg";
	CascadeClassifier CascadePanneauStop, CascadeFeuVert, CascadeFeuRouge;
	VideoCapture VideoSource;
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
		cvtColor(frame, Gray_Transformed, COLOR_RGB2GRAY);
		
	// DETECT MULTISCALE POUR LES STOPS 
	this->CascadePanneauStop.detectMultiScale(Gray_Transformed, this->PanneauStopVec);

	for (size_t i = 0; i < this->PanneauStopVec.size(); i++)
	{
		//this->isStop = false;
		rectangle(frame, this->PanneauStopVec[i], Scalar(0, 100, 0));
		
	}

	// DETECT MULTISCALE POUR LES FEUX VERTS 
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

	imshow("", frame);
		
}

bool ObjectScanner::returnIsStop()
{
	return this->isStop;
}



void main()
{
	ObjectScanner obj1;
	

	if (obj1.sceneScan()==3) {

		cout << "ATTNETION STOP" << endl;
	}
	if (obj1.sceneScan() == 4) {

		cout << "FEUX VERT" << endl;
	}
	if (obj1.sceneScan() == 3) {

		cout << "FEUX ROUGE" << endl;
	}
	
}






/*
class Vehicule {

private:

	int speed;
	int Motor_1, Motor_2;
	int servo;


public:
	Vehicule(int Motor_1,int Motor2);
	void veh_forward();
	void veh_backward();
	void veh_right();
	void veh_left();
	void veh_stop();




};



*/
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

