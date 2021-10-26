#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>

class tInputHandler
{
public:
	tInputHandler() 
	{
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

	bool* getKeysPtr()
	{
		return bKeyPressed;
	}

private:
	HANDLE hConsoleIn;
	short sRawKeyState[256];
	bool bKeyPressed[256];
	bool bConsoleInFocus = true;
	
};



#endif
