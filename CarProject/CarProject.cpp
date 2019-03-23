
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

	const string CascadePanneauStop_XML="C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml";
	const string CascadeFeuVert_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string CascadeFeuRouge_XML="C:/Users/mhaba/OneDrive/Desktop/FeuxDeCirculation.xml";
	const string videoSourceURL = "http://192.168.0.26:8080/video/jpeg";
	CascadeClassifier CascadePanneauStop, CascadeFeuVert, CascadeFeuRouge;
	VideoCapture VideoSource;
	Mat frame, frame_gray;
	vector<Rect> PanneauStopVec, FeuVertVec, FeuRougeVec;



public:
	ObjectScanner();
	void sceneScan();
	bool XmlExist(string URL);
	bool cameraTest(VideoCapture videoSource);
	void detectAndDisplay(Mat frame);
	

};

ObjectScanner::ObjectScanner()
{
}

void ObjectScanner::sceneScan()
{

	if (CascadePanneauStop.load(this->CascadePanneauStop_XML) &&
		CascadeFeuVert.load(CascadeFeuVert_XML) &&
		CascadeFeuRouge.load(CascadeFeuRouge_XML))
	{



		this->VideoSource.open(0);

		Sleep(1000);

		if (this->VideoSource.isOpened()) {
			cout << "CAMERA OUVERTE" << endl;

			while (this->VideoSource.read(this->frame)) {
				cout << "FRAME READ SUCCES" << endl;

				if (!this->frame.empty()) {
					cout << "FRAME NON VIDE SUCCES" << endl;
					detectAndDisplay(frame);
					//on applique les filtres:
					/*cvtColor(this->frame, frame_gray, COLOR_BGR2GRAY);
					equalizeHist(frame_gray, frame_gray);
					cout << "FILTRES APPLIQUEES AVEC SUCCES" << endl;


					CascadePanneauStop.detectMultiScale(frame_gray, this->PanneauStopVec);
					cout << "DETECTMULTISCAKE AVEC SUCCES" << endl;

					
					for (size_t i = 0; i < this->PanneauStopVec.size(); i++) {

						Point center(this->PanneauStopVec[i].x + this->PanneauStopVec[i].width / 2, this->PanneauStopVec[i].y + this->PanneauStopVec[i].height / 2);
						ellipse(this->frame, center, Size(this->PanneauStopVec[i].width / 2, this->PanneauStopVec[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);

						Mat faceROI = frame_gray(this->PanneauStopVec[i]);

					}

					

					//CascadeFeuVert.detectMultiScale(frame_gray, this->FeuVertVec);
					//CascadeFeuRouge.detectMultiScale(frame_gray, this->FeuRougeVec);


					imshow("Capture - Face detection", this->frame);
*/



				}



			}




		}

	}


}

void ObjectScanner:: detectAndDisplay( Mat frame )
{
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    std::vector<Rect> faces;
	this->CascadePanneauStop.detectMultiScale( frame_gray, faces );

    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4 );

        Mat faceROI = frame_gray( faces[i] );
		/*
        //-- In each face, detect eyes
        std::vector<Rect> eyes;
		this->CascadePanneauStop.detectMultiScale( faceROI, eyes );

        for ( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4 );
        }
		*/
    }

    //-- Show what you got
    imshow( "Capture - Face detection", frame );
}
bool ObjectScanner:: XmlExist(string URL) {
	ifstream Fichier;

	Fichier.open(URL);
	if (Fichier.fail()) {
		cout << "Erreur chargement fichier XML" << endl;
		return false;
	}
	else {
		return true;
	}
}

bool ObjectScanner::cameraTest(VideoCapture videoSource)
{
	return false;
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
/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables */
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;


/** @function main */

int main(int argc, const char** argv)
{
	CommandLineParser parser(argc, argv,
		"{help h||}"
		"{face_cascade|C:/Users/mhaba/OneDrive/Desktop/stopsign_classifier.xml|Path to face cascade.}"
		"{eyes_cascade|C:/OpenCV4/opencv-master/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|Path to eyes cascade.}"
		"{camera|0|Camera device number.}");

	parser.about("\nThis program demonstrates using the cv::CascadeClassifier class to detect objects (Face + eyes) in a video stream.\n"
		"You can use Haar or LBP features.\n\n");
	parser.printMessage();

	String face_cascade_name = parser.get<String>("face_cascade");
	String eyes_cascade_name = parser.get<String>("eyes_cascade");

	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name))
	{
		cout << "--(!)Error loading face cascade\n";
		return -1;
	};
	if (!eyes_cascade.load(eyes_cascade_name))
	{
		cout << "--(!)Error loading eyes cascade\n";
		return -1;
	};

	int camera_device = parser.get<int>("camera");
	VideoCapture capture;
	//-- 2. Read the video stream
	capture.open(camera_device);
	if (!capture.isOpened())
	{
		cout << "--(!)Error opening video capture\n";
		return -1;
	}

	Mat frame;
	while (capture.read(frame))
	{
		if (frame.empty())
		{
			cout << "--(!) No captured frame -- Break!\n";
			break;
		}

		//-- 3. Apply the classifier to the frame
		detectAndDisplay(frame);

		if (waitKey(10) == 27)
		{
			break; // escape
		}
	}
	return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay(Mat frame)
{
	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	std::vector<Rect> faces;
	face_cascade.detectMultiScale(frame_gray, faces);

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);

		Mat faceROI = frame_gray(faces[i]);

		//-- In each face, detect eyes
		std::vector<Rect> eyes;
		eyes_cascade.detectMultiScale(faceROI, eyes);

		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
		}
	}

	//-- Show what you got
	imshow("Capture - Face detection", frame);
}

