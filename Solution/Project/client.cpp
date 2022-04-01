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
	while (strcmp(buffer, "logout") != 0)
	{
		cin >> buffer;
		send(clientSocket, buffer, strlen(buffer), 0);
		temp = (char *)malloc(1024*sizeof(char));
		serverValue = read(clientSocket, temp, 1024);
		cout << temp << endl;
	}
	return 0;
}