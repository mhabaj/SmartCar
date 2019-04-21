#pragma once
#define WIN32_LEAN_AND_MEAN
#define DEFAULT_BUFLEN 56
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

class CarServerSocket {

private:
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	char* DEFAULT_PORT;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	char sendBuffer[DEFAULT_BUFLEN];
public:
	CarServerSocket(std::string DEFAULT_PORT);
	int  initSoc();
	std::string msgRecu();
	void msgEnvoi(std::string msgEnvoie);
	int returniResult();
	~CarServerSocket();
};

CarServerSocket::CarServerSocket(std::string DEFAULT_PORT) {
	char * cstr = new char[DEFAULT_PORT.length() + 1];
	strcpy_s(cstr, 6, DEFAULT_PORT.c_str());
	this->DEFAULT_PORT = cstr;
}
int CarServerSocket::initSoc() {


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("Error init Socket WSA : %d\n", iResult);
		return 1;
	}

	//les params de la socket:
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Initialisaation address local du serveur et le PORT.
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("error fonction getaddrinfo: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Creation de socket:
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf(" error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Initialisation du protocole TCP du Socket:
	iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("error bind(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("Error listen(): %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Lors qu'un client tent de se connecter:
	if (ClientSocket = accept(ListenSocket, NULL, NULL)) printf("\nclient initialisee \n");
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// On met la socket en Attente
	closesocket(ListenSocket);
	std::cout << "loading.." << std::endl; //(On met un peu de temps de chargement pour etre sur que les deux parties sont pret)
	Sleep(2000);
	return 0;
}
int CarServerSocket::returniResult() {

	return this->iResult;
}
CarServerSocket::~CarServerSocket() {
	closesocket(ClientSocket);
	WSACleanup();
	std::cout << "\n SocketDetruit \n" << std::endl;
}