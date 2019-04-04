#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 255
//#define DEFAULT_PORT "27015"

using namespace std;



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
	//string msg_recu(recvbuf);
	int recvbuflen = DEFAULT_BUFLEN;


public:

	CarServerSocket(string DEFAULT_PORT) {
		char * cstr = new char[DEFAULT_PORT.length() + 1];
		strcpy_s(cstr, 6 ,DEFAULT_PORT.c_str());
		this->DEFAULT_PORT = cstr;
	}

	int  initSoc() {

		
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("Erreur initialisation Socket WSA : %d\n", iResult);
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
			printf("erreur dans la fonction getaddrinfo: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Creation de socket:
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Initialisation du protocole TCP du Socket:
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("erreur bind(): %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("Erreur listen(): %d\n", WSAGetLastError());
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
		return 0;
	}

	string msgRecu() {


			this->iResult = recv(this->ClientSocket, this->recvbuf, this->recvbuflen, 0);

			if (this->iResult > 0)
			{
				string msg;

				for (int i = 0; i < this->iResult; i++)
				{
					msg += this->recvbuf[i];
				}
				//Sleep(3000); // Note uppercase S

					//cout << "msg recu: " << msg << endl;

			//	closesocket(ClientSocket);

				return msg;

			}
			else if (iResult == 0)
				printf("Connection closing...\n");

			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();

			}

		

	}

	void msgEnvoi(int msg) {

		char envoiebuff[1] = { msg };
		// On recoit jusqu'a la fin du msg envoyee par le client
		do
		{
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, envoiebuff, iResult, 0);
			//closesocket(ClientSocket);
		} while (this->iResult > 0);
	}

	int returniResult() {
		return this->iResult;
	}

	~CarServerSocket() {
		
		closesocket(ClientSocket);
		WSACleanup();
		cout << "SocketDetruit" << endl;
	}
};



int main(void)
{
	string portRecv = "27015";
	string portSend = "27016";
	CarServerSocket SocRecv(portRecv);
	CarServerSocket SocSend(portSend);

	while (SocRecv.initSoc() == 0) 
	{

		string s;
		while (1) 
		{

			s= SocRecv.msgRecu();
			cout << "String recu:  " << s << endl;
		}

	}

	



	SocRecv.~CarServerSocket();

	return 0;
}