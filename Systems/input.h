#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>

class tInputHandler
{
public:
	tInputHandler() 
	{
		hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
	};
	~tInputHandler() {};

	void updateKeyboardState()
	{
		for (int i = 0; i < 256; i++)
		{
			short actualPressed = GetAsyncKeyState(i);
			if (sRawKeyState[i] != actualPressed)
			{
				sRawKeyState[i] = actualPressed;
				if (actualPressed & 0x8000)
				{
					bKeyPressed[i] = true;
				}
				else
				{
					bKeyPressed[i] = false;
				}
			}
		}
	}

	void readConsoleEvents()
	{
		INPUT_RECORD inBuf[32];
		DWORD events = 0;
		GetNumberOfConsoleInputEvents(hConsoleIn, &events);
		if (events > 0)
			ReadConsoleInput(hConsoleIn, inBuf, events, &events);

		for (DWORD i = 0; i < events; i++)
		{
			if (inBuf[i].EventType == FOCUS_EVENT)	{
				bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
			}

			if (bConsoleInFocus)
			{
				if (inBuf[i].EventType == MOUSE_EVENT)
				{
					if (inBuf[i].Event.MouseEvent.dwEventFlags == MOUSE_MOVED)
					{
						iMousePos[0] = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						iMousePos[1] = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
					}

					if (inBuf[i].Event.MouseEvent.dwEventFlags == 0)
					{
						for (int m = 0; m < 5; m++)
						{
							bMousePressed[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
						}
					}
				}
			}
			
		}
	}

	void readInputs()
	{
		readConsoleEvents();
		if (bConsoleInFocus) updateKeyboardState();
	}


	bool* getKeysPtr()
	{
		return bKeyPressed;
	}

	bool* getMousePtr()
	{
		return bMousePressed;
	}

	short* getMouseCoordsPtr()
	{
		return iMousePos;
	}

private:
	HANDLE hConsoleIn;
	short sRawKeyState[256];
	bool bKeyPressed[256];
	short sRawMouseState[5];
	bool bMousePressed[5];
	bool bConsoleInFocus = true;
	short iMousePos[2];
	
};



#endif
