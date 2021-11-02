#ifndef SOFTWAREGL_H
#define SOFTWAREGL_H

#include "vec3.h"
#include "matrixmath.h"




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

void pixelRasterizer(tInOutVertex& vrtx0, tInOutVertex& vrtx1, tInOutVertex& vrtx2, int* outColor, int width, int height)
{
	rasterizePixel(vrtx0, outColor, width, height);
	rasterizePixel(vrtx1, outColor, width, height);
	rasterizePixel(vrtx2, outColor, width, height);
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

		rasterizeTriangle(pixelRasterizer, out0, out1, out2);
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