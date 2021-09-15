#ifndef CONSOLEDISPLAY_H
#define CONSOLEDISPLAY_H

#include "windows.h"
#include <iostream>
#include <sstream>
#include "engineConfig.h"
#include "Vec3.h"
#include <cwchar>
#include <unordered_map>
#include <iterator>



colorRGB cRgbCols[16] = { colorRGB(12,  12,  12),
						  colorRGB(0,   55,  218),
						  colorRGB(19,  161, 14), 
						  colorRGB(58,  150, 221),
						  colorRGB(197, 15,  31), 
						  colorRGB(136, 23,  152), 
						  colorRGB(193, 156, 0), 
						  colorRGB(204, 204, 204), 
						  colorRGB(118, 118, 118),
						  colorRGB(59,  120, 255), 
						  colorRGB(22,  198, 12), 
						  colorRGB(97,  214, 214), 
						  colorRGB(231, 72, 86), 
						  colorRGB(180, 0,  158), 
						  colorRGB(249, 241, 165), 
						  colorRGB(242, 242, 242)};
short iFrontCodes[16] = { 0x0000,
					    0x0001,
					    0x0002,
					    0x0003,
					    0x0004,
					    0x0005,
					    0x0006,
					    0x0007,
					    0x0008,
					    0x0009,
					    0x000A,
					    0x000B,
					    0x000C,
					    0x000D,
					    0x000E,
					    0x000F};
short iBackCodes[16] = { 0x0000,
					   0x0010,
					   0x0020,
					   0x0030,
					   0x0040,
					   0x0050,
					   0x0060,
					   0x0070,
					   0x0080,
					   0x0090,
					   0x00A0,
					   0x00B0,
					   0x00C0,
					   0x00D0,
					   0x00E0,
					   0x00F0};
float fPixSize[4] = { 1., 0.75, 0.5, 0.25 };
short sPixCodes[4] = { 0x2588, 0x2593, 0x2592, 0x2591 };

int color2RGBcode(colorRGB c) 
{
	return ((int)c.x() << 16) + ((int)c.y() << 8) + (int)c.z();
}


enum COLOUR
{
	FG_BLACK		= 0x0000,
	FG_DARK_BLUE    = 0x0001,	
	FG_DARK_GREEN   = 0x0002,
	FG_DARK_CYAN    = 0x0003,
	FG_DARK_RED     = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW  = 0x0006,
	FG_GREY			= 0x0007,
	FG_DARK_GREY    = 0x0008,
	FG_BLUE			= 0x0009,
	FG_GREEN		= 0x000A,
	FG_CYAN			= 0x000B,
	FG_RED			= 0x000C,
	FG_MAGENTA		= 0x000D,
	FG_YELLOW		= 0x000E,
	FG_WHITE		= 0x000F,
	BG_BLACK		= 0x0000,
	BG_DARK_BLUE	= 0x0010,
	BG_DARK_GREEN	= 0x0020,
	BG_DARK_CYAN	= 0x0030,
	BG_DARK_RED		= 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW	= 0x0060,
	BG_GREY			= 0x0070,
	BG_DARK_GREY	= 0x0080,
	BG_BLUE			= 0x0090,
	BG_GREEN		= 0x00A0,
	BG_CYAN			= 0x00B0,
	BG_RED			= 0x00C0,
	BG_MAGENTA		= 0x00D0,
	BG_YELLOW		= 0x00E0,
	BG_WHITE		= 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};


class consolePixel
{
public:
	consolePixel() {};
	consolePixel(short _sCharacter, short _sColorCode): sCharacter(_sCharacter), sColorCode(_sColorCode) {};
	~consolePixel() {};

	colorRGB cColorRGB;
	short sCharacter;
	short sColorCode;
};



class ConsoleScreen
{
public:
	ConsoleScreen() {};
	ConsoleScreen(engineConfig* _conf) : config{ _conf } 
	{
		m_nScreenWidth = config->screenWidth;
		m_nScreenHeight = config->screenHeight;

	};

	~ConsoleScreen() {};

		
	void startUp()
	{
		buildScreen();
		buildColorMap();
		buildRGBbuffer();
	}
	void buildScreen()
	{
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_rectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(hConsole, TRUE, &m_rectWindow);

		COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
		SetConsoleScreenBufferSize(hConsole, coord);
		SetConsoleActiveScreenBuffer(hConsole);

		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = config->pxlSize;
		cfi.dwFontSize.Y = config->pxlSize;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		wcscpy_s(cfi.FaceName, L"Consolas");

		SetCurrentConsoleFontEx(hConsole, false, &cfi);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
			m_nScreenHeight = csbi.dwMaximumWindowSize.Y;
		if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
			m_nScreenWidth = csbi.dwMaximumWindowSize.X;

		m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
		SetConsoleWindowInfo(hConsole, TRUE, &m_rectWindow);

		m_bufScreen = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
		memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);
	}

	void  buildRGBbuffer()
	{
		_bufRGB = new colorRGB[m_nScreenWidth * m_nScreenHeight];

	}

	int bufSize()
	{
		return m_nScreenWidth * m_nScreenHeight;
	}

	colorRGB* bufRGB()
	{
		return _bufRGB;
	}
	
	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
		{
			updateBuffer(y * m_nScreenWidth + x, c, col);
		}
	}

	void updateBuffer(int pos, short c = 0x2588, short col = 0x000F)
	{
		m_bufScreen[pos].Char.UnicodeChar = c;
		m_bufScreen[pos].Attributes = col;
	}
	
	void show()
	{
		WriteConsoleOutput(hConsole, m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
	}

	void updateConsoleBuffer()
	{
		for (int i = 0; i < m_nScreenWidth*m_nScreenHeight; i++) 
		{
			consolePixel closestPix;
			int minDist = 255 * 255 * 3;
			for (auto pallet : uRgb2ConsoleCode)
			{
				int curDist = (_bufRGB[i] - pallet.second.cColorRGB).length_squared();

				if (curDist == 0)
				{
					closestPix = pallet.second;
					break;
				}

				if (curDist < minDist)
				{
					closestPix = pallet.second;
					minDist = curDist;					
				}
			}

			updateBuffer(i, closestPix.sCharacter, closestPix.sColorCode);
			
		}
	}




	void buildColorMap()
	{
		char x;
		for (int i = 0; i< std::size(cRgbCols); i++)
		//for (int i = 0; i < 16; i++)
		{
			colorRGB front = cRgbCols[i];
			short sFrontCode = iFrontCodes[i];

			for (int j = 0; j < std::size(cRgbCols); j++)
			//for (int j = 0; j < 16; j++)
			{
				colorRGB back = cRgbCols[j];
				short sBackCode = iBackCodes[j];

				for (int k = 0; k < std::size(fPixSize); k++)
				//for (int k = 0; k < 4; k++)
				{
					float fCharSize = fPixSize[k];
					short sPixCode = sPixCodes[k];

					colorRGB mixedColor = fCharSize * front + (1 - fCharSize) * back;
					int iColorCode = color2RGBcode(mixedColor);
					consolePixel pix(sPixCode, sFrontCode + sBackCode);
					pix.cColorRGB = mixedColor;
					uRgb2ConsoleCode[iColorCode] = pix;
				}

			}
		}
	}

	void display_colors()
	{
		int i = 0;
		for (std::pair<int, consolePixel> k : uRgb2ConsoleCode)
		{
			
			int col_code = k.first;
			consolePixel pix = k.second;
			Draw(i%m_nScreenWidth, i / m_nScreenWidth, pix.sCharacter, pix.sColorCode);

			//std::cout << std::hex << col_code << "  :  " << pix.sCharacter << pix.sColorCode << '\n';
			i++;
		}		
		show();
	}
	

private:
	engineConfig* config;
	HANDLE hConsole;
	CHAR_INFO* m_bufScreen;
	SMALL_RECT m_rectWindow;

	colorRGB* _bufRGB;



	int m_nScreenWidth = 120;
	int m_nScreenHeight = 90;

	std::unordered_map< int, consolePixel> uRgb2ConsoleCode;

};


#endif
