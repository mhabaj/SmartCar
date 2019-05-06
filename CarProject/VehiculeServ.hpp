#pragma once
#include "CarServerSocket.hpp"
#include "ObjectScanner.hpp"



class VehiculeServ {

private:
	CarServerSocket* carSoc;

public:
	VehiculeServ(CarServerSocket& carSoc);
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
	static void start();
};