#pragma once

#ifdef CORE_EXPORTS
#define CORE_API  __declspec(dllexport)   // export DLL information
#else
#define CORE_API  __declspec(dllimport)   // import DLL information
#endif 

#include "clock.h"
#include "consoledisplay.h"
#include "softwaregl.h"


class CORE_API IClock
{
public:
	IClock(float startTimeSeconds = 0.0f);
	~IClock();

	void init();
	void startMeasure();
	void checkAndSwapMeasure();
	float calcDeltaSeconds(IClock other);
	
private:
	tClock* getClk();
	tClock* clk;
};

class CORE_API IConsoleScreen
{
public:
	IConsoleScreen(int _width, int _height, int _pixelSize);
	~IConsoleScreen();
	void startUp();
	int screenWidth();
	int screenHeight();
	int* bufRGB();
	void updateConsoleBuffer();
	void show();
private:
	tConsoleScreen* scr;
};

class CORE_API IVertexData
{
public:
	IVertexData();
	IVertexData(vec3<float> c);
	IVertexData(float x, float y, float z);
	~IVertexData();

	void translate(float dx, float dy, float dz);
	
private:
	tVertexData* getVrtxDataPtr();
	tVertexData* vrtx;
	friend class ISoftwareRasterizer;
};

class CORE_API ISoftwareRasterizer
{
public:
	ISoftwareRasterizer(int _width, int _height);
	~ISoftwareRasterizer();

	void initBuffers();
	void setOutBuffer(int* displayBuf);
	int* bufColor();
	void sGLFreeOutput();
	void sGLBufferData(int size, IVertexData*);
	void sGLDrawElements(int numElem);
private:
	tSoftwareRasterizer* rst;
};


