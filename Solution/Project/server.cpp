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
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
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


	//read existing registration details from file and insert to map and create the traders as well
	ifstream inputFile;
	inputFile.open("traders.txt");
	int noOfT = 5;
	while(!inputFile.eof())
	{
		char uName[50] = {'\0'}, uId[50] = {'\0'}, uPass[50] = {'\0'};
		inputFile >> uName >> uId >> uPass;
		uIds.insert(uId); //add to set
   		traderDetails[traderIndex++] = new Trader(uName, uId, uPass, -1); //create a new trader
		
		--noOfT;
		if (noOfT == 0) break;
	}
	inputFile.close();


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
		for (i = 0; i < maxClients; ++i) //for each client
		{
			sd = allClients[i]; //get the client id
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
                    	if (traderIndex == noOfTraders) //if 5 traders have already registered, no more registrations can be done
                    	{
                    		cout << traderIndex << "," << noOfTraders << endl;
                    		sprintf(temp,"%s","No more registrations possible");
                    		send(sd, temp, strlen(temp), 0);
                    		continue;
                    	}
                    	char uName[100] = {'\0'}, uId[50] = {'\0'}, password[50] = {'\0'};
                    	//read name, id and password from client
                    	clientValue = read(sd, uName, sizeof(uName));
                    	clientValue = read(sd, uId, sizeof(uId));
                    	clientValue = read(sd, password, sizeof(password));
                    	if (uIds.find(uId) == uIds.end()) //if new trader
                    	{
                    		//write to traders file
                    		ofstream outputFile;
                    		outputFile.open("traders.txt",std::ios_base::app);
                    		outputFile << uName << " " << uId << " " << password << endl;
                    		outputFile.close();

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
                    else if (strcmp(buffer,"login") == 0) //for login
                    {
                    	char uId[50] = {'\0'}, password[50] = {'\0'};
                    	//read the id password from client
                    	clientValue = read(sd, uId, sizeof(uId));
                    	clientValue = read(sd, password, sizeof(password));
                    	if (uIds.find(uId) == uIds.end()) //if non-existing user
                    	{
                    		sprintf(temp,"%s","Trader does not exist! Please Register!");
                    		send(sd, temp, strlen(temp), 0);
                    	}
                    	else //if existing user
                    	{
                    		bool isFound = false;
                    		for (j = 0; j < traderIndex; ++j)
                    		{
                    			char storedName[100] = {'\0'}, storedId[50] = {'\0'}, storedPw[50] = {'\0'};
                    			strcpy(storedId,traderDetails[j]->getUserId().c_str());
                    			strcpy(storedPw,traderDetails[j]->getPassword().c_str());
                    			if (strcmp(uId,storedId) == 0 && strcmp(password,storedPw) == 0) //match the id password, if matched correctly
                    			{
                    				if (traderDetails[j]->getLoginStatus()) //if already logged in 
                    				{
                    					isFound = true;
                    					sprintf(temp,"%s","You can not login on multiple devices!"); //don't allow mulitple logins
                    					send(sd, temp, strlen(temp), 0);
                    					break;
                    				}
                    				strcpy(storedName,traderDetails[j]->getName().c_str());
                    				traderDetails[j]->setNumber(sd); //set the unique client id to the trader in the server
                    				clientTraderMap[sd] = j; //store the trader's index to the map
                    				traderDetails[j]->setLoginStatus(true); //set login status as true
                    				getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);
			                        char tempMsg[1024] = "";
			                        sprintf(tempMsg,"%s logged in at %s, port: %d\n",storedName,inet_ntoa(address.sin_addr),ntohs(address.sin_port)); //display login message at server
			                        send(sd, tempMsg, strlen(tempMsg), 0);
			                        isFound = true;
			                        break;
                    			}
                    		}
                    		if (!isFound) //if the user is not found
                    		{
                    			sprintf(temp,"%s","Wrong Credentials! Try again!"); //print wrong credentials
                    			send(sd, temp, strlen(temp), 0);
                    		}
                    	}
                    }
                    else if (strcmp(buffer,"buy") == 0) //for buy order
                    {
                    	char itemNumber[5] = {'\0'}, itemPrice[8] = {'\0'}, itemQuantity[8] = {'\0'};
                    	getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength); //get the trader details
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus()) //if logged in
                    	{
                    		send(sd, itemList, strlen(itemList), 0);
                    		//get order details from client
                    		clientValue = read(sd, itemNumber, sizeof(itemNumber));
                    		clientValue = read(sd, itemPrice, sizeof(itemPrice));
                    		clientValue = read(sd, itemQuantity, sizeof(itemQuantity));
                    		//print at server
                    		cout << traderDetails[clientTraderMap[sd]]->getName() << " wants to buy Item: " << item_names[atoi(itemNumber)-1] << " at price " << itemPrice << " and net quantity " << itemQuantity << endl;
                    		//create a new order and add to queue
                    		Order * newOrder = new Order(traderDetails[clientTraderMap[sd]],atoi(itemNumber)-1,atoi(itemQuantity),atoi(itemPrice),0);
                    		items[atoi(itemNumber)-1]->insertItem(newOrder,0);
                    		traderDetails[clientTraderMap[sd]]->add_order(newOrder);
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"sell") == 0) //for sell order
                    {
                    	char itemNumber[5] = {'\0'}, itemPrice[8] = {'\0'}, itemQuantity[8] = {'\0'};
                        getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength); //get the trader details		
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus()) //if logged in
                    	{
                    		send(sd, itemList, strlen(itemList), 0);
                    		//get order details from client
                    		clientValue = read(sd, itemNumber, sizeof(itemNumber));
                    		clientValue = read(sd, itemPrice, sizeof(itemPrice));
                    		clientValue = read(sd, itemQuantity, sizeof(itemQuantity));
                    		//print at server
                    		cout << traderDetails[clientTraderMap[sd]]->getName() << " wants to sell Item: " << item_names[atoi(itemNumber)-1] << " at price " << itemPrice << " and net quantity " << itemQuantity << endl;
                    		//create a new order and add to queue
                    		Order * newOrder = new Order(traderDetails[clientTraderMap[sd]],atoi(itemNumber)-1,atoi(itemQuantity),atoi(itemPrice),1);
                    		items[atoi(itemNumber)-1]->insertItem(newOrder,1);
                    		traderDetails[clientTraderMap[sd]]->add_order(newOrder);
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"order") == 0) //for viewing best priced orders
                    {
                        getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength); //get the trader details
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus()) //if logged in
                    	{
                    		sprintf(temp,"%d",noOfItems);
                    		send(sd, temp, strlen(temp), 0); //send the total number of items
                    		sleep(1);
                    		for (j = 0; j < noOfItems; ++j) //for each item
                    		{
                    			char orderStatus[10240] = {'\0'};
                    			//get the buy book and sell book
                    			vector<Order *> buyOrders = items[j]->getBuyBook();
                    			vector<Order *> sellOrders = items[j]->getSellBook();
                    			//store the item name
                    			sprintf(orderStatus,"%s\n",items[j]->getItemName());
                    			for (k = 0; k < buyOrders.size() || k < sellOrders.size(); ++k)
                    			{
                    				char singleBuyOrder[1024] = {'\0'}, singleSellOrder[1024] = {'\0'};
                    				if(k < buyOrders.size()){
                    					sprintf(singleBuyOrder,"%d(%d)\t\t",buyOrders[k]->get_price(),buyOrders[k]->get_remaining_quantity()); //append the top buy order
                    				}
									else{
										sprintf(singleBuyOrder,"%s","\t\t");
									}
									if(k < sellOrders.size()){
										sprintf(singleSellOrder,"%d(%d)\n",sellOrders[k]->get_price(),sellOrders[k]->get_remaining_quantity()); //append the top sell order
                    				}
                    				else
                    					sprintf(singleSellOrder,"%s","\n");
                    				//append buy and sell order to a main string
                    				strcat(orderStatus,singleBuyOrder);
                    				strcat(orderStatus,singleSellOrder);
                    			}
                    			//send the main string to client
                    			send(sd, orderStatus, strlen(orderStatus), 0);
                    			sleep(1);
                    		}
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"trade") == 0) //for viewing the matched trades
                    {
                        getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength); //get the trader details
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus()) //if logged in
                    	{
                    		vector<Order *> all_trades = traderDetails[clientTraderMap[sd]]->getAllTrades(); //get the trades
							char orderStatus[10240] = {'\0'};
							sprintf(temp,"%ld",all_trades.size());
                    		send(sd, temp, strlen(temp), 0); //send no of trades to client
                    		sleep(1);
                    		send(sd, traderDetails[clientTraderMap[sd]]->getName().c_str(), strlen(traderDetails[clientTraderMap[sd]]->getName().c_str()), 0); //send trader name to client
                    		sleep(1);
							for(int i=0; i<all_trades.size(); i++) //for each trade print everything along with status
							{
								char tradeDetails[1024] = {'\0'};
								sprintf(tradeDetails,"%s\t%d\t\t%d\t%s\t%s\t",item_names[all_trades[i]->get_item_index()],all_trades[i]->get_remaining_quantity(),all_trades[i]->get_price(),all_trades[i]->get_type() == 0 ? "BUY" : "SELL",all_trades[i]->get_status() == 'Q' ? "NOT MATCHED" : "MATCHED\t");
								if (all_trades[i]->get_status() == 'F')
									strcat(tradeDetails,all_trades[i]->get_match()->getName().c_str());
								send(sd, tradeDetails, strlen(tradeDetails), 0);
								sleep(1);
							}
                    	}
                    	else
                    	{
							sprintf(temp,"%s","Please Login!");
                        	send(sd, temp, strlen(temp), 0);
                    	}
                    }
                    else if (strcmp(buffer,"logout") == 0) //for logout
                    {
                    	getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength); //get the trader details		
                    	if (traderDetails[clientTraderMap[sd]]->getLoginStatus()) //if logged in
                    	{
                    		traderDetails[clientTraderMap[sd]]->setLoginStatus(false); //set login status to false
	        				sprintf(temp,"%s logged out from %s, port: %d\n",traderDetails[clientTraderMap[sd]]->getName().c_str(),inet_ntoa(address.sin_addr),ntohs(address.sin_port)); //display on server
	                        send(sd, temp, strlen(temp), 0); //send message to client
	                    }
	                    else
	                    {
	                    	sprintf(temp,"%s","You are not logged in!");
	                    	send(sd, temp, strlen(temp), 0);
	                    }
                    }
                    else if (strcmp(buffer,"close") == 0) //for closing
                    {
                        sprintf(temp,"%s","Thanks for using the Application!");
                        send(sd, temp, strlen(temp), 0);

                        //print the host details
						getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addressLength);
						cout << "Client on " << inet_ntoa(address.sin_addr) << " disconnected, port: " << ntohs(address.sin_port) << endl;
						close(sd); //close the socket
						allClients[i] = 0;
                    }
				}
			}
		}
	}
	return 0;
}