#pragma once

#include "opencv2/objdetect.hpp"
#include <opencv2/videoio.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <string>


class ObjectScanner : public cv::DetectionBasedTracker::IDetector
{
private:

	std::vector<cv::Rect> PanneauStopVec, FeuTrafficVec, VirageDroiteVec, VirageGaucheVec;
	std::string ClassifierTrainingStop = "../assets/StopSign.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT stop.
	std::string ClassifierTrainingFeuRouge = "../assets/TrafficLights.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT feu rouge.
	std::string ClassifierTrainingVirageDroite = "../assets/DirectionRight.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT stop.
	std::string ClassifierTrainingVirageGauche = "../assets/DirectionLeft.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT feu rouge.	
	cv::VideoCapture VideoStream;
	//std::string videoSourceURL = "http://192.168.43.109:8080/video";
	int videoSourceURL = 0;
	cv::Ptr<cv::CascadeClassifier> cascadeStop;
	cv::Ptr<cv::CascadeClassifier> cascadeFeuxRouge;
	cv::Ptr<cv::CascadeClassifier> cascadeVirageDroite;
	cv::Ptr<cv::CascadeClassifier> cascadeVirageGauche;
	cv::Ptr<cv::CascadeClassifier> Detector;


public:
	cv::VideoCapture lancerCam();
	ObjectScanner();
	ObjectScanner(cv::Ptr<cv::CascadeClassifier> detector);
	void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects);
	bool isVideoStreamOpened(cv::VideoCapture VideoStream);
	void resetDetection();
	int sceneScan();
	virtual ~ObjectScanner();


};


ObjectScanner::ObjectScanner(cv::Ptr<cv::CascadeClassifier> detector) : IDetector(), Detector(detector)
{
	CV_Assert(detector);

}

ObjectScanner::~ObjectScanner()
{
}