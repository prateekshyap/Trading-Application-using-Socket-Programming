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
	char buffer[1024] = {0};
	char * temp;
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


	temp = (char *)malloc(1024*sizeof(char));
	serverValue = read(clientSocket, temp, 1024);
	cout << temp << endl;

	bool isClosed = false;
	while (strcmp(buffer, "close") != 0)
	{
		cout << "New Traders are welcome to register/login here" << endl;
		while (strcmp(buffer, "logout") != 0)
		{
			cin >> buffer;
			if (strcmp(buffer,"close") == 0)
			{
				isClosed = true;
				break;
			}
			//cout << buffer;
			send(clientSocket, buffer, strlen(buffer), 0);
			char uName[100] = {'\0'}, uId[50] = {'\0'}, password[50] = {'\0'};
			if (strcmp(buffer,"register") == 0) //if want to register
			{
				cout << "Name: ";
				scanf("%s",uName);
				send(clientSocket, uName, strlen(uName), 0);
				cout << "User ID: ";
				scanf("%s",uId);
				send(clientSocket, uId, strlen(uId), 0);
				cout << "Password: ";
				scanf("%s",password);
				send(clientSocket, password, strlen(password), 0);
			}
			else if (strcmp(buffer,"login") == 0) //if want to login
			{
				cout << "User ID: ";
				scanf("%s",uId);
				send(clientSocket, uId, strlen(uId), 0);
				cout << "Password: ";
				scanf("%s",password);
				send(clientSocket, password, strlen(password), 0);
			}
			temp = (char *)malloc(1024*sizeof(char));
			serverValue = read(clientSocket, temp, 1024);
			cout << temp << endl;
		}
		if (isClosed) break;
		cout << "Waiting for Traders... (Type close to close the client)" << endl;
		cin >> buffer;
	}
	send(clientSocket, buffer, strlen(buffer), 0);
	temp = (char *)malloc(1024*sizeof(char));
	serverValue = read(clientSocket, temp, 1024);
	cout << temp << endl;
	return 0;
}