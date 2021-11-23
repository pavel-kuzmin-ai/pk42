#ifndef SOFTWAREGL_H
#define SOFTWAREGL_H

#include "vec3.h"
#include "matrixmath.h"
#include <cmath>



struct tInOutVertex
{
	float x[3];
};

struct tIntInOutVertex
{
	int x[3];
};

struct tPixelData
{
	float xyz[3];
	float uv[2];
	float normal[3];
};

struct tColorData
{
	float rgba[4];
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

typedef void(*pxlShaderFunc)(tPixelData& currVrtx, tPixelData& _vrtx0, tPixelData& _vrtx1, tPixelData& _vrtx2, tColorData& out);

void whitePixShader(tPixelData& currVrtx, tPixelData& _vrtx0, tPixelData& _vrtx1, tPixelData& _vrtx2, tColorData& out)
{
	out.rgba[0] = 1.f;
	out.rgba[1] = 1.f;
	out.rgba[2] = 1.f;
	out.rgba[3] = 1.f;
}

void rasterizePixel(tPixelData& vrtx, float* outColor, int width, int height)
{
	float x = vrtx.xyz[0];
	float y = vrtx.xyz[1];
	float z = vrtx.xyz[2];
	if ((x > -1) && (x < 1) && (y > -1) && (y < 1) && (z > -1) && (z < 1))
	{
		int ix = (int)((x + 1) * width / 2);
		int iy = (int)(height - (y + 1) * height / 2);
		int idx = (int)(iy * width + ix) * 3;
		outColor[idx] = 1.f;
		outColor[idx + 1] = 1.f;
		outColor[idx + 2] = 1.f;
	}
}

inline int sign(float x)
{
	if (x >= 0) return 1;
	return -1;
	
}

void rasterizeEdge(tPixelData& vrtx0, tPixelData& vrtx1, float* outColor, int width, int height)
{
	//Naive implementation. Better use Bresenham's algorithm here
	float xstep = 2.f / (float)width;
	float ystep = 2.f / (float)height;

	float dx = vrtx1.xyz[0] - vrtx0.xyz[0];
	float dy = vrtx1.xyz[1] - vrtx0.xyz[1];
	float dz = vrtx1.xyz[2] - vrtx0.xyz[2];
	float xsteps = std::abs(dx) / xstep;
	float ysteps = std::abs(dy) / ystep;

	int argIdx = 0;
	int funcIdx = 1;
	float tan = 0;
	float step = xstep;
	
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
		step = ystep;
	}

	float dist = sqrt(dx * dx + dy * dy);
	float zTan = 0; 
	if (dist > step) zTan = dz / dist;
	


	tPixelData currVrtx;
	std::memcpy(currVrtx.xyz, vrtx0.xyz, 3 * sizeof(float));
	float delta = 0.f;
	float delta2 = 0.f;
	while ((vrtx1.xyz[argIdx]- currVrtx.xyz[argIdx])/ (stepSign * step) >= 1.)
	{ 
		delta2 = delta * tan;
		currVrtx.xyz[funcIdx] = delta2 + vrtx0.xyz[funcIdx];
		currVrtx.xyz[2] = sqrt(delta*delta + delta2*delta2) * zTan + vrtx0.xyz[2];

		rasterizePixel(currVrtx, outColor, width, height);

		currVrtx.xyz[argIdx] += stepSign * step;
		delta += stepSign * step;
	}
}

struct tBbox
{
	float xmin, xmax, ymin, ymax, zmin, zmax;
};


float edgePointDoubleArea(tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& testVrtx)
{
	return (vrtx1.xyz[0] - vrtx0.xyz[0]) * (testVrtx.xyz[1] - vrtx0.xyz[1]) - (vrtx1.xyz[1] - vrtx0.xyz[1]) * (testVrtx.xyz[0] - vrtx0.xyz[0]);
}

void setBbox(tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2, tBbox& box, float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	box.xmin = std::fmaxf(std::fminf(vrtx0.xyz[0], std::fminf(vrtx1.xyz[0], vrtx2.xyz[0])), minX);
	box.xmax = std::fminf(std::fmaxf(vrtx0.xyz[0], std::fmaxf(vrtx1.xyz[0], vrtx2.xyz[0])), maxX);

	box.ymin = std::fmaxf(std::fminf(vrtx0.xyz[1], std::fminf(vrtx1.xyz[1], vrtx2.xyz[1])), minY);
	box.ymax = std::fminf(std::fmaxf(vrtx0.xyz[1], std::fmaxf(vrtx1.xyz[1], vrtx2.xyz[1])), maxY);

	box.zmin = std::fmaxf(std::fminf(vrtx0.xyz[2], std::fminf(vrtx1.xyz[2], vrtx2.xyz[2])), minZ);
	box.zmax = std::fminf(std::fmaxf(vrtx0.xyz[2], std::fmaxf(vrtx1.xyz[2], vrtx2.xyz[2])), maxZ);
}

void upscaleCoords(tPixelData& _vrtx, int width, int height)
{
	_vrtx.xyz[0] = (_vrtx.xyz[0] + 1.f)*0.5f*width;
	_vrtx.xyz[1] = (_vrtx.xyz[1] + 1.f)*0.5f*height;
}

void setBuf(int* buf, int val, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	buf[idx+ channelIdx] = val;
}

void setBuf(float* buf, float val, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	buf[idx + channelIdx] = val;
}

int getBuf(int* buf, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	return buf[idx + channelIdx];
}

float getBuf(float* buf, int ix, int iy, int width, int channelIdx, int totalChan)
{
	int idx = (iy * width + ix) * totalChan;
	return buf[idx + channelIdx];
}

bool zTestAndSet(tPixelData& vrtx, float* zBuf, int i, int j, int width, int maxZ)
{
	int idx = j * width + i;
	bool res = (vrtx.xyz[2] > getBuf(zBuf, i, j, width, 0, 1)) && (vrtx.xyz[2] <= maxZ);
	setBuf(zBuf, vrtx.xyz[2], i, j, width, 0, 1);
	return res;
}
inline float weightedTriSum(float x, float y, float z, float wx, float wy, float wz)
{
	return x * wx + y * wy + z * wz;
}
void interpolatePixelData(tPixelData& res, tPixelData& pxl0, tPixelData& pxl1, tPixelData& pxl2, float &lambda0, float &lambda1, float &lambda2)
{
	res.xyz[2] = weightedTriSum(pxl0.xyz[2], pxl1.xyz[2], pxl2.xyz[2], lambda0, lambda1, lambda2);
	
	for (int i = 0; i < 2; i++)
	{
		res.uv[i] = weightedTriSum(pxl0.uv[i], pxl1.uv[i], pxl2.uv[i], lambda0, lambda1, lambda2);
	}

	for (int i = 0; i < 3; i++)
	{
		res.normal[i] = weightedTriSum(pxl0.normal[i], pxl1.normal[i], pxl2.normal[i], lambda0, lambda1, lambda2);
	}
}

void blendColor(tColorData& c, float* colorBuf, int i, int j, int width)
{
	float alpha = c.rgba[3];
	setBuf(colorBuf, (1.f - alpha) * getBuf(colorBuf, i, j, width, 0, 3) + alpha * c.rgba[0], i, j, width, 0, 3);
	setBuf(colorBuf, (1.f - alpha) * getBuf(colorBuf, i, j, width, 1, 3) + alpha * c.rgba[1], i, j, width, 1, 3);
	setBuf(colorBuf, (1.f - alpha) * getBuf(colorBuf, i, j, width, 2, 3) + alpha * c.rgba[2], i, j, width, 2, 3);
}

void rasterizeTriang(pxlShaderFunc pxlShader, tPixelData& _vrtx0, tPixelData& _vrtx1, tPixelData& _vrtx2,
	float* outColor, float* zBuf, int width, int height)
{
	tBbox box;
	tPixelData vrtx0, vrtx1, vrtx2;
	std::memcpy(vrtx0.xyz, _vrtx0.xyz, 3 * sizeof(float));
	std::memcpy(vrtx1.xyz, _vrtx1.xyz, 3 * sizeof(float));
	std::memcpy(vrtx2.xyz, _vrtx2.xyz, 3 * sizeof(float));

	upscaleCoords(vrtx0, width, height);
	upscaleCoords(vrtx1, width, height);
	upscaleCoords(vrtx2, width, height);
	
	setBbox(vrtx0, vrtx1, vrtx2, box, 0, 0, -1, (float)width, (float)height, 1);

	tPixelData currVrtx;
	bool bEscapedTriangle = false;

	int bboxMinX = (int)box.xmin;
	int bboxMaxX = (int)box.xmax;
	int bboxMinY = (int)box.ymin;
	int bboxMaxY = (int)box.ymax;
	float invTriangDoubleArea = -1.f;
	for (int i = bboxMinX; i <= bboxMaxX; i++)
	{
		currVrtx.xyz[0] = i + 0.5f;
		for (int j = bboxMinY; j <= bboxMaxY; j++)
		{
			currVrtx.xyz[1] = j + 0.5f;

			float mult2 = edgePointDoubleArea(vrtx0, vrtx1, currVrtx);
			float mult0 = edgePointDoubleArea(vrtx1, vrtx2, currVrtx);
			float mult1 = edgePointDoubleArea(vrtx2, vrtx0, currVrtx);
			if ((mult0 > 0) && (mult1 > 0) && (mult2 > 0))
			{
				if (invTriangDoubleArea<0.f) invTriangDoubleArea = 1.f/(mult0 + mult1 + mult2);
				mult0 *= invTriangDoubleArea;
				mult1 *= invTriangDoubleArea;
				mult2 *= invTriangDoubleArea;
				currVrtx.xyz[2] = vrtx2.xyz[2] * mult2 + vrtx0.xyz[2] * mult0 + vrtx1.xyz[2] * mult1;
				interpolatePixelData(currVrtx, vrtx0, vrtx1, vrtx2, mult0, mult1, mult2);
				if (zTestAndSet(currVrtx, zBuf, i, j, width, 1))
				{
					tColorData col;
					pxlShader(currVrtx, vrtx0, vrtx1, vrtx2, col);
					blendColor(col, outColor, i, j, width);
				}
			}
		}
	}
}

void pixelRasterizer(tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2, float* outColor, int width, int height)
{
	rasterizePixel(vrtx0, outColor, width, height);
	rasterizePixel(vrtx1, outColor, width, height);
	rasterizePixel(vrtx2, outColor, width, height);
}

void edgeRasterizer(pxlShaderFunc pxlShader, tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2, float* outColor, float* zBuf, int width, int height)
{
	rasterizeEdge(vrtx0, vrtx1, outColor, width, height);
	rasterizeEdge(vrtx1, vrtx2, outColor, width, height);
	rasterizeEdge(vrtx2, vrtx0, outColor, width, height);
}

void triangleRasterizer(pxlShaderFunc pxlShader, tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2, float* outColor, float* zBuf, int width, int height)
{
	rasterizeTriang(pxlShader, vrtx0, vrtx1, vrtx2, outColor, zBuf, width, height);
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
		
		iOutColor = new int[width*height * 3];
		iOutColorInit = new int[width*height * 3];

		fOutColor = new float[width*height * 3];
		fOutColorInit = new float[width*height * 3];
		
		zBuffer = new float[width*height];
		cleanZbuffer = new float[width*height];
		for (int i = 0; i < width*height; i++)
		{
			zBuffer[i] = -1;
			cleanZbuffer[i] = -1;
		}
		transform = new tMatrix(4, 4, Model2View);

		tmpIntermed = new tMatrix(4, 1, tmpIntermedData);
		tmpOut = new tMatrix(4, 1, tmpOutData);
	}
	void setOutBuffer(int * _buf)
	{
		iOutColor = _buf;
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
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				for (int c = 0; c < 3; c++)
				{
					int col = (int)(getBuf(fOutColor, i, j, width, c, 3) * 255);
					setBuf(iOutColor, col, i, height - j, width, c, 3);
				}

			}
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

	void rasterizeTriangle(void(*rasterizer)(pxlShaderFunc, tPixelData&, tPixelData&, tPixelData&, float*, float*, int, int),
		tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2)
	{
		rasterizer(whitePixShader, vrtx0, vrtx1, vrtx2, fOutColor, zBuffer, width, height);
	}

	void sGLDrawTriangle(int vertexIdx0, int vertexIdx1, int vertexIdx2)
	{
		tInOutVertex out0 = applyVertexShader(geometryShader, vertexIdx0);
		tInOutVertex out1 = applyVertexShader(geometryShader, vertexIdx1);
		tInOutVertex out2 = applyVertexShader(geometryShader, vertexIdx2);

		tPixelData pxl0, pxl1, pxl2;
		std::memcpy(pxl0.xyz, out0.x, 3 * sizeof(float));
		std::memcpy(pxl1.xyz, out1.x, 3 * sizeof(float));
		std::memcpy(pxl2.xyz, out2.x, 3 * sizeof(float));

		//rasterizeTriangle(pixelRasterizer, out0, out1, out2);
		//rasterizeTriangle(edgeRasterizer, out0, out1, out2);
		rasterizeTriangle(triangleRasterizer, pxl0, pxl1, pxl2);
	}


	void sGLCleanBuffers()
	{
		//std::memcpy(iOutColor, iOutColorInit, width*height * 3 * sizeof(int)); 
		std::memcpy(fOutColor, fOutColorInit, width*height * 3 * sizeof(float));
		std::memcpy(zBuffer, cleanZbuffer, width*height * sizeof(float));
	}
	
	int* bufColor() { return iOutColor; }

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

	float* fOutColor; 
	int* iOutColor;
	float* zBuffer;

	int* iOutColorInit;
	float* fOutColorInit;
	float* cleanZbuffer;


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