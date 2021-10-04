#pragma once

#ifdef DLLDIR_EX
#define DLLDIR  __declspec(dllexport)   // export DLL information
#else
#define DLLDIR  __declspec(dllimport)   // import DLL information
#endif 

#include <iostream>
#include <queue>
#include <string>
#include "clock.h"
#include "consoledisplay.h"
#include "softwaregl.h"


extern "C" DLLDIR tClock* APIENTRY GetClock(float strtT);
extern "C" DLLDIR tConsoleScreen* APIENTRY GetConsoleScreen(int _width, int _height, int _pixelSize);
extern "C" DLLDIR tSoftwareRasterizer* APIENTRY GetSoftwareRasterizer(int _width, int _height);