/*
 * Sam Stein
 * Joshua Wright
 *
 * TCPclient.java
 * Systems and Networks II
 * Project 1
 *
 * This file describes the functions to be implemented by the TCPclient class
 * You may also implement any auxillary functions you deem necessary.
 */
 
import java.io.*;
import java.net.*;
import java.util.Scanner;

       
public class TCPclient
{
	private Socket _socket; // the socket for communication with a server
	
	/**
	 * Constructs a TCPclient object.
	 */
	public TCPclient() 
	{	
	}
	
	/**
	 * Creates a streaming socket and connects to a server.
	 *
	 * @param host - the ip or hostname of the server
	 * @param port - the port number of the server
	 *
	 * @return - 0 or a negative number describing an error code if the connection could not be established
	 */
	public int createSocket(String host, int port)
	{	
		try {
			_socket = new Socket(host, port);  // Create the socket
		} catch (UnknownHostException e) {
			System.out.println("Unknown host");
			return 0;
		} catch (IOException e) {
			System.out.println("I/O errror");
			return 0;
		}
			
		return 1;
	}

	/**
	 * Sends a request for service to the server. Do not wait for a reply in this function. This will be
	 * an asynchronous call to the server.
	 * 
	 * @request - the request to be sent
	 *
	 * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int sendRequest(String request)
	{	
		try {
			OutputStream oStream = _socket.getOutputStream();  // Get output stream
			PrintWriter printWriter = new PrintWriter(oStream);

			byte[] message = request.getBytes(); //Use a byte array instead of a string
		
			oStream.write(message); // Write the message to the output stream
			printWriter.flush();  // Flush the output stream
		
		} catch(Exception e) {
			System.out.println("Connection error: unable to connect to the server");
			return -1;
		}
		return 0;
	}
	
	/**
	 * Receives the server's response. Also displays the response in a
	 * clear and concise format.
	 *
	 * @return - the server's response or NULL if an error occured
	 */
	public String receiveResponse()
	{
		byte[] retData = new byte[256];
		InputStream ret;
		String finalDat;
	
		try {
			ret = _socket.getInputStream();  // Get input stream
			ret.read(retData);  // Read from the data stream
			finalDat = new String(retData, "US-ASCII");  // format data
		} catch (IOException e) {
			System.err.println("Could not create input stream");
			return null;
		}
		return finalDat;
	}
	
	/*
    * Prints the response to the screen in a formatted way.
    *
    * response - the server's response as an XML formatted string
    *
    */
	public static void printResponse(String response)
	{
		System.out.println(response);
	}
 

	/*
	 * Closes an open socket.
	 *
    * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int closeSocket() 
	{
		try {
			_socket.close(); // Close the socket
		} catch (IOException e) {
			return -1;
		}
		return 0;
	}
}

