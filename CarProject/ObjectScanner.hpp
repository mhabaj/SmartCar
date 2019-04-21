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
	std::string ClassifierTrainingStop = "C:/Users/mhaba/OneDrive/Desktop/StopSign.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT stop.
	std::string ClassifierTrainingFeuRouge = "C:/Users/mhaba/OneDrive/Desktop/TrafficLights.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT feu rouge.
	std::string ClassifierTrainingVirageDroite = "C:/Users/mhaba/OneDrive/Desktop/DirectionRight.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT stop.
	std::string ClassifierTrainingVirageGauche = "C:/Users/mhaba/OneDrive/Desktop/DirectionLeft.xml"; //URL VERS LES DONNEES DE LENTRAINEMENT feu rouge.	
	cv::VideoCapture VideoStream;
	std::string videoSourceURL = "http://192.168.43.109:8080/video";
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