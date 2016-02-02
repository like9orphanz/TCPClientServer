CFLAGS = -g -Wall
CC = gcc
JC = javac
JFLAGS = -g


all : TCPserver TCPclientC TCPclient

TCPserver : TCPserver.c
	gcc -o TCPserver TCPserver.c -pthread

TCPclientC : TCPmain.c TCPclient.c TCPclient.h
	gcc -o TCPclientC TCPmain.c TCPclient.c TCPclient.h -pthread

TCPclient : TCPmain.java TCPclient.java
	javac TCPmain.java TCPclient.java
clean :
	rm TCPserver TCPclientC TCPclient.class TCPmain.class
