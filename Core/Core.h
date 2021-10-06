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



class DLLDIR IClock : public tClock
{
public:
	IClock(float startTimeSeconds = 0.0f);
	~IClock();
};

class DLLDIR IConsoleScreen : public tConsoleScreen
{
public:
	IConsoleScreen(int _width, int _height, int _pixelSize);
	~IConsoleScreen();
};

class DLLDIR ISoftwareRasterizer : public tSoftwareRasterizer
{
public:
	ISoftwareRasterizer(int _width, int _height);
	~ISoftwareRasterizer();
};

class DLLDIR IVertexData : public tVertexData
{
public:
	IVertexData();
	IVertexData(vec3<float> c);
	IVertexData(float x, float y, float z);
	~IVertexData();
};
