using namespace std;

#include<iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	const char * serverIP = argv[1];
	int PORT = atoi(argv[2]);
	int clientSocket = 0, serverValue;
	struct sockaddr_in serverAddress;
	char buffer[1024] = "open";
	char * temp, * welcomeMessage, * clientChoice;
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) //create a new socket
	{
		cout << "Socket Creation Failed" << endl;
		return -1;
	}

	//details of the types of sockets created
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);

	if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0) //convert text to binary form and store in server address
	{
		cout << "Invalid Address" << endl;
		return -1;
	}

	if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) //connect to server
	{
		cout << "Connection Failed" << endl;
		return -1;
	}

	welcomeMessage = (char *)malloc(1024*sizeof(char));
	serverValue = read(clientSocket, temp, 1024); //getting welcome message from server
	cout << "reading welcome message" << endl;
	while (strcmp(buffer, "close") != 0) //till client is not closed
	{
		cout << welcomeMessage << endl; //print welcome message from server
		while (strcmp(clientChoice, "logout") != 0) //till the client doesn't logout
		{
			cin >> clientChoice; //register/login/quit
			send(clientSocket, clientChoice, strlen(clientChoice), 0); //send the choice to server
			temp = (char *)malloc(1024*sizeof(char));
			serverValue = read(clientSocket, temp, 1024);
			cout << temp << endl;
		}
	}
	cout << "Client Closed" << endl;
	return 0;
}