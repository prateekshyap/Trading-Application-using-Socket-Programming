using namespace std;

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include "datastructures.h"

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[])
{
	int noOfTraders = 5, maxClients = 5, noOfItems = 10;
	int PORT = atoi(argv[1]);
	int option = TRUE;
	int masterSocket, addressLength, newSocket, activity, i, j, k, clientValue, sd, maxSd, traderIndex = 0;
	int * allClients = new int[maxClients];
	int * clientTraderMap = new int[maxClients];
	Trader ** traderDetails = new Trader*[noOfTraders];
	struct sockaddr_in address;
	set<string> uIds;
	Item ** items = new Item*[noOfItems];

	for (i = 0; i < noOfItems; ++i)
		items[i] = new Item(i);
	
	char buffer[1024];
	//const char * temp;

	fd_set fds; //represents file descriptor sets for select function, is a bit array

	const char * welcomeMessage = "==============================\nWelcome to Trading Application\n==============================\nEnter an option (register/login/quit):\n";

	for (i = 0; i < maxClients; ++i)
		allClients[i] = 0;

	
	//socket() creates a socket in the specified domain and its specified type
	//AF_INET = Internet domain IPv4 (AF_INET6 for IPv6)
	//SOCK_STREAM = connection based protocol i.e. TCP (SOCK_DGRAM for UDP)
	//0 is the default protocol
	
	if ((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Socket Failed");
		exit(EXIT_FAILURE);
	}

	
	//setsockopt() sets the socket options
	//SO_REUSEADDR = Specifies that the rules used in validating addresses supplied to bind() should allow reuse of local addresses, if this is supported by the protocol. This option takes an int value. This is a Boolean option.
	if (setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0)
	{
		perror("Multiple Clients Support Failed");
		exit(EXIT_FAILURE);
	}

	//details of the types of sockets created
	address.sin_family = AF_INET; //Internet domain
	address.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY is an IP address that is used when we don't want to bind a socket to any specific IP
	address.sin_port = htons(PORT); //htons() function translates a short integer from host byte order to network byte order

	if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address)) < 0) //binding the socket to localhost port
	{
		perror("Could not Bind");
		exit(EXIT_FAILURE);
	}

	cout << "=====================================" << endl;
	cout << "Welcome to TRADING APPLICATION SERVER" << endl;
	cout << "=====================================" << endl;
	cout << "Port: " << PORT << endl;

	if (listen(masterSocket, maxClients) < 0) //specify maximum number of clients those can be served simultaneously
	{
		perror("Could not Listen");
		exit(EXIT_FAILURE);
	}

	addressLength = sizeof(address);
	cout << "Waiting for Clients ..." << endl << endl;

	while (TRUE)
	{
		FD_ZERO(&fds); //initializes the file descriptor set fd_set to have zero bits for all file descriptors
		FD_SET(masterSocket, &fds); //add master socket to fd_set
		maxSd = masterSocket; //highest file descriptor number
		for (i = 0; i < maxClients; ++i) //add child sockets to the set
		{
			sd = allClients[i]; //store socket descriptor
			if (sd > 0) FD_SET(sd, &fds); //if it is a valid socket descriptor then add to read set
			if (sd > maxSd) maxSd = sd; //highest file descriptor number, need it for the select function
		}

		activity = select(maxSd+1, &fds, NULL, NULL, NULL); //waiting for an activity from any of the socket
		if (activity < 0 && errno != EINTR) printf("Select Error");
		if (FD_ISSET(masterSocket, &fds)) //master socket gets indicated that there is an incoming connection
		{
			if ((newSocket = accept(masterSocket, (struct sockaddr *)&address, (socklen_t *)&addressLength)) < 0) //accepting a new connection
			{
				perror("Accept");
				exit(EXIT_FAILURE);
			}
			cout << "WELCOME " << newSocket << ": " << inet_ntoa(address.sin_addr) << ", " << ntohs(address.sin_port) << endl;
			
			//sending welcome message to client
			if (send(newSocket, welcomeMessage, strlen(welcomeMessage), 0) != strlen(welcomeMessage)) //send welcome message to client
				perror("send");
			cout << "Host " << inet_ntoa(address.sin_addr) << ", " << ntohs(address.sin_port) << " is on" << endl;

			for (i = 0; i < maxClients; ++i)
			{
				if (allClients[i] == 0)
				{
					allClients[i] = newSocket;
					cout << "Adding to list of sockets as " << i << endl;
					break;
				}
			}
		}
		for (i = 0; i < maxClients; ++i)
		{
			sd = allClients[i];
			if (FD_ISSET(sd,&fds)) //if there is some request from any trader
			{
				if ((clientValue = read(sd, buffer, sizeof(buffer))) == 0) //if the request is for closing
				{
					//print the host details
					getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);
					cout << "Client on " << inet_ntoa(address.sin_addr) << " disconnected, port: " << ntohs(address.sin_port) << endl;
					close(sd); //close the socket
					allClients[i] = 0;
				}
				else //otherwise reading from client
				{
					char temp[1024] = {'\0'};
					buffer[clientValue] = '\0';
                    if (strcmp(buffer,"register") == 0) //for registration
                    {
                    	char uName[100] = {'\0'}, uId[50] = {'\0'}, password[50] = {'\0'};
                    	clientValue = read(sd, uName, sizeof(uName));
                    	clientValue = read(sd, uId, sizeof(uId));
                    	clientValue = read(sd, password, sizeof(password));
                    	//printf("'%s','%s','%s'\n",uName,uId,password);
                    	if (uIds.find(uId) == uIds.end()) //if new trader
                    	{
                    		uIds.insert(uId); //add to set
                    		traderDetails[traderIndex++] = new Trader(uName, uId, password, -1); //create a new trader
                    		sprintf(temp,"%s","Registration successful!");
                    	}
                    	else
                    	{
                    		sprintf(temp,"%s","Trader details already exists! Please Login!\nEnter an option (register/login/quit):\n");
                    	}
                        send(sd, temp, strlen(temp), 0);
                    }
                    else if (strcmp(buffer,"login") == 0)
                    {
                    	char uId[50] = {'\0'}, password[50] = {'\0'};
                    	clientValue = read(sd, uId, sizeof(uId));
                    	clientValue = read(sd, password, sizeof(password));
                    	if (uIds.find(uId) == uIds.end())
                    	{
                    		sprintf(temp,"%s","Trader does not exist! Please Register!");
                    		send(sd, temp, strlen(temp), 0);
                    	}
                    	else
                    	{
                    		char storedName[100] = {'\0'}, storedId[50] = {'\0'}, storedPw[50] = {'\0'};
                    		bool isFound = false;
                    		for (j = 0; j < traderIndex; ++j)
                    		{
                    			strcpy(storedId,traderDetails[j]->getUserId().c_str());
                    			strcpy(storedPw,traderDetails[j]->getPassword().c_str());
                    			//cout << storedId << " " << storedPw << endl;
                    			if (strcmp(uId,storedId) == 0 && strcmp(password,storedPw) == 0)
                    			{
                    				if (traderDetails[j]->getLoginStatus())
                    				{
                    					isFound = true;
                    					sprintf(temp,"%s","You can not login on multiple devices!");
                    					send(sd, temp, strlen(temp), 0);
                    					break;
                    				}
                    				strcpy(storedName,traderDetails[j]->getName().c_str());
                    				traderDetails[j]->setNumber(sd);
                    				clientTraderMap[sd] = j; //store the trader's index to the map
                    				traderDetails[j]->setLoginStatus(true);
                    				getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);
			                        char tempMsg[1024] = "";
			                        sprintf(tempMsg,"%s logged in at %s, port: %d\n",storedName,inet_ntoa(address.sin_addr),ntohs(address.sin_port));
			                        send(sd, tempMsg, strlen(tempMsg), 0);
			                        isFound = true;
			                        break;
                    			}
                    		}
                    		if (!isFound)
                    		{
                    			sprintf(temp,"%s","Wrong Credentials! Try again!");
                    			send(sd, temp, strlen(temp), 0);
                    		}
                    	}
                    }
                    else if (strcmp(buffer,"buy") == 0)
                    {
                    	char itemNumber[5] = {'\0'}, itemPrice[8] = {'\0'}, itemQuantity[8] = {'\0'};
                    	getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);		
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus())
                    	{
                    		send(sd, itemList, strlen(itemList), 0);
                    		clientValue = read(sd, itemNumber, sizeof(itemNumber));
                    		clientValue = read(sd, itemPrice, sizeof(itemPrice));
                    		clientValue = read(sd, itemQuantity, sizeof(itemQuantity));
                    		cout << traderDetails[clientTraderMap[sd]]->getName() << " wants to buy Item: " << item_names[atoi(itemNumber)-1] << " at price " << itemPrice << " and net quantity " << itemQuantity << endl;
                    		Order * newOrder = new Order(traderDetails[clientTraderMap[sd]],atoi(itemNumber)-1,atoi(itemQuantity),atoi(itemPrice),0);
                    		items[atoi(itemNumber)-1]->insertItem(newOrder,0);
                    		/*sprintf(temp,"%s","Buy request received successfully");
                        	send(sd, temp, strlen(temp), 0);*/
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"sell") == 0)
                    {
                    	char itemNumber[5] = {'\0'}, itemPrice[8] = {'\0'}, itemQuantity[8] = {'\0'};
                        getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);		
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus())
                    	{
                    		send(sd, itemList, strlen(itemList), 0);
                    		clientValue = read(sd, itemNumber, sizeof(itemNumber));
                    		clientValue = read(sd, itemPrice, sizeof(itemPrice));
                    		clientValue = read(sd, itemQuantity, sizeof(itemQuantity));
                    		cout << traderDetails[clientTraderMap[sd]]->getName() << " wants to sell Item: " << item_names[atoi(itemNumber)-1] << " at price " << itemPrice << " and net quantity " << itemQuantity << endl;
                    		Order * newOrder = new Order(traderDetails[clientTraderMap[sd]],atoi(itemNumber)-1,atoi(itemQuantity),atoi(itemPrice),1);
                    		items[atoi(itemNumber)-1]->insertItem(newOrder,1);
                    		/*sprintf(temp,"%s","Sell request received successfully");
                        	send(sd, temp, strlen(temp), 0);*/
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"order") == 0)
                    {
                        getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus())
                    	{
                    		sprintf(temp,"%d",noOfItems);
                    		send(sd, temp, strlen(temp), 0);
                    		char orderStatus[10240] = {'\0'};
                    		for (j = 0; j < noOfItems; ++j)
                    		{
                    			char itemDetails[1024] = {'\0'};
                    			Order ** buyOrders = items[j]->getBuyBook();
                    			int buyOrdersSize = items[j]->getBuyBookSize();
								Order ** sellOrders = items[j]->getSellBook();
								int sellOrdersSize = items[j]->getSellBookSize();
                    			if (buyOrdersSize > 0 && sellOrdersSize > 0)
                    				sprintf(itemDetails,"---------------------------------\nItem: %s\n---Buy Order---\nQuantity: %d\nPrice: %d\n---Sell Order---\nQuantity: %d\nPrice: %d\n",item_names[j],buyOrders[0]->get_quantity(),buyOrders[0]->get_price(),sellOrders[0]->get_quantity(),sellOrders[0]->getPrice());
                    			else if (buyOrdersSize > 0)
                    				sprintf(itemDetails,"---------------------------------\nItem: %s\n---Buy Order---\nQuantity: %d\nPrice: %d\n---Sell Order---\nQuantity: NIL\nPrice: NIL\n",item_names[j],buyOrders[0]->get_quantity(),buyOrders[0]->get_price());
                    			else if (sellOrdersSize > 0)
                    				sprintf(itemDetails,"---------------------------------\nItem: %s\n---Buy Order---\nQuantity: NIL\nPrice: NIL\n---Sell Order---\nQuantity: %d\nPrice: %d\n",item_names[j],sellOrders[0]->get_quantity(),sellOrders[0]->getPrice());
                    			else
                    				sprintf(itemDetails,"---------------------------------\nItem: %s\n---Buy Order---\nQuantity: NIL\nPrice: NIL\n---Sell Order---\nQuantity: NIL\nPrice: NIL\n",item_names[j]);
                    			strcat(orderStatus,itemDetails);
                    		}
                    		send(sd, orderStatus, strlen(orderStatus), 0);
                    		/*sprintf(temp,"%s","Order Status sent successfully");
                    		send(sd, temp, strlen(temp), 0);*/
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"trade") == 0)
                    {
                        getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);		
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus())
                    	{
                    		sprintf(temp,"%s","Sending trade status");
                    		send(sd, temp, strlen(temp), 0);
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"logout") == 0)
                    {
                    	getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);		
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus())
                    	{
                    		traderDetails[clientTraderMap[sd]]->setLoginStatus(false);
	        				sprintf(temp,"%s logged out from %s, port: %d\n",traderDetails[clientTraderMap[sd]]->getName().c_str(),inet_ntoa(address.sin_addr),ntohs(address.sin_port));
	                        send(sd, temp, strlen(temp), 0);
	                    }
	                    else
	                    {
	                    	sprintf(temp,"%s","You are not logged in!");
	                    	send(sd, temp, strlen(temp), 0);
	                    }
                    }
                    else if (strcmp(buffer,"close") == 0)
                    {
                        sprintf(temp,"%s","Thanks for using the Application!");
                        send(sd, temp, strlen(temp), 0);

                        //print the host details
						getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);
						cout << "Client on " << inet_ntoa(address.sin_addr) << " disconnected, port: " << ntohs(address.sin_port) << endl;
						close(sd); //close the socket
						allClients[i] = 0;

						/*//dummy print part to be removed
						for (j = 0; j < noOfItems; ++j)
							items[j]->printItemQueues();*/
                    }
                    else
                    {
                       	sprintf(temp,"%s","What are you saying?");
                        send(sd, temp, strlen(temp), 0);
                    }
				}
			}
		}
	}
	return 0;
}