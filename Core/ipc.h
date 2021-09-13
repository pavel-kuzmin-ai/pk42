#ifndef IPC_H
#define IPC_H

#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include "msgBus.h"
#include <iostream>
#include <queue>
#include <conio.h>
#include <sstream>


using std::string;
using std::queue;
using std::stringstream;

#define BUF_SIZE 256
TCHAR fromMainName[] = TEXT("Global\FromMain");
TCHAR fromChildName[] = TEXT("Global\FromChild");

HANDLE ghWriteFromMainEventDone = CreateEvent(NULL,               // default security attributes
	TRUE,               // manual-reset event
	FALSE,              // initial state is nonsignaled
	TEXT("WriteFromMainEventDone")  // object name
);

HANDLE ghReadFromMainEventDone = CreateEvent(NULL,               // default security attributes
	TRUE,               // manual-reset event
	TRUE,              // initial state is signaled
	TEXT("ReadFromMainEventDone")  // object name
);

HANDLE ghWriteFromChildEventDone = CreateEvent(NULL,               // default security attributes
	TRUE,               // manual-reset event
	FALSE,              // initial state is nonsignaled
	TEXT("WriteFromChildEventDone")  // object name
);

HANDLE ghReadFromChildEventDone = CreateEvent(NULL,               // default security attributes
	TRUE,               // manual-reset event
	TRUE,              // initial state is signaled
	TEXT("ReadFromChildEventDone")  // object name
);


void createFileMapBuf(TCHAR* szName, LPTSTR& pBuf, HANDLE& hMapFile)
{
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);



	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);
	}
}

void putToShared(TCHAR* szName, messageQueue& szMsg, LPTSTR& pBuf, HANDLE& hMapFile)
{
	string to_save = szMsg.str();
	TCHAR *tCharBuf = new TCHAR[to_save.size() + 1];
	const char *cstr = to_save.c_str();
	for (int i = 0; i < to_save.size() + 1; i++) 
	{
		tCharBuf[i] = (TCHAR)cstr[i];
	}
	CopyMemory((PVOID)pBuf, tCharBuf, (_tcslen(tCharBuf) * sizeof(TCHAR)));
}

void closeFileMap(LPTSTR& pBuf, HANDLE& hMapFile)
{
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}


void openFileMapBuf(TCHAR* szName, LPTSTR& pBuf, HANDLE& hMapFile)
{
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);


	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(hMapFile);
	}

}

messageQueue getFromShared(TCHAR* szName, LPTSTR& pBuf, HANDLE& hMapFile)
{
	string s;
	for (int i = 0; i < _tcslen(pBuf); i++)
	{
		s.push_back((char)pBuf[i]);
	}
	messageQueue q(s);
	return q;
}

/*
class sysMmapSaverFromChild : public Node
{
public:
	sysMmapSaverFromChild() {};
	sysMmapSaverFromChild(string s) :Node(s) {};
	virtual ~sysMmapSaverFromChild() {};



	void callSystem()
	{
		queue<string> q = getFromShared();

		while (!q.empty())
		{
			qOutputMsgQueue.push(q.front());
			q.pop();
		}

	}
};
*/


class sysMmapLoaderFromChild : public Node
{
public:
	sysMmapLoaderFromChild() {};
	sysMmapLoaderFromChild(string s) :Node(s) {};
	virtual ~sysMmapLoaderFromChild() { closeFileMap(pBuf, hMapFile); };

	void startUp() { createFileMapBuf(szName, pBuf, hMapFile); }
	void shutDown() 
	{ 
		ResetEvent(ghWriteFromChildEventDone); 
		ResetEvent(ghReadFromChildEventDone);
		closeFileMap(pBuf, hMapFile); 
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{

		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(ghWriteFromChildEventDone, // event handle
			10);    // indefinite wait
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			messageQueue q = getFromShared(szName, pBuf, hMapFile);

			while (!q.empty())
			{
				message mycmd = q.getMsgAndPop();
				qOut->push(mycmd);
			}

			ResetEvent(ghWriteFromChildEventDone);
			SetEvent(ghReadFromChildEventDone);

		}
		
		
		return 0;
	}

protected:
	LPTSTR pBuf;
	HANDLE hMapFile;
	TCHAR* szName = fromChildName;
};




class sysMmapSaverFromMain : public Node
{
public:
	sysMmapSaverFromMain() {};
	sysMmapSaverFromMain(string s) :Node(s) {};
	virtual ~sysMmapSaverFromMain() {};

	void startUp() { createFileMapBuf(szName, pBuf, hMapFile); }
	void shutDown() 
	{ 
		ResetEvent(ghReadFromMainEventDone);
		ResetEvent(ghWriteFromMainEventDone);
		closeFileMap(pBuf, hMapFile); 
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		if (!qIn->empty())
		
		{
			DWORD dwWaitResult;
			dwWaitResult = WaitForSingleObject(ghReadFromMainEventDone, // event handle
				10);    // indefinite wait
			
			if (dwWaitResult == WAIT_OBJECT_0)
			{
				putToShared(szName, *qIn, pBuf, hMapFile);
				ResetEvent(ghReadFromMainEventDone);
				SetEvent(ghWriteFromMainEventDone);
			}
		}

		return 0;
	}

	

protected:
	LPTSTR pBuf;
	HANDLE hMapFile;
	TCHAR* szName = fromMainName;
};

class sysMmapLoaderFromMain : public Node
{
public:
	sysMmapLoaderFromMain() {};
	sysMmapLoaderFromMain(string s) :Node(s) {};
	virtual ~sysMmapLoaderFromMain() {};

	void startUp() { openFileMapBuf(szName, pBuf, hMapFile); }
	void shutDown() 
	{ 
		ResetEvent(ghWriteFromMainEventDone);
		ResetEvent(ghReadFromMainEventDone);
		closeFileMap(pBuf, hMapFile); 
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{

		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(ghWriteFromMainEventDone, // event handle
										   10);    // indefinite wait
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			
			messageQueue q = getFromShared(szName, pBuf, hMapFile);
			while (!q.empty())
			{
				message mycmd = q.getMsgAndPop();
				qOut->push(mycmd);
			}
			ResetEvent(ghWriteFromMainEventDone);
			SetEvent(ghReadFromMainEventDone);
		}

		return 0;
	}


protected:
	LPTSTR pBuf;
	HANDLE hMapFile;
	TCHAR* szName = fromMainName;
};

class mmapSaverFromChild
{
public:
	mmapSaverFromChild() { };
	virtual ~mmapSaverFromChild() { };

	void startUp() { openFileMapBuf(szName, pBuf, hMapFile); }
	void shutDown()
	{
		ResetEvent(ghReadFromChildEventDone);
		ResetEvent(ghWriteFromChildEventDone);
		closeFileMap(pBuf, hMapFile);
	}

	void saveAndEraseQueue(messageQueue& q)
	{
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(ghReadFromChildEventDone, // event handle
			10);    // indefinite wait

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			putToShared(szName, q, pBuf, hMapFile);
			
			while (!q.empty())
			{
				q.pop();
			}
			ResetEvent(ghReadFromChildEventDone);
			SetEvent(ghWriteFromChildEventDone);
		}
	}



private:
	LPTSTR pBuf;
	HANDLE hMapFile;
	TCHAR* szName = fromChildName;

};


void CreateChildProcess()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//SECURITY_ATTRIBUTES sa;
	

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	//ZeroMemory(&sa, sizeof(sa));
	//sa.bInheritHandle = true;

	//if (argc != 2)
	//{
	//	printf("Usage: %s [cmdline]\n", argv[0]);
	//	return;
	//}
	TCHAR szCmdline[] = TEXT("pkEng.exe");
	// Start the child process. 
	/*if (!CreateProcess(NULL,   // No module name (use command line)
	//	argv[1],        // Command line
		szCmdline,
		&sa,           // Process handle not inheritable
		&sa,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)*/

	if (!CreateProcess(NULL,   // No module name (use command line)
	//	argv[1],        // Command line
		szCmdline,
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)


	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
#endif