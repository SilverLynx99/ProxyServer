#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8888"
#define DEFAULT_BUFLEN 512

int main()
{
	
	// Send and receive data.
	//char recvbuf[DEFAULT_BUFLEN];
	//int iSendResult;
	//int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	//do {

	//	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	//	if (iResult > 0) {
	//		printf("Bytes received: %d\n", iResult);

	//		recvbuf[iResult] = '\0';

	//		// process to push get to real server
	//		printf("%s", recvbuf);

	//		printf("Press enter to end!");
	//		system("pause");

	//		//// Echo the buffer back to the sender
	//		//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
	//		//if (iSendResult == SOCKET_ERROR) {
	//		//	printf("send failed: %d\n", WSAGetLastError());
	//		//	closesocket(ClientSocket);
	//		//	WSACleanup();
	//		//	return 1;
	//		//}
	//		//printf("Bytes sent: %d\n", iSendResult);
	//	}
	//	else if (iResult == 0)
	//		printf("Connection closing...\n");
	//	else {
	//		printf("recv failed: %d\n", WSAGetLastError());
	//		closesocket(ClientSocket);
	//		WSACleanup();
	//		return 1;
	//	}

	//} while (iResult > 0);

	// close here
	return 0;

}

// How to use UDP
SOCKET* InitializeSocketForReceivingConnectionFromClients(int numOfSockets)
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return NULL;
	}

	struct addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return NULL;
	}

	// Create a SOCKET object called ListenSocket for the server to listen for client connections.
	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return NULL;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return NULL;
	}

	// Delete data in result pointer
	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return NULL;
	}

	// Create an array of SOCKET objects called ClientSockets for receive connection from client
	SOCKET* ClientSockets = new SOCKET[numOfSockets];
	for (int i = 0; i < numOfSockets; i++) {
		// Accept a client socket
		ClientSockets[i] = accept(ListenSocket, NULL, NULL);
		if (ClientSockets[i] == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return NULL;
		}
	}

	// No longer need server socket
	closesocket(ListenSocket);

	return ClientSockets;
}