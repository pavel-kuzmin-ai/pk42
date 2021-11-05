#ifndef SOFTWAREGL_H
#define SOFTWAREGL_H

#include "vec3.h"
#include "matrixmath.h"
#include <cmath>


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

struct tInOutVertex
{
	float x[3];
};


float tmpIntermediateData[4] = {1., 1., 1., 1.};
tMatrix* tmpIntermediate = new tMatrix(4, 1, tmpIntermediateData);

float tmpOutputData[4] = { 1., 1., 1., 1. };
tMatrix* tmpOutput = new tMatrix(4, 1, tmpOutputData);

tInOutVertex geometryShader(tInOutVertex& v, tMatrix* M)
{
	tInOutVertex out;
	arr2Matrix(v.x, *tmpIntermediate, 3, 0);
	Multiply(*M, *tmpIntermediate, tmpOutput);

	out.x[0] = tmpOutput->getValue(0, 0);
	out.x[1] = tmpOutput->getValue(1, 0);
	out.x[2] = tmpOutput->getValue(2, 0);

	return out;
}

tInOutVertex whitePixShader(tInOutVertex& v)
{
	tInOutVertex out;
	out.x[0] = 255;
	out.x[1] = 255;
	out.x[2] = 255;

	return out;
}

void rasterizePixel(tInOutVertex& vrtx, int* outColor, int width, int height)
{
	float x = vrtx.x[0];
	float y = vrtx.x[1];
	float z = vrtx.x[2];
	if ((x > -1) && (x < 1) && (y > -1) && (y < 1) && (z > -1) && (z < 1))
	{
		int ix = (int)((x + 1) * width / 2);
		int iy = (int)(height - (y + 1) * height / 2);
		int idx = (int)(iy * width + ix) * 3;
		outColor[idx] = 255;
		outColor[idx + 1] = 255;
		outColor[idx + 2] = 255;
	}
}

inline int sign(float x)
{
	if (x >= 0) return 1;
	return -1;
	
}

void rasterizeEdge(tInOutVertex& vrtx0, tInOutVertex& vrtx1, int* outColor, int width, int height)
{
	float step = std::fmin(2.f / (float)width, 2.f / (float)height);

	float dx = vrtx1.x[0] - vrtx0.x[0];
	float dy = vrtx1.x[1] - vrtx0.x[1];
	float dz = vrtx1.x[2] - vrtx0.x[2];
	float xsteps = std::abs(dx) / step;
	float ysteps = std::abs(dy) / step;

	int argIdx = 0;
	int funcIdx = 1;
	float tan = 0;
	
	int stepSign = sign(dx);
	if (xsteps > ysteps)
	{
		tan = dy / dx;
	}
	else if (xsteps < ysteps)
	{
		argIdx = 1;
		funcIdx = 0;
		tan = dx / dy;
		stepSign = sign(dy);
	}

	float dist = sqrt(dx * dx + dy * dy);
	float zTan = 0; 
	if (dist > step) zTan = dz / dist;
	


	tInOutVertex currVrtx;
	std::memcpy(currVrtx.x, vrtx0.x, 3 * sizeof(float));
	float delta = 0.f;
	float delta2 = 0.f;
	while ((vrtx1.x[argIdx]- currVrtx.x[argIdx])/ (stepSign * step) >= 1.)
	{ 
		delta2 = delta * tan;
		currVrtx.x[funcIdx] = delta2 + vrtx0.x[funcIdx];
		currVrtx.x[2] = sqrt(delta*delta + delta2*delta2) * zTan + vrtx0.x[2];

		rasterizePixel(currVrtx, outColor, width, height);

		currVrtx.x[argIdx] += stepSign * step;
		delta += stepSign * step;
	}
}

void pixelRasterizer(tInOutVertex& vrtx0, tInOutVertex& vrtx1, tInOutVertex& vrtx2, int* outColor, int width, int height)
{
	rasterizePixel(vrtx0, outColor, width, height);
	rasterizePixel(vrtx1, outColor, width, height);
	rasterizePixel(vrtx2, outColor, width, height);
}

void edgeRasterizer(tInOutVertex& vrtx0, tInOutVertex& vrtx1, tInOutVertex& vrtx2, int* outColor, int width, int height)
{
	rasterizeEdge(vrtx0, vrtx1, outColor, width, height);
	rasterizeEdge(vrtx1, vrtx2, outColor, width, height);
	rasterizeEdge(vrtx2, vrtx0, outColor, width, height);
}

class tSoftwareRasterizer
{
public:
	tSoftwareRasterizer() {};
	tSoftwareRasterizer(int _width = 120, int _height = 90) :width(_width), height( _height ) {};
	~tSoftwareRasterizer() {};

	void initBuffers()
	{
		VertexData = new float[maxVerts];
		VertexBuffer = new int[maxVerts];
		outVertexBuffer = new float[maxVerts];
		outColor = new int[width*height * 3];
		transform = new tMatrix(4, 4, Model2View);

		tmpIntermed = new tMatrix(4, 1, tmpIntermedData);
		tmpOut = new tMatrix(4, 1, tmpOutData);
	}
	void setOutBuffer(int * _buf)
	{
		outColor = _buf;
	}

	void setModel2ViewMatrix(float* inMatrix)
	{
		std::memcpy(Model2View, inMatrix, 16 * sizeof(float));
	}

	void setVertexData(float* inVerts, int size)
	{
		if (size > maxVerts) size = maxVerts;
		std::memcpy(VertexData, inVerts, size * sizeof(float));
		
	}


	void sGLBufferData(int size, int* vertexIds)
	{
		if (size > maxVerts) size = maxVerts;
		std::memcpy(VertexBuffer, vertexIds, size * sizeof(int));
		bufVerts = size;
		bufTris = size / 3;
	}

	void sGLDrawElements(int maxcount)
	{
		for (int i = 0; i < bufTris; i++)
		{
			if (i >= maxcount) break;
			sGLDrawTriangle(VertexBuffer[i * 3], VertexBuffer[i * 3 + 1], VertexBuffer[i * 3 + 2]);
		}
	}

	tInOutVertex applyVertexShader(tInOutVertex(*vertexShader)(tInOutVertex&, tMatrix*), int bufferedIdx)
	{
		tInOutVertex shadedVrtx;
		shadedVrtx.x[0] = VertexData[bufferedIdx * 3];
		shadedVrtx.x[1] = VertexData[bufferedIdx * 3 + 1];
		shadedVrtx.x[2] = VertexData[bufferedIdx * 3 + 2];

		shadedVrtx = vertexShader(shadedVrtx, transform);
		return shadedVrtx;
	}

	void rasterizeTriangle(void(*rasterizer)(tInOutVertex&, tInOutVertex&, tInOutVertex&, int*, int, int), 
				   tInOutVertex& vrtx0, tInOutVertex& vrtx1, tInOutVertex& vrtx2)
	{
		rasterizer(vrtx0, vrtx1, vrtx2, outColor, width, height);
	}

	void sGLDrawTriangle(int vertexIdx0, int vertexIdx1, int vertexIdx2)
	{
		tInOutVertex out0 = applyVertexShader(geometryShader, vertexIdx0);
		tInOutVertex out1 = applyVertexShader(geometryShader, vertexIdx1);
		tInOutVertex out2 = applyVertexShader(geometryShader, vertexIdx2);

		//rasterizeTriangle(pixelRasterizer, out0, out1, out2);
		rasterizeTriangle(edgeRasterizer, out0, out1, out2);
	}


	void sGLFreeOutput()
	{
		for (int i = 0; i < width*height * 3; i++)
			outColor[i] = 0;
	}
	
	int* bufColor() { return outColor; }

private:
	int maxTris = 1000;
	int maxVerts = maxTris * 3;
	float* VertexData;
	
	int* VertexBuffer;
	float* outVertexBuffer;


	int bufTris = 0;
	int bufVerts = 0;

	int width;
	int height;
	int* outColor;


	float xmin = -1;
	float xmax = 1;
	float xspan = fabs(xmax - xmin);
	float ymin = -1;
	float ymax = 1;
	float yspan = fabs(ymax - ymin);

	float Model2View[16];
	tMatrix* transform;

	float tmpIntermedData[4] = { 1.f, 1.f, 1.f, 1.f };
	tMatrix* tmpIntermed;

	float tmpOutData[4] = { 1.f, 1.f, 1.f, 1.f };;
	tMatrix* tmpOut;




};




#endif