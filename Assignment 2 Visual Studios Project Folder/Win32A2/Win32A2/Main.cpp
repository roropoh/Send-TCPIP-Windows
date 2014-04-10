/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Source.cpp - An application that will allow the user to choose either
--								to host a server, or be the client with the option of 
--								using either the TCP, or the UDP protocol.
--
-- PROGRAM: Win32A2.exe
--
-- FUNCTIONS:
--	LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
--	void setup_InputFields(int cxChar, int cyChar, HWND hwnd, LPARAM lParam);
--	void startup_enable_inputFields(HWND hwnd);
--	void draw_text_title(HDC hdc, int cxChar, int cyChar);
--	void server_selected(HWND hwnd, HMENU hMenu);
--	void client_selected(HWND hwnd, HMENU hMenu);
--	BOOL OpenFile(HWND hwnd);
--	BOOL FileRead(HWND hwnd, LPTSTR pstrFileName);

--
-- DATE: February 1, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- NOTES:
-- The program will allow the user to select either being a Server, or Client. On the server side,
--	the user will be able to select what port to open through TCP or UDP connections. On the client side,
--	the user will be able to select the packet size of the file, as well as the times it wants to send it
--	over the TCP or UDP connection.
--
-- Remember to add "WS2_32.Lib" to the project source list.
----------------------------------------------------------------------------------------------------------------------*/
#include "Main.h"

TCHAR szAppName[] = TEXT ("ASSIGNMENT2") ;

HWND        IP_inputField;
HWND		port_inputField;
HWND		packetSize_inputField;
HWND		timesToSend_inputField;
HWND		tcp_protocol_button;
HWND		udp_protocol_button;
HWND		resultBox;
HWND		start_serv_button;
HWND		send_file_button;
HWND		send_random_button;
HWND		connect_button;
HWND		disconnect_button;
char*		result;
char*		defaultIP = "192.168.1.203";
char*		defaultPort = "5150";

BOOL		ServerStarted;
BOOL		protocol_Selected;

OPENFILENAME openfilename;
HANDLE		serverSTATSFile;
HANDLE		clientSTATSFile;
HANDLE		serverSaveFile;

HWND		hwnd;
TCHAR		lpFileName[MAX_PATH];
TCHAR*		lpFile;
struct hostent	*hp;
SOCKET serverSocket, clientSocket;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: February 2, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
--                HINSTANCE hInst: Handle to the current instance of the program.
--                HINSTANCE hPrevInstance: Handle to the previous instance of the program.
--                LPSTR lspszCmdParam: Command line for the application.
--                int nCmdShow: Control for how the window should be shown.
--
-- RETURNS: Returns the exit value upon exit.
--
-- NOTES:
-- This function is the entry point for a graphical Windows-based application.
------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	MSG      msg ;
	ZeroMemory(&msg,sizeof(MSG));
	WNDCLASS wndclass ;

	WSADATA wsaData;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1); ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, TEXT ("Robin Hsieh Assignment 2"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL, NULL, hInstance, NULL) ;

	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		return FALSE;
	}

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: February 2, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--                HWND hwnd: Handle to the window
--                UINT Message: The message
--                WPARAM wParam: Parameter
--                LPARAM lParam: Parameter
--
-- RETURNS: 
--
-- NOTES:
-- This function that handles the different type of window messages.
------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HMENU hMenu;
	static int cxChar;
	static int cyChar;
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;
	hdc = GetDC (hwnd) ;
	GetWindowRect(hwnd, &rect);
	SOCKET acceptSocket;


	switch(message)
	{
	case WM_COMMAND:
		hMenu = GetMenu (hwnd);
		hdc = GetDC(hwnd);
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT));

		TCHAR host[MAX_PATH];
		TCHAR port[MAX_PATH];
		TCHAR packetSize[MAX_PATH];
		TCHAR timesToSend[MAX_PATH];

		GetWindowText(GetDlgItem(hwnd, IDC_IP_INPUTFIELD), host, MAX_PATH);
		GetWindowText(GetDlgItem(hwnd, IDC_PORT_INPUTFIELD), port, MAX_PATH);
		GetWindowText(GetDlgItem(hwnd, IDC_PACKETSIZE_INPUT), packetSize, MAX_PATH);
		GetWindowText(GetDlgItem(hwnd, IDC_TIMESTOSEND_INPUT), timesToSend, MAX_PATH);

		switch(LOWORD (wParam))
		{
		case ID_SC_SERVER:
			server_selected(hwnd, hMenu);
			break;

		case ID_START_SERVER_BUTTON:
			if(ServerStarted == 0)
			{
				if(protocol_Selected == 1)
				{
					if(SetupServer(hwnd, &serverSocket, atoi(port), protocol_Selected) == TRUE)
					{
						ServerStarted = 1;
						Button_SetText(GetDlgItem( hwnd, ID_START_SERVER_BUTTON ), "STOP SERVER");
						EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_TCP), FALSE);
						EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_UDP), FALSE);
					}
				}
				else if(protocol_Selected == 2)
				{
					if(SetupServer(hwnd, &serverSocket, atoi(port), protocol_Selected) == TRUE)
					{
						ServerStarted = 1;
						Button_SetText(GetDlgItem( hwnd, ID_START_SERVER_BUTTON ), "STOP SERVER");
						EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_TCP), FALSE);
						EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_UDP), FALSE);
					}
				}
				else if(protocol_Selected == 0)
				{
					resetResultBox();
					addToResultBox("Must select protocol before starting server");
				}

			}
			else if(ServerStarted == 1)
			{
				ServerStarted = 0;
				stopServer(serverSocket);
				Button_SetText(GetDlgItem( hwnd, ID_START_SERVER_BUTTON ), "START SERVER");
				EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_TCP), TRUE);
				EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_UDP), TRUE);
			}
			break;

		case ID_FILE_SAVEFILE:
			break;

		case ID_SC_CLIENT:
			client_selected(hwnd, hMenu);
			break;

		case ID_CONNECT_BUTTON:

			if(protocol_Selected == 1) // TCP
			{
				if(Check_IP(host, hp) == 0)
				{
					break;
				}
				if(ConnectToTCPServer(hwnd, &clientSocket, host, atoi(port)) == 0)
				{
					break;
				}

				resetResultBox();
				addToResultBox("Ready to send");
				addToResultBox("Connected to TCP server");
			}
			else if(protocol_Selected == 2) // UDP
			{
				if(Check_IP(host, hp) == 0)
				{
					break;
				}
				if(UDPCreateSocket(hwnd, &clientSocket, host, atoi(port), hp) == 0)
				{
					break;
				}
				resetResultBox();
				addToResultBox("Ready to send");
				addToResultBox("Connected to UDP server");
			}
			else //NOT SELECTED
			{
				resetResultBox();
				addToResultBox("Protocol must be selected");
				break;
			}
			EnableWindow( GetDlgItem( hwnd, ID_CONNECT_BUTTON ), FALSE);
			EnableWindow( GetDlgItem( hwnd, ID_DISCONNECT_BUTTON ), TRUE);
			EnableWindow( GetDlgItem( hwnd, ID_SENDFILE_BUTTON ), TRUE);
			EnableWindow( GetDlgItem( hwnd, ID_SENDRANDOM_BUTTON ), TRUE);
			EnableWindow( GetDlgItem( hwnd, IDC_TIMESTOSEND_INPUT), TRUE);
			EnableWindow( GetDlgItem( hwnd, IDC_PACKETSIZE_INPUT ), TRUE);
			break;

		case ID_DISCONNECT_BUTTON:

			closesocket(clientSocket);
			closesocket(serverSocket);
			CloseHandle(serverSTATSFile);
			CloseHandle(clientSTATSFile);
			CloseHandle(serverSaveFile);
			resetResultBox();
			addToResultBox("Disconnected from server");

			EnableWindow( GetDlgItem( hwnd, ID_CONNECT_BUTTON ), TRUE);
			EnableWindow( GetDlgItem( hwnd, ID_DISCONNECT_BUTTON ), FALSE);
			EnableWindow( GetDlgItem( hwnd, ID_SENDFILE_BUTTON ), FALSE);
			EnableWindow( GetDlgItem( hwnd, ID_SENDRANDOM_BUTTON ), FALSE);
			EnableWindow( GetDlgItem( hwnd, IDC_TIMESTOSEND_INPUT), FALSE);
			EnableWindow( GetDlgItem( hwnd, IDC_PACKETSIZE_INPUT ), FALSE);
			break;

		case ID_FILE_SENDFILE:
		case ID_SENDFILE_BUTTON:
			if (!OpenFile(hwnd))
			{
				break;
			}
			if (!FileRead(hwnd, lpFileName))
			{
				break;
			}
			if(protocol_Selected == 1) // TCP
			{
				SendFileToTCP(&clientSocket, atoi(packetSize) * KB, atoi(timesToSend));
			}
			else if(protocol_Selected == 2) // UDP
			{
				SendFileToUDP(&clientSocket, host, atoi(port), atoi(packetSize) * KB, atoi(timesToSend));
			}
			break;

		case ID_SENDRANDOM_BUTTON:
			if(protocol_Selected == 1) // TCP
			{
				SendTCPRandomData(&clientSocket, atoi(packetSize) * KB, atoi(timesToSend));
			}
			else if(protocol_Selected == 2) // UDP
			{
				SendUDPRandomData(&clientSocket, host, atoi(port), atoi(packetSize) * KB, atoi(timesToSend));
			}
			break;

		case IDC_PROTOCOL_RADIO_TCP:
			protocol_Selected = 1;
			break;
		case IDC_PROTOCOL_RADIO_UDP:
			protocol_Selected = 2;
			break;
		}
		break;

	case WM_SERVERTCP:
		switch(lParam)
		{
		case FD_ACCEPT:
			AcceptTCPConnections(hwnd, &serverSocket, &acceptSocket);
			break;
		case FD_CLOSE:
			closesocket(wParam);
			break;
		}

		break;
	case WM_SERVERUDP:
		switch(lParam)
		{
		case FD_READ:
			UDPRead(&serverSocket);
			break;
		case FD_CLOSE:
			closesocket(wParam);
		}
		break;

	case WM_TCPCONNECTED:
		switch(lParam)
		{
		case FD_READ:
			TCPRead(&(SOCKET)wParam);
			break;
		case FD_CLOSE:
			closesocket(wParam);
			break;
		}
		break;

	case WM_CLIENT_UDP:
		switch (lParam)
		{
		case FD_CLOSE:
			closesocket(wParam);
			break;
		}
		break;



	case WM_CREATE:
		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;
		hdc = GetDC (hwnd) ;
		GetWindowRect(hwnd, &rect);

		setup_InputFields(cxChar, cyChar, hwnd, lParam);
		hp = (struct hostent*) malloc(sizeof(MAXGETHOSTSTRUCT));
		lpFileName[0] = '\0';

		startup_enable_inputFields(hwnd);
		ListBox_Enable(resultBox, FALSE);
		break;

	case WM_PAINT:
		hdc = BeginPaint (hwnd, &ps) ;
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
		SetBkMode (hdc, TRANSPARENT) ;

		draw_text_title(hdc, cxChar, cyChar);

		ReleaseDC(hwnd, hdc);
		EndPaint (hwnd, &ps) ;
		break;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		shutdown(serverSocket,SD_BOTH);
		closesocket(serverSocket);
		WSACleanup();
		return 0 ;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: draw_text_title
--
-- DATE: January 8, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void draw_text_title(HDC hdc, int cxChar, int cyChar)
--                HDC hdc: hdc
--                int cxChar: The x-coordinate to be printed.
--                int cyChar: The y-coordinate to be printed.
--
-- RETURNS: -
--
-- NOTES:
-- This function draws the titles in the window.
------------------------------------------------------------------------------------------------------------------*/
void draw_text_title(HDC hdc, int cxChar, int cyChar)
{
	CHAR* ip_title = "IP Address";
	CHAR* port_title = "Port Number";
	CHAR* packetsize_title = "Packet size: (kB)";
	CHAR* timestosend_title = "Times to send:";

	TextOut(hdc, cxChar, cyChar, ip_title, strlen(ip_title));
	TextOut(hdc, cxChar, cyChar * 4, port_title, strlen(port_title));
	TextOut(hdc, cxChar, cyChar * 17, packetsize_title, strlen(packetsize_title));
	TextOut(hdc, cxChar, cyChar * 20, timestosend_title, strlen(timestosend_title));
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: server_selected
--
-- DATE: January 31, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void server_selected(HWND hwnd, HMENU hMenu)
--
-- RETURNS: -
--
-- Notes:
-- Enables the start server button for server.
------------------------------------------------------------------------------------------------------------------*/
void server_selected(HWND hwnd, HMENU hMenu)
{
	EnableWindow( GetDlgItem( hwnd, IDC_PORT_INPUTFIELD ), TRUE);
	EnableWindow( GetDlgItem( hwnd, ID_START_SERVER_BUTTON ), TRUE);
	EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_TCP), TRUE);
	EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_UDP), TRUE);
	EnableWindow( GetDlgItem( hwnd, ID_SENDFILE_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_SENDRANDOM_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, IDC_IP_INPUTFIELD ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_CONNECT_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_DISCONNECT_BUTTON ), FALSE);

	CheckMenuItem(hMenu, ID_SC_CLIENT, MF_UNCHECKED); 
	CheckMenuItem(hMenu, ID_SC_SERVER, MF_CHECKED); 
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: client_selected
--
-- DATE: January 31, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void client_selected(HWND hwnd, HMENU hMenu)
--
-- RETURNS: -
--
-- Notes:
-- Enables the send button for client.
------------------------------------------------------------------------------------------------------------------*/
void client_selected(HWND hwnd, HMENU hMenu)
{
	EnableWindow( GetDlgItem( hwnd, ID_CONNECT_BUTTON ), TRUE);
	EnableWindow( GetDlgItem( hwnd, IDC_PORT_INPUTFIELD ), TRUE);
	EnableWindow( GetDlgItem( hwnd, IDC_IP_INPUTFIELD ), TRUE);
	EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_TCP), TRUE);
	EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_UDP), TRUE);
	EnableWindow( GetDlgItem( hwnd, ID_START_SERVER_BUTTON ), FALSE);
	CheckMenuItem(hMenu, ID_SC_SERVER, MF_UNCHECKED); 
	CheckMenuItem(hMenu, ID_SC_CLIENT, MF_CHECKED); 
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: startup_enable_inputFields
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void startup_enable_inputFields(HWND hwnd)
--                HWND hwnd: The handle to the window
--
-- RETURNS: -
--
-- NOTES:
-- This function sets all the input fields initially to false so nothing can be inputted.
------------------------------------------------------------------------------------------------------------------*/
void startup_enable_inputFields(HWND hwnd)
{
	EnableWindow( GetDlgItem( hwnd, IDC_IP_INPUTFIELD ), FALSE);
	EnableWindow( GetDlgItem( hwnd, IDC_PORT_INPUTFIELD ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_SENDFILE_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_SENDRANDOM_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_START_SERVER_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_CONNECT_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, ID_DISCONNECT_BUTTON ), FALSE);
	EnableWindow( GetDlgItem( hwnd, IDC_TIMESTOSEND_INPUT), FALSE);
	EnableWindow( GetDlgItem( hwnd, IDC_PACKETSIZE_INPUT ), FALSE);
	EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_TCP), FALSE);
	EnableWindow( GetDlgItem( hwnd, IDC_PROTOCOL_RADIO_UDP), FALSE);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: setup_InputFields
--
-- DATE: February 10, 2014
--
-- DESIGNER: Robin Hsieh A00657820
--
-- PROGRAMMER: Robin Hsieh A00657820
--
-- INTERFACE: void setup_InputFields(int cxChar, int cyChar, HWND hwnd, LPARAM lParam)
--                int cxChar: The x position on the window.
--				  int cyChar: The y position on the window.
--				  HWND hwnd: The handle of the window.
--				  LPARAM lParam: The lParam of the window.
--
-- RETURNS: -
--
-- NOTES:
-- This function sets all the input fields.
------------------------------------------------------------------------------------------------------------------*/
void setup_InputFields(int cxChar, int cyChar, HWND hwnd, LPARAM lParam)
{
	IP_inputField = CreateWindow(TEXT("EDIT"), defaultIP,
		WS_BORDER|WS_CHILD|WS_VISIBLE, 
		cxChar, cyChar*2, 30 * cxChar, cyChar * 4 / 3 , 
		hwnd, (HMENU)IDC_IP_INPUTFIELD, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	port_inputField = CreateWindow(TEXT("EDIT"), defaultPort,
		WS_BORDER|WS_CHILD|WS_VISIBLE, 
		cxChar, cyChar*5, 10 * cxChar, cyChar * 4 / 3 , 
		hwnd, (HMENU)IDC_PORT_INPUTFIELD, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	tcp_protocol_button = CreateWindow(TEXT("BUTTON"), TEXT ("TCP"), 
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 
		cxChar, cyChar*8, 7 * cxChar, 7 * cyChar / 4, 
		hwnd, (HMENU)IDC_PROTOCOL_RADIO_TCP, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	udp_protocol_button = CreateWindow(TEXT("BUTTON"), TEXT ("UDP"), 
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 
		8*cxChar, cyChar*8, 7 * cxChar, 7 * cyChar / 4, 
		hwnd, (HMENU)IDC_PROTOCOL_RADIO_UDP, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	connect_button = CreateWindow(TEXT("BUTTON"), TEXT ("Connect"), 
		WS_BORDER|WS_CHILD|WS_VISIBLE,
		cxChar, cyChar * 11, 13 * cxChar, 8 * cyChar / 4, 
		hwnd, (HMENU)ID_CONNECT_BUTTON, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	disconnect_button = CreateWindow(TEXT("BUTTON"), TEXT ("Disconnect"), 
		WS_BORDER|WS_CHILD|WS_VISIBLE,
		16*cxChar, cyChar * 11, 13 * cxChar, 8 * cyChar / 4, 
		hwnd, (HMENU)ID_DISCONNECT_BUTTON, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	send_file_button = CreateWindow(TEXT("BUTTON"), TEXT ("Send File"), 
		WS_BORDER|WS_CHILD|WS_VISIBLE, 
		cxChar, cyChar * 14, 13 * cxChar, 8 * cyChar / 4, 
		hwnd, (HMENU)ID_SENDFILE_BUTTON, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	send_random_button = CreateWindow(TEXT("BUTTON"), TEXT ("Send Random"), 
		WS_BORDER|WS_CHILD|WS_VISIBLE,
		16*cxChar, cyChar * 14, 13 * cxChar, 8 * cyChar / 4, 
		hwnd, (HMENU)ID_SENDRANDOM_BUTTON, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	start_serv_button = CreateWindow(TEXT("BUTTON"), TEXT ("START SERVER"), 
		WS_BORDER|WS_CHILD|WS_VISIBLE, 
		cxChar * 14, cyChar * 4, 16 * cxChar, 13 * cyChar / 4, 
		hwnd, (HMENU)ID_START_SERVER_BUTTON, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	packetSize_inputField = CreateWindow(TEXT("EDIT"), "1",
		WS_BORDER|WS_CHILD|WS_VISIBLE, 
		cxChar, cyChar*18, 10 * cxChar, cyChar * 4 / 3 , 
		hwnd, (HMENU)IDC_PACKETSIZE_INPUT, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	timesToSend_inputField = CreateWindow(TEXT("EDIT"), "1",
		WS_BORDER|WS_CHILD|WS_VISIBLE, 
		cxChar, cyChar*21, 10 * cxChar, cyChar * 4 / 3 , 
		hwnd, (HMENU)IDC_TIMESTOSEND_INPUT, ((LPCREATESTRUCT)lParam)->hInstance, 0);

	resultBox = CreateWindow(TEXT("listbox"), NULL, 
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL, 
		cxChar * 35, 1* cyChar, 475, 490,
		hwnd, (HMENU)IDC_RESULTBOX, 
		NULL, NULL);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: OpenFile
--
-- DATE: November 12, 2013
--
-- REVISIONS: 2014/01/28 - Vincent - Revised for reuse. Complete initialization
-- 
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL OpenFile (HWND hwnd)
--				HWND hwnd: Handle to the window
--
-- RETURNS: Returns true if able to get a file back after selecting.
--
-- NOTES:
-- This function initializes the parameters that are going to be used to for opening a file.
-- Opens a file dialog for the user to select what file they want to open.
-- This function was used in the Wireless protocol group project in Term 3.
------------------------------------------------------------------------------------------------------------------*/
BOOL OpenFile(HWND hwnd)
{
	ZeroMemory(&openfilename, sizeof(openfilename));

	openfilename.lStructSize = sizeof(openfilename); 
	openfilename.hwndOwner	= hwnd;
	openfilename.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	openfilename.lpstrFile	= lpFileName;
	openfilename.nMaxFile	= MAX_PATH;
	openfilename.Flags		= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	openfilename.lpstrDefExt = "txt";
	openfilename.nFileOffset		= 0 ;
	openfilename.nFileExtension	= 0 ;
	openfilename.nMaxCustFilter	= 0 ;
	openfilename.nFilterIndex	= 0 ;

	return (GetOpenFileName(&openfilename));
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: FileRead
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL FileRead(HWND hwndEdit, LPCSTR pstrFileName)
--				HWND hwnd: Handle to the file selection
--				LPTSTR pstrFileName: Pointer to the file name
--
-- RETURNS: Returns true if able to successfully read the file.
--
-- NOTES:
-- This function proceeds the FileOpen function and is used to read in the file.
-- This function was used in the Wireless protocol group project in Term 3.
------------------------------------------------------------------------------------------------------------------*/
BOOL FileRead(HWND hwnd, LPTSTR pstrFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);

		if(dwFileSize  != 0xFFFFFFFF)
		{
			lpFile = (TCHAR*) malloc(dwFileSize + 1);
			if(lpFile != NULL)
			{
				DWORD dwRead;
				if(ReadFile(hFile, lpFile, dwFileSize, &dwRead, NULL))
				{
					lpFile[dwFileSize] = '\0' ;
					bSuccess = TRUE;
				}
			}
		}

		CloseHandle(hFile);
	}
	return bSuccess;
}