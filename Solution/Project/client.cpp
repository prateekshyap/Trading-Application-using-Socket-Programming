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
	char * tempText;
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


	tempText = (char *)malloc(1024*sizeof(char));
	serverValue = read(clientSocket, tempText, 1024);
	cout << tempText << endl;

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
			char uName[100] = {'\0'}, uId[50] = {'\0'}, password[50] = {'\0'}, itemNumber[5] = {'\0'}, itemPrice[8] = {'\0'}, temp[1024] = {'\0'};
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
			else if (strcmp(buffer,"buy") == 0) //if want to buy
			{
				read(clientSocket, temp, 1024);
				cout << temp << endl;
				if (strcmp(temp,"Please Login!") != 0)
				{
					cout << "Enter item number: " << endl;
					scanf("%s",itemNumber);
					send(clientSocket, itemNumber, strlen(itemNumber), 0);
					cout << "Enter price: " << endl;
					scanf("%s",itemPrice);
					send(clientSocket, itemPrice, strlen(itemPrice), 0);
				}
				//send(clientSocket, "0", strlen("0"), 0);
			}
			else if (strcmp(buffer,"sell") == 0) //if want to sell
			{
				read(clientSocket, temp, 1024);
				cout << temp << endl;
				if (strcmp(temp,"Please Login!") != 0)
				{
					cout << "Enter item number: " << endl;
					scanf("%s",itemNumber);
					send(clientSocket, itemNumber, strlen(itemNumber), 0);
					cout << "Enter price: " << endl;
					scanf("%s",itemPrice);
					send(clientSocket, itemPrice, strlen(itemPrice), 0);
				}
				//send(clientSocket, "1", strlen("1"), 0);
			}
			else if (strcmp(buffer,"order status") == 0) //if want to order
			{
				read(clientSocket, temp, 1024);
				cout << temp << endl;
			}
			else if (strcmp(buffer,"trade status") == 0) //if want to trade
			{
				read(clientSocket, temp, 1024);
				cout << temp << endl;
			}
			if (strcmp(temp,"Please Login!") != 0)
			{
				tempText = (char *)malloc(1024*sizeof(char));
				serverValue = read(clientSocket, tempText, 1024);
				cout << tempText << endl;
			}
		}
		if (isClosed) break;
		cout << "Waiting for Traders... (Type close to close the client)" << endl;
		cin >> buffer;
	}
	send(clientSocket, buffer, strlen(buffer), 0);
	tempText = (char *)malloc(1024*sizeof(char));
	serverValue = read(clientSocket, tempText, 1024);
	cout << tempText << endl;
	return 0;
}