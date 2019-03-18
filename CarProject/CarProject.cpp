
#include <opencv2/imgproc.hpp>  // Blur gauss (cf livre)
#include <opencv2/core.hpp>        //  Mat, Scalar
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV
#include <opencv2/features2d.hpp>
#include <opencv2/objdetect.hpp>
#include <stdio.h>
#include <iostream>
#include <stdbool.h>

using namespace std;
using namespace cv;

const string Fenetre = "Class Reconnaissance de panneaux.";




class DetectionParCascade : public DetectionBasedTracker::IDetector
{

private:
	DetectionParCascade();
	cv::Ptr<cv::CascadeClassifier> Detector;
	string StopClassifierTraining; //URL VERS LES DONNEES DE LENTRAINEMENT.
public:

	void getStopClassifierTraining(string StopClassifierTraining){
		this->StopClassifierTraining = StopClassifierTraining;
	}

	DetectionParCascade(string StopClassifierTraining) {
		this->StopClassifierTraining = StopClassifierTraining;
	}
	DetectionParCascade(cv::Ptr<cv::CascadeClassifier> detector) : IDetector(), Detector(detector)
	{
		CV_Assert(detector);
	}

	void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) 
	{
		Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);
	
	}

	boolean isVideoStreamOpened(VideoCapture VideoStream) {
		
		if (!VideoStream.isOpened())
		{
			printf("Erreur Camera\n");
			return false;
		}
		else
			return true;
	}

	void isObjectDetected() {

		namedWindow(Fenetre);

		VideoCapture VideoStream(0);

		if (isVideoStreamOpened(VideoStream)) {
			//on include le .XML de l'entrainement
			std::string fichierXmlCascadeStop = samples::findFile(this->StopClassifierTraining); 


			cv::Ptr<cv::CascadeClassifier> cascade = makePtr<cv::CascadeClassifier>(fichierXmlCascadeStop); 
			cv::Ptr<DetectionBasedTracker::IDetector> detect = makePtr<DetectionParCascade>(cascade);
			if (cascade->empty())
			{
				printf("Erreur fichier Casscade %s\n", fichierXmlCascadeStop.c_str());
				return 2;
			}

			cascade = makePtr<cv::CascadeClassifier>(fichierXmlCascadeStop);
			cv::Ptr<DetectionBasedTracker::IDetector> DetecteurTrack = makePtr<DetectionParCascade>(cascade);
			if (cascade->empty())
			{
				printf("Error: Erreur donnees cascade %s\n", fichierXmlCascadeStop.c_str());
				return 2;
			}

			DetectionBasedTracker::Parameters params;
			DetectionBasedTracker Detector(detect, DetecteurTrack, params);

			if (!Detector.run())
			{
				printf("Erreur lancement detecteur\n");
				return 3;
			}

			Mat RFrame; //referencee
			Mat WFrame;
			vector<Rect> PanneauxStop;

			do
			{
				VideoStream >> RFrame;
				cvtColor(RFrame, WFrame, COLOR_BGR2GRAY);
				Detector.process(WFrame);
				Detector.getObjects(PanneauxStop);

				for (size_t i = 0; i < PanneauxStop.size(); i++)
				{
					rectangle(RFrame, PanneauxStop[i], Scalar(0, 100, 0));

				}

				imshow(Fenetre, RFrame);

			} while (waitKey(30) < 0);

			Detector.stop();
			return 4;
		}
	}


	virtual ~DetectionParCascade() 
	{}
};



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





class CarDecision {
private:
	Vehicule v1;
	DetectionParCascade SignalStop,SignalFeux;
public :

	CarDecision(Vehicule v1, DetectionParCascade SignalStop, DetectionParCascade SignalFeux);

};

CarDecision::CarDecision(Vehicule v1, DetectionParCascade SignalStop, DetectionParCascade SignalFeux) {
	this->v1 = v1;
	this->SignalStop = SignalStop;
	this->SignalFeux = SignalFeux;
}



int main()
{
	string StopTrainPATH = "C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml";

	DetectionParCascade::startStopRec(StopTrainPATH);

	if (DetectionParCascade::startStopRec(StopTrainPATH) == 4) {

		cout << "PANNEAU STOP DETECTEE" << endl;
		



	}
	
}




