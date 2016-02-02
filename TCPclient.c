/*
 *	File Name: TCPclient.c
 *	Author: Joshua Wright
 *	Project: 1
 *
 */


#include "TCPclient.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 *	This program is a TCPclient that communicates to TCPserver. This program uses TCPmain 
 *		to send a certain ammount of messages and tests to see if those messages
 *		fit the criteria that the server replies with
 *
 */

//Creates the socket 
int createSocket(char * serverName, int port, struct sockaddr_in * dest)
{
	int sockfd;
	struct hostent *server;
	struct sockaddr_in serveraddr;
	
	//creates the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		fprintf(stderr, "Error opening socket");	

	//find the server ip address
	server = gethostbyname(serverName);
	if(server == NULL)
	{
		fprintf(stderr, "ERROR: no such host as %s\n", serverName);
		exit(1);
	}
	
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr = *((struct in_addr *)server->h_addr);

	//connects to the server
	if(connect(sockfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		fprintf(stderr, "ERROR CONNECTING\n");

	return sockfd;
}

//sends the message to the server
int sendRequest(int sock, char * request)
{
	printf("Sending request\n");
	int errorCheck = 0;
	errorCheck = send(sock, request, strlen(request), 0);
	if(errorCheck < 0)
	{
		perror("ERROR WRITING TO SOCKET");
		exit(1);
	}
	return errorCheck;
}

//receives the response from the server
int receiveResponse(int sock, char * response)
{
	int errorCheck = 0;
	errorCheck = recv(sock, response, 256, 0);
	return errorCheck;
}

//prints the response that the server gives
void printResponse(char* response)
{
	printf("\nResponse is: %s\n", response);
}

//CLOSES THE SOCKET THAT WAS CREATED AT THE END
int closeSocket(int sock)
{
	int errorCheck = 0;
	errorCheck = close(sock);
	return errorCheck;
}

