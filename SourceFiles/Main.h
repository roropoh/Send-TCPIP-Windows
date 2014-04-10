#ifndef MAIN_H
#define MAIN_H


#include "resource.h"
#include "Server.h"
#include "Client.h"
#include "EditResultBox.h"
#include <Windows.h>
#include <WindowsX.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <cstdio>


#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define IDC_IP_INPUTFIELD		1001
#define IDC_PORT_INPUTFIELD		1002
#define IDC_PROTOCOL_RADIO_TCP  1003
#define IDC_PROTOCOL_RADIO_UDP  1004
#define WM_SOCKET				1005
#define IDC_PACKETSIZE_INPUT	1006
#define IDC_TIMESTOSEND_INPUT	1007

#define KB 1024

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void setup_InputFields(int cxChar, int cyChar, HWND hwnd, LPARAM lParam);
void startup_enable_inputFields(HWND hwnd);
void draw_text_title(HDC hdc, int cxChar, int cyChar);
void server_selected(HWND hwnd, HMENU hMenu);
void client_selected(HWND hwnd, HMENU hMenu);

BOOL OpenFile(HWND hwnd);
BOOL FileRead(HWND hwnd, LPTSTR pstrFileName);

#endif