#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WindowsX.h>
#include "resource.h"
#include "EditResultBox.h"

#define WM_CLIENT_UDP WM_APP+4
#define WM_CLIENT_TCP WM_APP+5
#define STAT_BUFFER 10000


BOOL UDPCreateSocket(HWND hwnd, SOCKET* lpClientSocket, TCHAR* host, int port, struct hostent* hp);
void SendFileToUDP(SOCKET* lpUDPSendSocket, TCHAR* host, int port, int packetSize, int timesToSend);

BOOL ConnectToTCPServer(HWND hwnd, SOCKET* lpConnectSocket, TCHAR* host, int port);
void SendFileToTCP(SOCKET* lpConnectedSocket, int packetSize, int timesToSend);

void SendUDPRandomData(SOCKET* lpDirectedSocket, TCHAR* host, int port, int packetSize, int timesToSend);
void SendTCPRandomData(SOCKET* lpConnectedSocket, int packetSize, int timesToSend);

BOOL Check_IP(TCHAR* host, struct hostent* hp);
void RandomizePacket(int packetSize, TCHAR* packet);

extern TCHAR* lpFile;
extern HANDLE clientSTATSFile;
extern SYSTEMTIME time;

#endif