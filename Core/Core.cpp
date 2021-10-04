#include "core.h"

tClock* GetClock(float strtT = 0.0f)
{
	return &tClock(strtT);
}


tConsoleScreen* GetConsoleScreen(int _width = 120, int _height = 80, int _pixelSize = 8)
{
	return &tConsoleScreen(_width, _height, _pixelSize);
}


tSoftwareRasterizer* GetSoftwareRasterizer(int _width = 120, int _height = 80)
{
	return &tSoftwareRasterizer(_width, _height);
}


vertexData::vertexData(){}
vertexData::vertexData(vec3<float> c) : tVertexData(c) {}
vertexData::vertexData(float x, float y, float z) : tVertexData(x, y, z) {}
vertexData::~vertexData(void) {}