//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
     
#define TRUE   1
#define FALSE  0
#define PORT 8888
     
int main(int argc, char *argv[])
{
    int opt = TRUE;  
    int master_socket, addr_len, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;  
    int max_sd;  
    struct sockaddr_in address;  
         
    char buffer[1025];
    char * temp;
         
    fd_set readfds; //set of socket descriptors

    char *message = "Trading Application \r\n"; //welcome message for client
    
    for (i = 0; i < max_clients; i++)
        client_socket[i] = 0;  

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) //create a master socket
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
     
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) //set master socket to allow multiple connections
    {
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }

    //type of sockets created
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons(PORT);  

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) //bind the socket to localhost port
    {
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }
    printf("Listener on port %d \n", PORT);  

    if (listen(master_socket, 3) < 0) //specify maximum pending connections for the master socket 
    {
        perror("listen");  
        exit(EXIT_FAILURE);  
    }

    addr_len = sizeof(address); //accept the incoming connection
    puts("Waiting for connections ...");
         
    while (TRUE)  
    {
        FD_ZERO(&readfds); //clear the socket set
     
        FD_SET(master_socket, &readfds); //add master socket to the set
        max_sd = master_socket;
             
        for (i = 0; i < max_clients; ++i) //add child sockets to the set 
        {
            sd = client_socket[i]; //store socket descriptor
            if (sd > 0) FD_SET(sd,&readfds); //if valid socket descriptor then add to read list
            if (sd > max_sd) max_sd = sd; //highest file descriptor number, need it for the select function
        }

        activity = select(max_sd+1, &readfds, NULL, NULL, NULL); //wait for an activity from any of the sockets
       
        if ((activity < 0) && (errno != EINTR))
            printf("select error");
             
        if (FD_ISSET(master_socket, &readfds)) //master socket gets indicated that there is an incoming connection
        {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0)
            {  
                perror("accept");
                exit(EXIT_FAILURE);
            }  
            printf("New connection, socket fd is %d, ip is : %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) //send welcome message
                perror("send");
                 
            puts("Welcome message sent successfully");  
                 
            for (i = 0; i < max_clients; i++) //add new socket to array of sockets
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                         
                    break;
                }
            }
        }
        for (i = 0; i < max_clients; i++) //for all other operations
        {  
            sd = client_socket[i];  
                 
            if (FD_ISSET(sd, &readfds)) //if there is some request
            {
                if ((valread = read(sd, buffer, sizeof(buffer))) == 0) //if the request is for closing
                {
                    //print the host details
                    getpeername(sd, (struct sockaddr*)&address , \
                        (socklen_t*)&addr_len);
                    printf("Host disconnected, ip %s ,port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(sd); //close the socket
                    client_socket[i] = 0; //mark 0 in the list
                }
                else //otherwise
                {
                    buffer[valread] = '\0';
                    if (strcmp(buffer,"login") == 0)
                    {
                        temp = "Trying to login";
                        send(sd , temp , strlen(temp) , 0 );
                    }
                    else if (strcmp(buffer,"buy") == 0)
                    {
                        temp = "Sending Buy request";
                        send(sd , temp , strlen(temp) , 0 );
                    }
                    else if (strcmp(buffer,"sell") == 0)
                    {
                        temp = "Sending Sell request";
                        send(sd , temp , strlen(temp) , 0 );
                    }
                    else if (strcmp(buffer,"order") == 0)
                    {
                        temp = "Asking Order Status";
                        send(sd , temp , strlen(temp) , 0 );
                    }
                    else if (strcmp(buffer,"trade") == 0)
                    {
                        temp = "Asking Trade Status";
                        send(sd , temp , strlen(temp) , 0 );
                    }
                    else if (strcmp(buffer,"logout") == 0)
                    {
                        temp = "Bye!";
                        send(sd , temp , strlen(temp) , 0 );

                        //print the host details
                        getpeername(sd, (struct sockaddr*)&address , \
                            (socklen_t*)&addr_len);
                        printf("Host disconnected, ip %s ,port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                        close(sd); //close the socket
                        client_socket[i] = 0; //mark 0 in the list
                    }
                }
            }
        }
    }
         
    return 0;
}  


/*TCP Server*/
/*#include<unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
   
// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;
   
        // and send that buffer to client
        write(connfd, buff, sizeof(buff));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}
   
// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}*/


/*Server for multiple clients*/
/*//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
     
#define TRUE   1 
#define FALSE  0 
#define PORT 8888 
     
int main(int argc , char *argv[])  
{  
    int opt = TRUE;  
    int master_socket , addr_len , new_socket , client_socket[30] , 
          max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
         
    char buffer[1025];  //data buffer of 1K 
         
    //set of socket descriptors 
    fd_set readfds;  
         
    //a message 
    char *message = "ECHO Daemon v1.0 \r\n";  
     
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
         
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
     
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
         
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
         
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
         
    //accept the incoming connection 
    addr_len = sizeof(address);  
    puts("Waiting for connections ...");  
         
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
             
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
     
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addr_len))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
             
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
           
            //send new connection greeting message 
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            {  
                perror("send");  
            }  
                 
            puts("Welcome message sent successfully");  
                 
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                         
                    break;  
                }  
            }  
        }  
             
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addr_len);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                    send(sd , buffer , strlen(buffer) , 0 );  
                }  
            }  
        }  
    }  
         
    return 0;  
}  
*/

/*Normal server*/
/*#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, val_read;
    struct sockaddr_in address;
    int opt = 1;
    int addr_len = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
       
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while (strcmp(buffer,"stop") != 0)
    {
        val_read = read( new_socket , buffer, 1024);
        printf("Client: %s\n",buffer );
        buffer = {0};
    }
    printf("bye!");
    return 0;
}*/