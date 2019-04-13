/*

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>


const char hostname[] = "192.168.43.244";
const int portnum = 27016;

using namespace std;

class CarClientSocket {
private:

	int sd;  // n° fd de la socket
	int ret;
	struct hostent *host;
	struct sockaddr_in addrsrv;
	char msg[128];
	bool isConnected;

public:

	void initSoc() {


		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("Erreur socket");
			this->isConnected = false;
			exit(0);
		}
		else
			printf("socket = %d connectee sur le port %d\n", sd), portnum;


		if ((host = gethostbyname(hostname)) == NULL) {
			herror("gethostbyname");
			this->isConnected = false;
			exit(-1);
		}

		bcopy(host->h_addr, (char *)&addrsrv.sin_addr, host->h_length);
		addrsrv.sin_family = AF_INET;
		addrsrv.sin_port = htons(portnum);

		connect(sd, (const struct sockaddr*)&addrsrv, sizeof(addrsrv));
		this->isConnected = true;

	}

	void msgEnvoie(string s) {



		int n = s.length();
		strcpy(this->msg, s.c_str());
		if ((ret = send(sd, msg, s.length(), 0)) == -1) {
			perror("Erreur sendto");
		}
		else {
			printf("sendto = %d; msg = %s\n", ret, msg);
			this_thread::sleep_for(chrono::milliseconds(30));

		}
	}

	string msgRecv() {
		string msgRecu = "";
		if ((ret = recv(sd, msg, sizeof(msg), 0)) == -1) {
			perror("Erreur recvfrom");
		}
		if (this->ret > 0) {

			for (int i = 0; i < this->ret; i++) {
				msgRecu += this->msg[i];
			}
			return msgRecu;

		}
		else if (ret == 0) printf("femerutre de la connexion.....\n");

		else {
			printf("Erreur recv (COnnexion perdu avec le serveur?");
			shutdown(sd, SHUT_RDWR);
			close(sd);
		}
		exit(1);

	}
	bool retIsConnected() {
		return this->isConnected;
	}
};
/*
//main test client
int main(void) {
	CarClientSocket c1;
	//c1.initSoc();
	int i = 1;
	c1.initSoc();
	while (c1.retIsConnected()) {


		//c1.msgEnvoie("test????");
		cout << i << "message recu : " << c1.msgRecv() << endl;
		i++;
	}
	return 0;
}
*/