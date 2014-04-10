#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WindowsX.h>
#include "resource.h"

#define WM_SERVERTCP WM_APP+1
#define WM_SERVERUDP WM_APP+2
#define WM_TCPCONNECTED WM_APP+3
#define STAT_BUFFER 10000
#define BUFFER_LENGTH 700000

BOOL SetupServer(HWND hwnd, SOCKET* lpListenSocket, int port, BOOL protocol_Selected);
BOOL SetupTCPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr);
BOOL SetupUDPServer(SOCKET* lpListenSocket, SOCKADDR_IN serverAddr);
BOOL AcceptTCPConnections(HWND hwnd, SOCKET* lpListenSocket, SOCKET* lpAcceptSocket);
void TCPRead(SOCKET* lpAcceptSocket);
void UDPRead(SOCKET* lpListenSocket);
void stopServer(SOCKET serverSocket);

extern HANDLE serverSaveFile;
extern HANDLE serverSTATSFile;

#endif