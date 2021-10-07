#ifndef SOFTWAREGL_H
#define SOFTWAREGL_H

#include "vec3.h"

class tVertexData
{
public:
	tVertexData() {};
	tVertexData(vec3<float> c) :_coords{ c } {};
	tVertexData(float x, float y, float z) :_coords(x,y,z) {};
	~tVertexData() {};

	vec3<float> coords() { return _coords; }
	void translate(float dx, float dy, float dz) { _coords += vec3<float>(dx, dy, dz); }

private:
	vec3<float> _coords;
};



class tSoftwareRasterizer
{
public:
	tSoftwareRasterizer() {};
	tSoftwareRasterizer(int _width = 120, int _height = 90) :width(_width), height( _height ) {};
	~tSoftwareRasterizer() {};

	void initBuffers()
	{
		VertexBuffer = new tVertexData[maxVerts];
		outColor = new int[width*height * 3];
	}
	void setOutBuffer(int * _buf)
	{
		outColor = _buf;
	}

	void sGLBufferData(int size, tVertexData* vertexData)
	{
		if (size > maxVerts) size = maxVerts;
		for (int i = 0; i < size; i++)
		{
			VertexBuffer[i] = vertexData[i];
		}
		bufVerts = size;
	}

	void sGLDrawElements(int maxcount)
	{

		for (int i = 0; i < bufVerts; i++)
		{
			if (i >= maxcount) break;
			sGLDrawVertex(VertexBuffer[i]);
		}
	}

	void sGLDrawVertex(tVertexData vertex)
	{
		float x = ((vertex.coords().x() - xmin) * 2 / xspan - 1);
		float y = ((vertex.coords().y() - ymin) * 2 / yspan - 1);
		if ((x > -1) && (x < 1) && (y > -1) && (y < 1))
		{
			int ix = (int)((x + 1) * width / 2);
			int iy = (int)(height - (y + 1) * height / 2);

			int idx = (int)(iy * width + ix) * 3;
			outColor[idx] = 255;
			outColor[idx + 1] = 255;
			outColor[idx + 2] = 255;
		}
	}

	void sGLFreeOutput()
	{
		for (int i = 0; i < width*height * 3; i++)
			outColor[i] = 0;
	}
	
	int* bufColor() { return outColor; }

private:
	tVertexData* VertexBuffer;
	int maxTris = 1000;
	int maxVerts = maxTris * 3;
	int bufTris = 0;
	int bufVerts = 0;

	int width;
	int height;
	int* outColor;

	//float xmin = -5;
	//float xmax = 15;
	//float xspan = fabs(xmax - xmin);
	//float ymin = -13;
	//float ymax = 2;
	//float yspan = fabs(ymax - ymin);

	float xmin = -1;
	float xmax = 1;
	float xspan = fabs(xmax - xmin);
	float ymin = -1;
	float ymax = 1;
	float yspan = fabs(ymax - ymin);

};




#endif