/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Server.cpp - 
--
-- PROGRAM: Win32A2.exe
--
-- FUNCTIONS:
--	BOOL SetupServer(HWND hwnd, SOCKET* lpListenSocket, int port, BOOL protocol_Selected);
--	BOOL SetupTCPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr);
--	BOOL SetupUDPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr);
--	BOOL AcceptTCPConnections(HWND hwnd, SOCKET* lpListenSocket, SOCKET* lpAcceptSocket);
--	void TCPRead(SOCKET* lpAcceptSocket);
--	void UDPRead(SOCKET* lpListenSocket);
--	void stopServer(SOCKET serverSocket);
--
-- DATE: February 5, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- NOTES:
-- This source file has the code for all the functions to setup server side for both TCP and UDP conenctions.
----------------------------------------------------------------------------------------------------------------------*/
#include "Server.h"
#include "EditResultBox.h"

WSAData			wsaData;
static int		BytesRead;
SYSTEMTIME time;
TCHAR* TextFormatForServerStats = "[%02d:%02d:%02d:%03d] Bytes Received: %d B\tTotal Bytes Received: %d B\r\n\0";

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SetupServer
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SetupServer(HWND hwnd, SOCKET* lpListenSocket, int port, BOOL protocol_Selected)
--
-- RETURNS: BOOL
--				returns TRUE when either TCP/UDP server has been setup properly
--
-- NOTES:
-- This function will setup a TCP/UDP server.
------------------------------------------------------------------------------------------------------------------*/
BOOL SetupServer(HWND hwnd, SOCKET* lpListenSocket, int port, BOOL protocol_Selected)
{
	SOCKADDR_IN serverAddr;

	BytesRead = 0; 

	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	if(protocol_Selected == 1) // TCP
	{
		if(!(SetupTCPServer(lpListenSocket, serverAddr)))
		{
			return FALSE;
		}
		serverSaveFile = CreateFile("TCPServerSaveFile.txt", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		serverSTATSFile = CreateFile("TCPServerSTATS.txt", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		return TRUE;
	}
	else if(protocol_Selected == 2) // UDP
	{
		if(!(SetupUDPServer(lpListenSocket, serverAddr)))
		{
			return FALSE;
		}
		serverSaveFile = CreateFile("UDPServerSaveFile.txt", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		serverSTATSFile = CreateFile("UDPServerSTATS.txt", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		return TRUE;
	}

	return FALSE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SetupTCPServer
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SetupTCPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr)
--
-- RETURNS: BOOL
--				returns TRUE TCP Server has been set up
--
-- NOTES:
-- This function will setup a TCP server.
------------------------------------------------------------------------------------------------------------------*/
BOOL SetupTCPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr)
{
	if((*lpListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0)) == INVALID_SOCKET)
		{
			return FALSE;
		}
		WSAAsyncSelect(*lpListenSocket, hwnd, WM_SERVERTCP, FD_ACCEPT | FD_CLOSE);
		if (bind(*lpListenSocket, (PSOCKADDR) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
			return FALSE;

		if (listen(*lpListenSocket, 5))
			return FALSE;
		resetResultBox();
		HWND resultBox = GetDlgItem(hwnd, IDC_RESULTBOX);
		ListBox_InsertString(resultBox, INSERT_POSITION, "TCP Server Started");
		return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SetupUDPServer
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: SetupUDPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr)
--
-- RETURNS: BOOL
--				returns TRUE UDP socket has been opened properly
--
-- NOTES:
-- This function will open up socket for UDP server
------------------------------------------------------------------------------------------------------------------*/
BOOL SetupUDPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr)
{
        if ((*lpListenSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, 0)) == INVALID_SOCKET)
        {
            return FALSE;
        }
        WSAAsyncSelect(*lpListenSocket, hwnd, WM_SERVERUDP, FD_READ | FD_CLOSE);
        if (bind(*lpListenSocket, (PSOCKADDR) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            return FALSE;
        }
        resetResultBox();
        HWND resultBox = GetDlgItem(hwnd, IDC_RESULTBOX);
        ListBox_InsertString(resultBox, INSERT_POSITION, "UDP Server Started");
		return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AcceptTCPConnections
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: AcceptTCPConnections(HWND hwnd, SOCKET* lpListenSocket, SOCKET* lpAcceptSocket)
--
-- RETURNS: BOOL
--				returns TRUE when a TCP connection has been accepted
--
-- NOTES:
-- This function will accept TCP connections.
------------------------------------------------------------------------------------------------------------------*/
BOOL AcceptTCPConnections(HWND hwnd, SOCKET* lpListenSocket, SOCKET* lpAcceptSocket)
{
	SOCKADDR_IN sockaddr_accept;
	INT sockaddr_size = sizeof(sockaddr_accept);

	resetResultBox();
	addToResultBox("Listening");

	BytesRead = 0;

	if((*lpAcceptSocket = WSAAccept(*lpListenSocket, (SOCKADDR*)&sockaddr_accept, &sockaddr_size, NULL, NULL)) == INVALID_SOCKET)
	{
		return FALSE;
	}
	WSAAsyncSelect(*lpAcceptSocket, hwnd, WM_TCPCONNECTED, FD_READ | FD_CLOSE);
	resetResultBox();
	AddTCPServerStats(TRUE, BytesRead);
	addToResultBox("Accepted socket");
	return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: TCPRead
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: TCPRead(SOCKET* lpAcceptSocket)
--
-- RETURNS: -
--
-- NOTES:
-- This function will read from TCP stream.
------------------------------------------------------------------------------------------------------------------*/
void TCPRead(SOCKET* lpAcceptSocket)
{
	DWORD dwBytesReceived = 0, dwBytesWritten = 0, dwF = 0;
	TCHAR receiveBuffer[BUFFER_LENGTH] = "\0", statsString[STAT_BUFFER];
	WSABUF buffers;
	int rc;

	buffers.buf = receiveBuffer;
	buffers.len = BUFFER_LENGTH;

	rc = WSARecv(*lpAcceptSocket, &buffers, 1, &dwBytesReceived, &dwF, 0, 0);

	if (rc == SOCKET_ERROR)
	{
		rc = WSAGetLastError();
		return;
	}

	WriteFile(serverSaveFile, buffers.buf, strlen(buffers.buf), &dwBytesWritten, NULL);

	BytesRead += dwBytesReceived;
	GetSystemTime(&time);
	sprintf_s(statsString, STAT_BUFFER, TextFormatForServerStats, 
		(time.wHour+16) % 24, time.wMinute, time.wSecond, time.wMilliseconds, dwBytesReceived, BytesRead);
	WriteFile(serverSTATSFile, statsString, strlen(statsString), &dwBytesWritten, NULL);

	resetResultBox();
	AddTCPServerStats(TRUE, BytesRead);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: UDPRead
--
-- DATE: February 7, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: UDPRead(SOCKET* lpListenSocket)
--
-- RETURNS: -
--
-- NOTES:
-- This function will read from UDP socket.
------------------------------------------------------------------------------------------------------------------*/
void UDPRead(SOCKET* lpListenSocket)
{
	DWORD dwBytesReceived = 0, dwBytesWritten = 0, dwF;
	WSABUF buffers;
	SOCKADDR senderAddr;
	BOOL resized = FALSE;
	HWND lb = GetDlgItem(hwnd, IDC_RESULTBOX);
	TCHAR* receiveBuffer, statsString[STAT_BUFFER];
	int senderAddrSize = sizeof(struct sockaddr_in);
	int rc, bsize = 1;

	receiveBuffer = (TCHAR*)malloc(BUFFER_LENGTH * sizeof(TCHAR*));
	buffers.buf = receiveBuffer;
	buffers.len = BUFFER_LENGTH * bsize;

	do 
	{
		resized = FALSE;
		buffers.len = BUFFER_LENGTH * bsize;
		dwF = MSG_PEEK;

		if (WSARecvFrom(*lpListenSocket, &buffers, 1, &dwBytesReceived,
			&dwF, &senderAddr, &senderAddrSize, NULL, NULL) == SOCKET_ERROR)
		{
			switch (rc = WSAGetLastError())
			{
			case WSAEMSGSIZE:
				receiveBuffer = (TCHAR*)malloc((BUFFER_LENGTH * ++bsize)* sizeof(TCHAR*));
				dwBytesReceived = 0;
				resized = TRUE;
				break;
			default: 
				return;
			}
		}
	} while(dwBytesReceived == 0);

	WSARecvFrom(*lpListenSocket, &buffers, 1, &dwBytesReceived, &dwF, &senderAddr, &senderAddrSize, NULL, NULL);

	GetSystemTime(&time);
	buffers.buf[dwBytesReceived] = '\0';
	WriteFile(serverSaveFile, buffers.buf, dwBytesReceived, &dwBytesWritten, NULL);
	buffers.buf = NULL;
	BytesRead += dwBytesReceived;
	
	sprintf_s(statsString, STAT_BUFFER, TextFormatForServerStats, 
		(time.wHour+16) % 24, time.wMinute, time.wSecond, time.wMilliseconds, dwBytesReceived, BytesRead);
	WriteFile(serverSTATSFile, statsString, strlen(statsString), &dwBytesWritten, NULL);

	resetResultBox();
	AddUDPServerStats(TRUE, BytesRead);
	free(receiveBuffer);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: stopServer
--
-- DATE: February 11, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: stopServer(SOCKET serverSocket)
--
-- RETURNS: -
--
-- NOTES:
-- This function will stop the server and close all the handles for the server side.
------------------------------------------------------------------------------------------------------------------*/
void stopServer(SOCKET serverSocket)
{
	resetResultBox();
	closesocket(serverSocket);
	CloseHandle(serverSaveFile);
	WSACleanup();
	HWND resultBox = GetDlgItem(hwnd, IDC_RESULTBOX);
	ListBox_InsertString(resultBox, INSERT_POSITION, "Server Stopped");
}

