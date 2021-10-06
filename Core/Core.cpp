#include "core.h"


IClock::IClock(float startTimeSeconds) : tClock(startTimeSeconds) {};
IClock::~IClock(void) {};

IConsoleScreen::IConsoleScreen(int _width, int _height, int _pixelSize) : 
	tConsoleScreen(_width, _height, _pixelSize) {};
IConsoleScreen::~IConsoleScreen(void) {};

ISoftwareRasterizer::ISoftwareRasterizer(int _width, int _height) :
	tSoftwareRasterizer(_width, _height) {};
ISoftwareRasterizer::~ISoftwareRasterizer(void) {};

IVertexData::IVertexData() {};
IVertexData::IVertexData(vec3<float> c) : tVertexData(c) {};
IVertexData::IVertexData(float x, float y, float z) : tVertexData(x, y, z) {};
IVertexData::~IVertexData(void) {};