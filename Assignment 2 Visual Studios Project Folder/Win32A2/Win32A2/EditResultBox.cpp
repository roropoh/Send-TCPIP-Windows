/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: EditResultBox.cpp - 
--
-- PROGRAM: Win32A2.exe
--
-- FUNCTIONS:
--	void resetResultBox();
--	void addToResultBox(const CHAR* s);
--	void AddTCPServerStats(BOOL ClientConnected, int bytesReceived);
--	void AddTCPClientStats(BOOL connected, int bytesSent);
--	void AddUDPServerStats(BOOL ListeningForClients, int bytesReceived);
--	void AddUDPClientStats(BOOL connected, int bytesSent);
--
-- DATE: February 8, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- NOTES:
-- This source file has the code for all the functions to manipulate the ListBox.
--
----------------------------------------------------------------------------------------------------------------------*/

#include "EditResultBox.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: resetResultBox
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void resetResultBox()
--
-- RETURNS: -
--
-- NOTES:
-- This function resets the result box.
------------------------------------------------------------------------------------------------------------------*/
void resetResultBox()
{
	HWND resultBox = GetDlgItem(hwnd, IDC_RESULTBOX);
	ListBox_ResetContent(resultBox);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: addToResultBox
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void addToResultBox(const CHAR* s)
--
-- RETURNS: -
--
-- NOTES:
-- This function adds a string to the result box.
------------------------------------------------------------------------------------------------------------------*/
void addToResultBox(const CHAR* s)
{
	HWND resultBox = GetDlgItem(hwnd, IDC_RESULTBOX);
	ListBox_InsertString(resultBox, INSERT_POSITION, s);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AddTCPServerStats
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: AddTCPServerStats(BOOL ClientConnected, int bytesReceived)
--
-- RETURNS: -
--
-- NOTES:
-- This function will display refresh the TCP Server Stats on the listbox
------------------------------------------------------------------------------------------------------------------*/
void AddTCPServerStats(BOOL ClientConnected, int bytesReceived)
{
	HWND lb = GetDlgItem(hwnd, IDC_RESULTBOX);
	TCHAR ClientConnectedString[MAX_PATH]	= "Client connected: ";
	TCHAR BytesReceivedTitle[MAX_PATH]		= "Bytes Received: ";
	TCHAR BytesRecievedString[MAX_PATH];
	TCHAR* cc;

	if(ClientConnected == TRUE)
	{
		cc = "Yes";
	}
	else
	{
		cc = "No";
	}
	sprintf_s(BytesRecievedString, "%d", bytesReceived);
	strcat_s(ClientConnectedString, cc);
	strcat_s(BytesReceivedTitle, BytesRecievedString);

	ListBox_InsertString(lb, INSERT_POSITION, ClientConnectedString);
	ListBox_InsertString(lb, INSERT_POSITION+1, BytesReceivedTitle);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AddTCPClientStats
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: AddUDPClientStats(BOOL connected, int bytesSent)
--
-- RETURNS: -
--
-- NOTES:
-- This function will display refresh the TCP Client Stats on the listbox
------------------------------------------------------------------------------------------------------------------*/
void AddTCPClientStats(BOOL connected, int bytesSent)
{
	HWND lb = GetDlgItem(hwnd, IDC_RESULTBOX);
	TCHAR* connect;
	TCHAR TCPServerTitle[MAX_PATH]	= "Connected to TCP Server";
	TCHAR connectString[MAX_PATH]	= "Connected: ";
	TCHAR BytesSent[MAX_PATH];
	TCHAR BytesSentString[MAX_PATH] = "Bytes Sent: ";

	if (connected)
		connect = "Yes";
	else
		connect = "No";

	sprintf_s(BytesSent, "%d", bytesSent);
	strcat_s(connectString, connect);
	strcat_s(BytesSentString, BytesSent);
	ListBox_InsertString(lb, INSERT_POSITION, connectString);
	ListBox_InsertString(lb, INSERT_POSITION+1, BytesSentString);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AddUDPServerStats
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: AddUDPServerStats(BOOL ListeningForClients, int bytesReceived)
--
-- RETURNS: -
--
-- NOTES:
-- This function will display refresh the UDP Server Stats on the listbox
------------------------------------------------------------------------------------------------------------------*/
void AddUDPServerStats(BOOL ListeningForClients, int bytesReceived)
{
	HWND lb = GetDlgItem(hwnd, IDC_RESULTBOX);
	TCHAR ListeningForClientsString[MAX_PATH]	= "ListeningForClients: ";
	TCHAR BytesReceivedTitle[MAX_PATH]		= "Bytes Received: ";
	TCHAR BytesRecievedString[MAX_PATH];
	TCHAR* lfc;

	if(ListeningForClients == TRUE)
	{
		lfc = "Yes";
	}
	else
	{
		lfc = "No";
	}
	sprintf_s(BytesRecievedString, "%d", bytesReceived);
	strcat_s(ListeningForClientsString, lfc);
	strcat_s(BytesReceivedTitle, BytesRecievedString);

	ListBox_InsertString(lb, INSERT_POSITION, ListeningForClientsString);
	ListBox_InsertString(lb, INSERT_POSITION+1, BytesReceivedTitle);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AddUDPClientStats
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: AddUDPClientStats(BOOL connected, int bytesSent)
--
-- RETURNS: -
--
-- NOTES:
-- This function will display refresh the UDP Client Stats on the listbox
------------------------------------------------------------------------------------------------------------------*/
void AddUDPClientStats(BOOL connected, int bytesSent)
{
	HWND lb = GetDlgItem(hwnd, IDC_RESULTBOX);
	TCHAR* connect;
	TCHAR UDPServerTitle[MAX_PATH]	= "Opened Socket to UDP Server";
	TCHAR connectString[MAX_PATH]	= "Connected: ";
	TCHAR BytesSent[MAX_PATH];
	TCHAR BytesSentString[MAX_PATH]		= "Bytes Sent: ";

	if (connected)
		connect = "Yes";
	else
		connect = "No";

	sprintf_s(BytesSent, "%d", bytesSent);
	strcat_s(connectString, connect);
	strcat_s(BytesSentString, BytesSent);
	ListBox_InsertString(lb, INSERT_POSITION, connectString);
	ListBox_InsertString(lb, INSERT_POSITION+1, connectString);
	ListBox_InsertString(lb, INSERT_POSITION+2, BytesSentString);
}

