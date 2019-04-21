#pragma once
#include "CarServerSocket.hpp"
#include "ObjectScanner.hpp"



class Vehicule {

private:
	CarServerSocket* carSoc;
	ObjectScanner* objDetection;

public:
	Vehicule(CarServerSocket& carSoc, ObjectScanner& objDetection);
	void forward();
	void backward();
	void right();
	void left();
	void stop();
	int returnIfObstacle();
	void stopSignManeuver();
	void redTrafficLightManeuver();
	void turnRightManeuver();
	void turnLeftManeuver();
	int goSmart();
};