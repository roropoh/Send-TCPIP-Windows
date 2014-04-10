/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Client.cpp - 
--
-- PROGRAM: Win32A2.exe
--
-- FUNCTIONS:
--	BOOL UDPCreateSocket(HWND hwnd, SOCKET* lpClientSocket, TCHAR* host, int port, struct hostent* hp);
--	void SendFileToUDP(SOCKET* lpUDPSendSocket, TCHAR* host, int port, int packetSize, int timesToSend);
--	BOOL ConnectToTCPServer(HWND hwnd, SOCKET* lpConnectSocket, TCHAR* host, int port);
--	void SendFileToTCP(SOCKET* lpConnectedSocket, int packetSize, int timesToSend);
--	void SendUDPRandomData(SOCKET* lpDirectedSocket, TCHAR* host, int port, int packetSize, int timesToSend);
--	void SendTCPRandomData(SOCKET* lpConnectedSocket, int packetSize, int timesToSend);
--	BOOL Check_IP(TCHAR* host, struct hostent* hp);
--	void RandomizePacket(int packetSize, TCHAR* packet);
--
-- DATE: February 6, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- NOTES:
-- This source file has the code for all the functions to manipulate the ListBox.
--
----------------------------------------------------------------------------------------------------------------------*/
#include "Client.h"

static int BytesSent;
TCHAR* TextFormatForClientStats = "[%02d:%02d:%02d:%03d] Bytes Sent: %d B\tTotal Bytes Sent: %d B\r\n\0";


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: UDPCreateSocket
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: UDPCreateSocket(HWND hwnd, SOCKET* lpClientSocket, TCHAR* host, int port, struct hostent* hp)
--
-- RETURNS: BOOL
--				returns when UDP socket has been created
--
-- NOTES:
-- This function will create an UDP socket.
------------------------------------------------------------------------------------------------------------------*/
BOOL UDPCreateSocket(HWND hwnd, SOCKET* lpClientSocket, TCHAR* host, int port, struct hostent* hp)
{
	int rc;

	if(Check_IP(host, hp) == 0)
	{
		return FALSE;
	}
	BytesSent = 0;
	if ((*lpClientSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, 0)) == INVALID_SOCKET)
	{
		return FALSE;
	}
	rc = WSAAsyncSelect(*lpClientSocket, hwnd, WM_CLIENT_UDP, FD_CLOSE);
	clientSTATSFile = CreateFile("UDPClientSTATS.txt",	GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
	return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SendFileToUDP
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SendFileToUDP(SOCKET* lpUDPSendSocket, TCHAR* host, int port, int packetSize, int timesToSend)
--
-- RETURNS: -
--
-- NOTES:
-- This function will send a file through UDP, to an UDP server.
------------------------------------------------------------------------------------------------------------------*/
void SendFileToUDP(SOCKET* lpUDPSendSocket, TCHAR* host, int port, int packetSize, int timesToSend)
{
	TCHAR* packet = (TCHAR*)malloc((sizeof(char) * packetSize) + 1);
	TCHAR statsString[STAT_BUFFER];
	WSABUF sendBuffer;
	DWORD dwF = 0, dwBytesSent = 0, dwBytesWritten = 0;
	BOOL position_EOF = FALSE;
	SOCKADDR_IN serverAddr;
	int i;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port   = htons(port);
	if ((serverAddr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) // Get ip
	{
		MessageBox(hwnd, "Cannot get host", "Error", MB_OK);
		return;
	}
	for (i = 0; i < packetSize; i++) 
	{
		if (lpFile[i] == '\0' || lpFile[i] == EOF)
			position_EOF = TRUE;

		if (position_EOF)
			packet[i] = ' ';
		else
			packet[i] = lpFile[i];
	}
	packet[i] = '\0';

	sendBuffer.buf = packet;
	sendBuffer.len = strlen(packet);

	for (i = 0; i < timesToSend; i++)
	{
		WSASendTo(*lpUDPSendSocket, &sendBuffer, 1, &dwBytesSent, dwF, //NULL, NULL, NULL, NULL);
			(SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL);

		GetSystemTime(&time);
		BytesSent += dwBytesSent;
		sprintf_s(statsString, STAT_BUFFER, TextFormatForClientStats, 
			(time.wHour+16) % 24, time.wMinute, time.wSecond, time.wMilliseconds, dwBytesSent, BytesSent);
		WriteFile(clientSTATSFile, statsString, strlen(statsString), &dwBytesWritten, NULL);
		resetResultBox();
		AddUDPClientStats(FALSE, BytesSent);
		Sleep(20);
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SendUDPRandomData
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SendUDPRandomData(SOCKET* lpDirectedSocket, TCHAR* host, int port, int packetSize, int timesToSend)
--
-- RETURNS: -
--
-- NOTES:
-- This function will send random data with a packetsize specified by the user.
------------------------------------------------------------------------------------------------------------------*/
void SendUDPRandomData(SOCKET* lpDirectedSocket, TCHAR* host, int port, int packetSize, int timesToSend)
{
	TCHAR* packet = (TCHAR*)malloc(sizeof(char) * packetSize);
	TCHAR statsString[STAT_BUFFER];
	WSABUF sendBuffer;
	DWORD dwF = 0, dwBytesSent= 0, dwBytesWritten = 0;
	SOCKADDR_IN serverAddr;
	int i, j;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port   = htons(port);
	if ((serverAddr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) // Get ip
	{
		MessageBoxA(hwnd, "Cannot get host", "Error", MB_OK);
		return;
	}

	RandomizePacket(packetSize, packet);

	sendBuffer.buf = packet;
	sendBuffer.len = strlen(packet);

	for (i = 0; i < timesToSend; i++)
	{
		j = WSASendTo(*lpDirectedSocket, &sendBuffer, 1, &dwBytesSent, dwF, //NULL, NULL, NULL, NULL);
			(SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL);
		GetSystemTime(&time);
		BytesSent += dwBytesSent;
		sprintf_s(statsString, STAT_BUFFER, TextFormatForClientStats, 
			(time.wHour+16) % 24, time.wMinute, time.wSecond, time.wMilliseconds, dwBytesSent, BytesSent);
		WriteFile(clientSTATSFile, statsString, strlen(statsString), &dwBytesWritten, NULL);
		resetResultBox();
		AddUDPClientStats(FALSE, BytesSent);
		Sleep(20);
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ConnectToTCPServer
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: ConnectToTCPServer(HWND hwnd, SOCKET* lpConnectSocket, TCHAR* host, int port)
--
-- RETURNS: BOOL
--				returns when connection has been made successfully
--
-- NOTES:
-- This function will connect to a TCP server.
------------------------------------------------------------------------------------------------------------------*/
BOOL ConnectToTCPServer(HWND hwnd, SOCKET* lpConnectSocket, TCHAR* host, int port)
{
	SOCKADDR_IN serverAddr;
	int rc;

	BytesSent = 0;

	if ((*lpConnectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0))
		== INVALID_SOCKET)
		return FALSE;

	WSAAsyncSelect(*lpConnectSocket, hwnd, WM_CLIENT_TCP, FD_CONNECT | FD_CLOSE);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port   = htons(port);
	if ((serverAddr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) // Get ip
	{
		MessageBoxA(hwnd, "Cannot get host", "Error", MB_OK);
		return FALSE;
	}

	rc = WSAConnect(*lpConnectSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);
	clientSTATSFile = CreateFile("TCPClientSTATS.txt",	GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);

	return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SendFileToTCP
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SendFileToTCP(SOCKET* lpConnectedSocket, int packetSize, int timesToSend)
--
-- RETURNS: -
--
-- NOTES:
-- This function send a file through a TCP connection.
------------------------------------------------------------------------------------------------------------------*/
void SendFileToTCP(SOCKET* lpConnectedSocket, int packetSize, int timesToSend)
{
	TCHAR* packet = (TCHAR*)malloc((sizeof(char) * packetSize) + 1);
	TCHAR  statsString[STAT_BUFFER];
	WSABUF sendBuffer;
	DWORD dwF = 0, dwBytesSent = 0, dwBytesWritten = 0;;
	BOOL position_EOF = FALSE;
	int i;

	for (i = 0; i < packetSize; i++) 
	{
		if (lpFile[i] == '\0' || lpFile[i] == EOF)
			position_EOF = TRUE;

		if (position_EOF)
			packet[i] = ' ';
		else
			packet[i] = lpFile[i];
	}
	packet[i] = '\0';

	sendBuffer.buf = packet;
	sendBuffer.len = strlen(packet);

	for (i = 0; i < timesToSend; i++)
	{
		WSASend(*lpConnectedSocket, &sendBuffer, 1, &dwBytesSent, dwF, NULL, NULL);
		GetSystemTime(&time);
		BytesSent += dwBytesSent;
		sprintf_s(statsString, STAT_BUFFER, TextFormatForClientStats, 
			(time.wHour+16) % 24, time.wMinute, time.wSecond, time.wMilliseconds, dwBytesSent, BytesSent);
		WriteFile(clientSTATSFile, statsString, strlen(statsString), &dwBytesWritten, NULL);
		resetResultBox();
		AddTCPClientStats(TRUE, BytesSent);
		Sleep(20);
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SendTCPRandomData
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SendTCPRandomData(SOCKET* lpConnectedSocket, int packetSize, int timesToSend)
--
-- RETURNS: -
--
-- NOTES:
-- This function send random data through a TCP connection.
------------------------------------------------------------------------------------------------------------------*/
void SendTCPRandomData(SOCKET* lpConnectedSocket, int packetSize, int timesToSend)
{
	TCHAR* packet = (TCHAR*)malloc(sizeof(char) * packetSize);
	TCHAR statsString[STAT_BUFFER];
	WSABUF sendBuffer;
	DWORD dwF = 0, dwBytesSent = 0, dwBytesWritten = 0;
	int i;

	RandomizePacket(packetSize, packet);

	sendBuffer.buf = packet;
	sendBuffer.len = strlen(packet);

	for (i = 0; i < timesToSend; i++)
	{
		WSASend(*lpConnectedSocket, &sendBuffer, 1, &dwBytesSent, dwF, NULL, NULL);
		BytesSent += dwBytesSent;
		GetSystemTime(&time);
		sprintf_s(statsString, STAT_BUFFER, TextFormatForClientStats, 
			(time.wHour+16) % 24, time.wMinute, time.wSecond, time.wMilliseconds, dwBytesSent, BytesSent);
		WriteFile(clientSTATSFile, statsString, strlen(statsString), &dwBytesWritten, NULL);
		resetResultBox();
		AddTCPClientStats(TRUE, BytesSent);
		Sleep(20);
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Check_IP
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: Check_IP(TCHAR* host, struct hostent* hp)
--
-- RETURNS: BOOL
--				returns TRUE if IP is valid
--
-- NOTES:
-- This function will check if the IP specified is valid.
------------------------------------------------------------------------------------------------------------------*/
BOOL Check_IP(TCHAR* host, struct hostent* hp)
{
	int test;
	struct in_addr	my_addr, *my_addr_p;
	char			ip[256];

	my_addr_p = (struct in_addr*) malloc(sizeof(struct in_addr));
	my_addr_p = &my_addr;

	if(isdigit(*host))
	{
		if((test = inet_addr(host)) == 0)
		{
			//failed
			return FALSE;
		}
		strcpy_s(ip, host);
		my_addr_p->s_addr = inet_addr(ip);

		hp = gethostbyaddr((char*)my_addr_p, sizeof (my_addr), AF_INET);
		if(hp == NULL)
		{
			addToResultBox("Error gethostbyaddr(): Bad IP");
			return FALSE;
		}


	}
	else
	{
		hp = gethostbyaddr((char*)my_addr_p, sizeof (my_addr), AF_INET);
		if(hp == NULL)
		{
			addToResultBox("Error gethostbyaddr(): Bad host name");
			return FALSE;
		}
	}
	return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: RandomizePacket
--
-- DATE: February 9, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: RandomizePacket(int packetSize, TCHAR* packet)
--
-- RETURNS: -
--
-- NOTES:
-- This function will randomize a packet with a specified packet size.
------------------------------------------------------------------------------------------------------------------*/
void RandomizePacket(int packetSize, TCHAR* packet)
{
	int i, j;
	for (i = 0; i < packetSize; i++)
	{
		j = (i < 26) ? i : i % 26;
		packet[i] = 'a' + j;
	}
	packet[i] = '\0';
}