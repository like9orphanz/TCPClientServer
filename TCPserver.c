/* 
 * File:   TCPserver.c
 * Author: wrigh
 *
 * Created on January 13, 2016, 6:08 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
/*
 * 
 */


/*
 *	This program is to simulate a server that can communicate with a client and 
 *	respond accordingly. Depending on what message the client sends the server will 
 *	send either <reply></reply>, the load average, or <error></error>
 *
 */
int ListenSockCreation(int port, struct sockaddr_in *address);

void handler(int param)
{
	fprintf(stderr, "\nCTRL-C has been entered. The server is shut down.\n");
	exit(EXIT_SUCCESS);
}

void * processInfo(void *);

int main(int argc, char** argv)
{
	if(argc != 2)
        {
                fprintf(stderr, "Please input <port>\n");
                exit(1);
        }

	//Listen socket descriptor (reference)
	int ls;
	//Socket descriptor (reference)
	int socket_fd;
	//Number of bytes to send or reciever
	int len = 0;
	//Size of waiting clients
	int waitSize = 16;
	//Server address, and client address
	struct sockaddr_in serverAddress, clientAddress;
	struct in_addr a;
	//Length of client address
	int clientAddressLength;
	
	int errorCheck = 0;
	char hostname[1024];
	hostname[1023] = '\0';

	//bind the socket
	ls = ListenSockCreation(atoi(argv[1]), &serverAddress);

	//function for if the user hits Ctrl-C
	signal(SIGINT,handler);
	//find the hostname
	gethostname(hostname, 1023);
	struct hostent * hostptr;
	
	//find the ip address
	hostptr = gethostbyname(hostname);
	
	//listen to connection
	if(listen(ls, waitSize) < 0)
	{
		fprintf(stderr, "Error listening\n");
		close(ls);
		exit(1);
	}

	//prints the host name, ip address, and port
	struct sockaddr_in printSock;
	socklen_t addrLen = sizeof(struct sockaddr);
	getsockname(ls, (struct sockaddr *)&printSock, &addrLen);
	fprintf(stderr, "Host Name: %s\n", hostname);
	fprintf(stderr, "IP address: %s\n", inet_ntoa(*(struct in_addr*)hostptr->h_addr));
	fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));

	//init thread 
	clientAddressLength = sizeof(clientAddress);
	pthread_t tid[waitSize];
	pthread_attr_t attr;
	int threadCount = 0;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	//infinite loop, accept connection and process info
	while(1)
	{
		socket_fd = accept(ls, (struct sockaddr *) &clientAddress, &clientAddressLength);
		pthread_create(&tid[threadCount], &attr, processInfo, (void *)(intptr_t)socket_fd);
		threadCount = (threadCount + 1) % waitSize;
	}

	return(EXIT_SUCCESS);
}

/*
 *	processInfo : reads the message that the client sends and depending on what the message says 
 *		responds accordingly
 *
 */
void * processInfo(void *socketS)
{
	int Sock = (intptr_t)socketS;
	int errorCheck, count, count2;
	long tid;
	tid = (long)socket;
	ssize_t bytes_read;
	char buffer[256];
	char message[256];
	char reply[256];
	bzero(buffer, 256);
	bzero(message, 256);
	bzero(reply, 256);
	char myNull = '\0';
	size_t length = 255;

	//read message sent
	bytes_read = read(Sock, buffer, length);

	fprintf(stderr, "message is: %s\n", buffer);
	
	//if message is <echo>
	if(buffer[0] == '<' && buffer[1] == 'e' && buffer[2] == 'c' && buffer[3] == 'h' && buffer[4] == 'o' && buffer[5] == '>' &&buffer[6] != ' ')
	{
		count = 6;
		while(buffer[count] != '\0')
		{
			message[count - 6] = buffer[count];
			fprintf(stderr, "buffer = %c ",buffer[count]);
			count++;
		}

		fprintf(stderr, "count = %d\n", count);
		message[count - 6] = myNull;

		//and if message ends with </echo>
		if(message[count - 7] != ' ' && buffer[count - 7] == '<' && buffer[count - 6] == '/' && buffer[count - 5] == 'e' && buffer[count - 4] == 'c' && buffer[count - 3] == 'h' && buffer[count - 2] == 'o' && buffer[count - 1] == '>' && buffer[count] == '\0')
		{
			char *tempChar;
			tempChar = malloc(sizeof(char) * 7);
			sprintf(tempChar, "<reply>");
			memcpy(&reply, tempChar, (sizeof(char) * 7));
			count2 = count - 13;
			
			//sending back same message with <reply></reply>
			for(count = 0; count < count2; count++)
				reply[count + 7] = message[count];
			tempChar = malloc(sizeof(char) * 9);
			sprintf(tempChar, "</reply>%c", myNull);
			memcpy(&reply[count + 7], tempChar, (sizeof(char) * 9));
			errorCheck = write(Sock, reply, sizeof(reply));

			
		}
	}
		//if message is <loadavg/>
		else if(strcmp(buffer, "<loadavg/>") == 0)
		{
		double loadavg[3];
		char *tempChar;
		errorCheck = getloadavg(loadavg, 3);

		if(errorCheck > 0)
		{
			tempChar = malloc(sizeof(char) * 9);
			sprintf(tempChar, "<replyLoadAvg>");
			memcpy(&reply, tempChar, (sizeof(char) * 14));
			tempChar = malloc(sizeof(char) * 8);
			sprintf(tempChar, "%lf", loadavg[0]);
			memcpy(&reply[14], tempChar, (sizeof(char) * 10));
			reply[18] = ':';
			sprintf(tempChar, "%lf", loadavg[1]);
			memcpy(&reply[19], tempChar, (sizeof(char) * 10));
			reply[23] = ':';
			sprintf(tempChar, "%lf", loadavg[2]);
			memcpy(&reply[24], tempChar, (sizeof(char) * 10));
			tempChar = malloc(sizeof(char) * 11);
			sprintf(tempChar, "</replyLoadAvg>", myNull);
			memcpy(&reply[28], tempChar, (sizeof(char) * 15));
			write(Sock, reply, sizeof(reply));
		}
	}	
	
	//if message is unknown send response
	sprintf(reply, "<error>unknown format</error>%c", myNull);
	write(Sock, reply, sizeof(reply));

	pthread_exit(0);
		
}
//function to creation listen socket and bind it
int ListenSockCreation(int port, struct sockaddr_in *address)
{
	int sock_ls;

        memset(address, 0, sizeof(*address));
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = htonl(INADDR_ANY);
        address->sin_port = htons(port);

	//creates a socket
	if((sock_ls = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
                fprintf(stderr, "Error: listen sock failed!");
                exit(1);
        }

	//binds the socket
        if(bind(sock_ls, (struct sockaddr *)address, sizeof(*address)) < 0)
        {
                fprintf(stderr, "Error binding\n");
                close(sock_ls);
                exit(1);
        }

	return sock_ls;

}					
