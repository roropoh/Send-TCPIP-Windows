#ifndef EDITRESULTBOX_H
#define EDITRESULTBOX_H

#include <stdio.h>
#include <Windows.h>
#include <WindowsX.h>
#include "resource.h"

#define INSERT_POSITION 0

void resetResultBox();
void addToResultBox(const CHAR* s);

void AddTCPServerStats(BOOL ClientConnected, int bytesReceived);
void AddTCPClientStats(BOOL connected, int bytesSent);

void AddUDPServerStats(BOOL ListeningForClients, int bytesReceived);
void AddUDPClientStats(BOOL connected, int bytesSent);

extern HWND hwnd;

#endif