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


const char hostname[] = "192.168.43.244";
const int portnum = 27015;

using namespace std;

class CarClientSocket {
private:

	int sd;  // n° fd de la socket
	int ret;
	struct hostent *host;
	struct sockaddr_in addrsrv;
	char msg[128];

public:

	void initSoc() {


		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			perror("Erreur socket");
		else
			printf("socket = %d\n", sd);

		if ((host = gethostbyname(hostname)) == NULL) {
			herror("gethostbyname");
			exit(-1);
		}

		bcopy(host->h_addr, (char *)&addrsrv.sin_addr, host->h_length);
		addrsrv.sin_family = AF_INET;
		addrsrv.sin_port = htons(portnum);

		connect(sd, (const struct sockaddr*)&addrsrv, sizeof(addrsrv));

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

	int msgRecv() {
		if ((ret = recv(sd, msg, sizeof(msg), 0)) == -1) {
			perror("Erreur recvfrom");
		}
		else {
			msg[ret] = '\0'; printf("recvfrom = %d; msg = %s\n", ret, msg);
		}

	}
};


/* //main test client
int main(void) {
	CarClientSocket c1;
	c1.initSoc();
	int i = 1;
	while (i < 5) {


		c1.msgEnvoie("test????");
		i++;
	}
	return 0;
}
*/