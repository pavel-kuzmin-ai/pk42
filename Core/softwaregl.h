#ifndef SOFTWAREGL_H
#define SOFTWAREGL_H

#include "vec3.h"
#include "matrixmath.h"
#include <cmath>
#include <unordered_map>
#include "glcommons.h"
#include "shaders.h"
#include "materials.h"


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


bool zTestAndSet(tPixelData& vrtx, float* zBuf, int i, int j, int width, int maxZ)
{
	int idx = j * width + i;
	bool res = (vrtx.xyz[2] > getBuf(zBuf, i, j, width, 0, 1)) && (vrtx.xyz[2] <= maxZ);
	setBuf(zBuf, vrtx.xyz[2], i, j, width, 0, 1);
	return res;
}



void blendColor(tColorData& c, float* colorBuf, int i, int j, int width)
{
	float alpha = c.rgba[3];
	if (alpha > 0.999)
	{
		setBuf(colorBuf, c.rgba[0], i, j, width, 0, 3);
		setBuf(colorBuf, c.rgba[1], i, j, width, 1, 3);
		setBuf(colorBuf, c.rgba[2], i, j, width, 2, 3);
		return;
	}
	setBuf(colorBuf, (1.f - alpha) * getBuf(colorBuf, i, j, width, 0, 3) + alpha * c.rgba[0], i, j, width, 0, 3);
	setBuf(colorBuf, (1.f - alpha) * getBuf(colorBuf, i, j, width, 1, 3) + alpha * c.rgba[1], i, j, width, 1, 3);
	setBuf(colorBuf, (1.f - alpha) * getBuf(colorBuf, i, j, width, 2, 3) + alpha * c.rgba[2], i, j, width, 2, 3);
}

struct tTmpCalcData
{
	tColorData col;
	tMaterialParam tmpMatPar;
	tPixelData currVrtx;
	tBbox box;
} tmpVars;

void rasterizeTriang(pxlShaderFunc pxlShader, tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2,
	float* outColor, float* zBuf, int width, int height)
{
	
	upscaleCoords(vrtx0, width, height);
	upscaleCoords(vrtx1, width, height);
	upscaleCoords(vrtx2, width, height);
	
	setBbox(vrtx0, vrtx1, vrtx2, tmpVars.box, 0, 0, -1, (float)width, (float)height, 1);


	int bboxMinX = (int)tmpVars.box.xmin;
	int bboxMaxX = (int)tmpVars.box.xmax;
	int bboxMinY = (int)tmpVars.box.ymin;
	int bboxMaxY = (int)tmpVars.box.ymax;
	
	float triangDoubleArea = edgePointDoubleArea(vrtx0, vrtx1, vrtx2);
	//if (triangDoubleArea <= 1e-3) return;
	float invTriangDoubleArea = -1.f;
	if (triangDoubleArea != 0.f) invTriangDoubleArea = 1.f / triangDoubleArea;

	
	
	// using cross to calculate areas of triangles created by edges and test point. All areas + mean point inside triangle
	// key feature - one time cross calculation. after that areas can be changed by addition:
	//(x1 - x0)*(y2 - y0) - (y1 - y0)*(x2 - x0) = y2 * rx1 - y0 * rx1 - x2 * ry1 + x0 * ry1 = (y2prev - y0)*rx1 + rx1 * dy - (x2prev + dx - x0)*ry1 =
	//= (y2prev - y0)*rx1 - (x2prev - x0)*ry1 + rx1 * dy - ry1 * dx = prev_area + rx1 * dy - ry1 * dx
	tmpVars.currVrtx.xyz[0] = bboxMinX + 0.5f;
	tmpVars.currVrtx.xyz[1] = bboxMinY + 0.5f;

	float darea2 = edgePointDoubleArea(vrtx0, vrtx1, tmpVars.currVrtx);
	
	float ry2 = vrtx1.xyz[1] - vrtx0.xyz[1];
	float rx2 = vrtx1.xyz[0] - vrtx0.xyz[0];
	float darea0 = edgePointDoubleArea(vrtx1, vrtx2, tmpVars.currVrtx);
	float ry0 = vrtx2.xyz[1] - vrtx1.xyz[1];
	float rx0 = vrtx2.xyz[0] - vrtx1.xyz[0];

	float darea1 = edgePointDoubleArea(vrtx2, vrtx0, tmpVars.currVrtx);
	float ry1 = vrtx0.xyz[1] - vrtx2.xyz[1];
	float rx1 = vrtx0.xyz[0] - vrtx2.xyz[0];

	for (int i = bboxMinX; i <= bboxMaxX; i++)
	{

		tmpVars.currVrtx.xyz[0] = i + 0.5f;

		float darea1Init = darea1;
		float darea2Init = darea2;
		float darea0Init = darea0;
		
		
		for (int j = bboxMinY; j <= bboxMaxY; j++)
		{
			tmpVars.currVrtx.xyz[1] = j + 0.5f;

			bool inside =( (darea2>0)&& (darea1 > 0)&& (darea0 > 0));

			if (inside)
			{
				float mult2 = darea2 * invTriangDoubleArea;
				float mult0 = darea0 * invTriangDoubleArea;
				float mult1 = 1.0f - mult2 - mult0;

				
				tmpVars.currVrtx.xyz[2] = weightedTriSum(vrtx0.xyz[2], vrtx1.xyz[2], vrtx2.xyz[2], mult0, mult1, mult2);
				
				if (zTestAndSet(tmpVars.currVrtx, zBuf, i, j, width, 1))
				{
					interpolatePixelData(tmpVars.currVrtx, vrtx0, vrtx1, vrtx2, mult0, mult1, mult2);
					pxlShader(tmpVars.currVrtx, &tmpVars.tmpMatPar, tmpVars.col);
					blendColor(tmpVars.col, outColor, i, j, width);
				}
			}
			darea2 += rx2;
			darea0 += rx0;
			darea1 += rx1;
		}

		darea2 = darea2Init;
		darea0 = darea0Init;
		darea1 = darea1Init;

		darea2 -= ry2;
		darea0 -= ry0;
		darea1 -= ry1;
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

	void buildBuf(int width, int height, int channels, fBuffer* fOutBuf)
	{
		fOutBuf->fBuf = new float[width * height * channels];
		fOutBuf->width = width;
		fOutBuf->height = height;
		fOutBuf->channels = channels;
	}

	void initBuffers()
	{
		VertexData = new float[maxVerts];
		VertexBuffer = new int[maxVerts];
		outVertexBuffer = new float[maxVerts];
		
		iOutColor = new int[width*height * 3];
		iOutColorInit = new int[width*height * 3];

		
		fOutColor = new fBuffer;
		fOutColorInit = new fBuffer;
		buildBuf(width, height, 3, fOutColor);
		//screenBuffers["fOutColor"] = fOutColor;
		buildBuf(width, height, 3, fOutColorInit);
		//screenBuffers["fOutColorInit"] = fOutColorInit;
		for (int i = 0; i < width*height*3; i++)
		{
			fOutColor->fBuf[i] = 0.f;
			fOutColorInit->fBuf[i] = 0.f;
		}


		zBuffer = new fBuffer;
		cleanZbuffer = new fBuffer;
		buildBuf(width, height, 1, zBuffer);
		//screenBuffers["zBuffer"] = zBuffer;
		buildBuf(width, height, 1, cleanZbuffer);
		//screenBuffers["cleanZbuffer"] = cleanZbuffer;
		for (int i = 0; i < width*height; i++)
		{
			zBuffer->fBuf[i] = -1.f;
			cleanZbuffer->fBuf[i] = -1.f;
		}

		Model2CamMatrix = new tMatrix(4, 4, Model2Cam);
		Cam2ViewMatrix = new tMatrix(4, 4, Cam2View);

		tmpIntermed = new tMatrix(4, 1, tmpIntermedData);
		tmpOut = new tMatrix(4, 1, tmpOutData);
	}
	void setOutBuffer(int * _buf)
	{
		iOutColor = _buf;
	}

	void setModel2CamMatrix(float* inMatrix)
	{
		std::memcpy(Model2Cam, inMatrix, 16 * sizeof(float));
	}
	
	void setCam2ViewMatrix(float* inMatrix)
	{
		std::memcpy(Cam2View, inMatrix, 16 * sizeof(float));
	}

	void setVertexData(float* inVerts, int size)
	{
		if (size > maxVerts) size = maxVerts;
		std::memcpy(VertexData, inVerts, size * sizeof(float));
	}

	void setMaterial(materialBase* newMaterial) { currentMaterial = newMaterial; }


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
					//int col = (int)(getBuf(screenBuffers["fOutColor"]->fBuf, i, j, width, c, 3) * 255);
					int col = (int)(getBuf(fOutColor->fBuf, i, j, width, c, 3) * 255);
					setBuf(iOutColor, col, i, height - j - 1, width, c, 3);
				}

			}
		}
	}

	void verts2tPixelData(int bufferedIdx, tPixelData* out)
	{
		out->xyz[0] = VertexData[bufferedIdx * 3];
		out->xyz[1] = VertexData[bufferedIdx * 3 + 1];
		out->xyz[2] = VertexData[bufferedIdx * 3 + 2];
	}
	void applyVertexShader(void(*vertexShader)(tPixelData&, tPixelData&, tMatrix*, tMatrix*), tPixelData* inpVrtx, tPixelData* outVrtx)
	{
		vertexShader(*inpVrtx, *outVrtx, Model2CamMatrix, Cam2ViewMatrix);
	}

	void rasterizeTriangle(void(*rasterizer)(pxlShaderFunc, tPixelData&, tPixelData&, tPixelData&, float*, float*, int, int),
		tPixelData& vrtx0, tPixelData& vrtx1, tPixelData& vrtx2)
	{
		//rasterizer(whitePixShader, vrtx0, vrtx1, vrtx2, screenBuffers["fOutColor"]->fBuf, screenBuffers["zBuffer"]->fBuf, width, height);
		//rasterizer(whitePixShader, vrtx0, vrtx1, vrtx2, fOutColor->fBuf, zBuffer->fBuf, width, height);
		rasterizer(currentMaterial->getShader(), vrtx0, vrtx1, vrtx2, fOutColor->fBuf, zBuffer->fBuf, width, height);
	}

	tPixelData pxl0, pxl1, pxl2;
	float dir0[3], dir1[3], dir2[3];
	float faceNormal[3];

	void sGLDrawTriangle(int vertexIdx0, int vertexIdx1, int vertexIdx2)
	{
		//tPixelData pxl0, pxl1, pxl2;
		verts2tPixelData(vertexIdx0, &pxl0);
		pxl0.faceLeader = true;
		verts2tPixelData(vertexIdx1, &pxl1);
		verts2tPixelData(vertexIdx2, &pxl2);

		pxl0.viewDirection = dir0;
		pxl1.viewDirection = dir1;
		pxl2.viewDirection = dir2;


		calcFaceNormals(pxl0, pxl1, pxl2, faceNormal);
		pxl0.faceNormal = faceNormal;
		applyVertexShader(geometryShader, &pxl0, &pxl0);

		bool backFace = dot(pxl0.viewDirection, pxl0.faceNormal) > 0;
		if (backFace) return;
	
		applyVertexShader(geometryShaderCut, &pxl1, &pxl1);
		applyVertexShader(geometryShaderCut, &pxl2, &pxl2);
		pxl1.viewDirection = dir0;
		pxl2.viewDirection = dir0;
		
		
		//std::memcpy(pxl0.normal, pxl0.faceNormal, 3 * sizeof(float));

		//std::memcpy(pxl1.normal, pxl0.normal, 3 * sizeof(float));
		//std::memcpy(pxl2.normal, pxl0.normal, 3 * sizeof(float));
		//std::memcpy(pxl1.viewDirection, pxl0.viewDirection, 3 * sizeof(float));
		//std::memcpy(pxl2.viewDirection, pxl0.viewDirection, 3 * sizeof(float));

		rasterizeTriangle(triangleRasterizer, pxl0, pxl1, pxl2);

	}


	void sGLCleanBuffers()
	{
		//std::memcpy(screenBuffers["fOutColor"]->fBuf , screenBuffers["fOutColorInit"]->fBuf, width*height * 3 * sizeof(float));
		//std::memcpy(screenBuffers["zBuffer"]->fBuf, screenBuffers["cleanZbuffer"]->fBuf, width*height * sizeof(float));
		std::memcpy(fOutColor->fBuf, fOutColorInit->fBuf, width*height * 3 * sizeof(float));
		std::memcpy(zBuffer->fBuf, cleanZbuffer->fBuf, width*height * sizeof(float));
	}
	
	int* bufColor() { return iOutColor; }

private:
	int maxTris = 2048;
	int maxVerts = maxTris * 3;
	float* VertexData;
	
	int* VertexBuffer;
	float* outVertexBuffer;


	int bufTris = 0;
	int bufVerts = 0;

	int width;
	int height;

	//tNamedBuffers screenBuffers;

	fBuffer* fOutColor; 
	int* iOutColor;
	fBuffer* zBuffer;

	int* iOutColorInit;
	fBuffer* fOutColorInit;
	fBuffer* cleanZbuffer;


	float xmin = -1;
	float xmax = 1;
	float xspan = fabs(xmax - xmin);
	float ymin = -1;
	float ymax = 1;
	float yspan = fabs(ymax - ymin);

	float Model2Cam[16];
	tMatrix* Model2CamMatrix;

	float Cam2View[16];
	tMatrix* Cam2ViewMatrix;

	float tmpIntermedData[4] = { 1.f, 1.f, 1.f, 1.f };
	tMatrix* tmpIntermed;

	float tmpOutData[4] = { 1.f, 1.f, 1.f, 1.f };;
	tMatrix* tmpOut;

	// pxlShaderFunc currentShader = normalsPixShader;
	//tMaterialParam* currentMaterialParamList;
	materialBase* currentMaterial;




};






#endif