// pkEng.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Systems.h"
#include "windows.h"
#include "string.h"
#pragma comment(lib,"Systems.lib") 

void CreateChildProcess(int window_flag = 0x00000010)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	TCHAR szCmdline[] = TEXT("pkEng.exe -console");
	if (!CreateProcess(NULL,   // No module name (use command line)
	//	argv[1],        // Command line
		szCmdline,
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		window_flag,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}




int main(int argc, char* argv[])
{
	
	bool show_console = true;
	int type = 0;

	for (int i = 0; i < argc; i++)
	{
		std::string a(argv[i]);
		if (a == "-with_console")
		{
			show_console = true;
		}

		if (a == "-console")
		{
			type = 1;
		}
		std::cout << argv[i] << '\n';

	}
	 
	int WindowFlag;
	if (show_console)
	{
		WindowFlag = 0x00000010; //CREATE_NEW_CONSOLE
	}
	else
	{
		WindowFlag = 0x08000000; //CREATE_NO_WINDOW
	}
	
	if (type == 0)
	{
		std::cout << "THIS IS MAIN PROCESS" << '\n';
		pk42Core* sys = new pk42Core;
		sys->startUp();
		CreateChildProcess(WindowFlag);
		sys->runGameLoop();
		sys->shutDown();
	}
	else
	{
		std::cout << "THIS IS CONNECTED CONSOLE" << '\n';
		pk42Console* sys = new pk42Console;
		sys->startUp();
		sys->continuousRun();
		sys->shutDown();
	}	
	
	/*
	while (true)
	{
		int a;
		std::cin >> a;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		HWND wnd = GetConsoleWindow();
		WINDOWINFO wndInfo;

		GetWindowInfo(wnd, &wndInfo);

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		short columns = csbi.dwSize.X;
		short rows = csbi.dwSize.Y;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

		
		int left = csbi.srWindow.Left;// ((int)csbi.srWindow.Left + (int)csbi.srWindow.Right) / 2;
		int right = csbi.srWindow.Top; //((int)csbi.srWindow.Top + (int)csbi.srWindow.Bottom) / 2;
		int centerx = (csbi.srWindow.Left + csbi.srWindow.Right) / 2;
		int centery = (csbi.srWindow.Top + csbi.srWindow.Bottom) / 2;
		std::cout << wndInfo.rcClient.left << " " << wndInfo.rcClient.top << " "
			<< wndInfo.rcClient.right << " " << wndInfo.rcClient.bottom << " "
			<< centerx << " " << centery << " " << columns << " " << rows << '\n';

	}
	*/


	return 1;
}

