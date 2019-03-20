
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
const string Fenetre = "Class Reconnaissance objets.";

class DetectionParCascade : public DetectionBasedTracker::IDetector
{

private:
	DetectionParCascade();
	Ptr<CascadeClassifier> Detector;
	string ClassifierTraining; //URL VERS LES DONNEES DE LENTRAINEMENT.
	static int counter;
public:

	DetectionParCascade(Ptr<CascadeClassifier> detector) : IDetector(), Detector(detector) 
	{
		CV_Assert(detector);
	} 
	DetectionParCascade(string ClassifierTraining) {
		getClassifierTraining(ClassifierTraining);
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
	void getClassifierTraining(string ClassifierTraining){
		this->ClassifierTraining = ClassifierTraining;
	}



	void action() {

		counter = counter + 1;
		printf("COUNT\nCOUNT\nCOUNT\nCOUNT\nCOUNT\nCOUNT\nCOUNT\nCOUNT\nCOUNT : %d\n", counter);

	}




	bool isObjectDetected() {

		namedWindow(Fenetre);

		VideoCapture VideoStream(0);

		if (isVideoStreamOpened(VideoStream)) {
			//on include le .XML de l'entrainement
			std::string fichierXmlCascade = samples::findFile(this->ClassifierTraining); 

			//pointeur sur objet CascadeClassifier :
			Ptr<CascadeClassifier> cascade = makePtr<CascadeClassifier>(fichierXmlCascade); 
			Ptr<DetectionBasedTracker::IDetector> detect = makePtr<DetectionParCascade>(cascade);
			if (cascade->empty())
			{
				printf("Erreur fichier Casscade %s\n", fichierXmlCascade.c_str());
				return false;
			}

			cascade = makePtr<CascadeClassifier>(fichierXmlCascade);
			Ptr<DetectionBasedTracker::IDetector> DetecteurTrack = makePtr<DetectionParCascade>(cascade);
			if (cascade->empty())
			{
				printf("Error: Erreur donnees cascade %s\n", fichierXmlCascade.c_str());
				return false;
			}

			DetectionBasedTracker::Parameters params;
			DetectionBasedTracker Detector(detect, DetecteurTrack, params);

			if (!Detector.run())
			{
				printf("Erreur lancement detecteur\n");
				return false;
			}

			Mat RFrame; //referencee
			Mat WFrame;
			vector<Rect> ObjetDetectee;

			do
			{
				VideoStream >> RFrame;
				cvtColor(RFrame, WFrame, COLOR_BGR2GRAY);
				Detector.process(WFrame);
				Detector.getObjects(ObjetDetectee);
				//distance=vitesse/temps
				for (size_t i = 0; i < ObjetDetectee.size(); i++)
				{
					rectangle(RFrame, ObjetDetectee[i], Scalar(0, 100, 0));
					action();
				}

				imshow(Fenetre, RFrame);

			} while (waitKey(30) < 0);

			Detector.stop();
			return true;
		}
	}


	virtual ~DetectionParCascade() 
	{}
};



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

int DetectionParCascade::counter = 0;
int main()
{
	string StopTrainPATH = "C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml";

	DetectionParCascade StopDetect(StopTrainPATH);

	StopDetect.isObjectDetected();
	
	
}




