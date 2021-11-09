#ifndef SOFTWARERENDER_H
#define SOFTWARERENDER_H

#include <iostream>
#include <sstream>
#include "consoledisplay.h"
#include "softwaregl.h"
#include "world.h"

class tSoftwareRenderer
{
public:
	tSoftwareRenderer() {};
	tSoftwareRenderer(int _w, int _h, int _pxlS) :
		screenWidth(_w),
		screenHeight(_h),
		pxlSize(_pxlS)
	{};
	virtual ~tSoftwareRenderer() {};

	int getWidth() { return screenWidth; }
	int getHeight() { return screenHeight; }

	void startUp()
	{
		display = new tConsoleScreen(screenWidth, screenHeight, pxlSize);
		display->startUp();
		screenWidth = display->screenWidth();
		screenHeight = display->screenHeight();
		displayBuffer = display->bufRGB();

		rasterizer = new tSoftwareRasterizer(screenWidth, screenHeight);
		rasterizer->initBuffers();
		rasterizer->setOutBuffer(displayBuffer);
		sGLColor = rasterizer->bufColor();

		m2proj = new tTransformMatrix(m2projData);
	}

	void shutDown()
	{
		delete display, rasterizer, m2proj;
	}

	void setWorldPtr(tScene* _wrld) { world = _wrld; }
	void setCamPtr(tCameraObject* _cam) { cam = _cam; }

	void render()
	{
		rasterizer->sGLCleanBuffers();
		tTransformMatrix* w2proj = cam->getW2Projection();

		
		
		for (auto keyVal : *(world->getObjectsPtr()))
		{
			tMeshObject* obj = keyVal.second;
			tTransformMatrix* transform = obj->getM2Wmatrix();
			std::shared_ptr<tMesh> mesh = obj->getMeshPtr();

			Multiply(*w2proj, *transform, m2proj);
			rasterizer->setModel2ViewMatrix(m2proj->getDataPtr());
			int size = mesh->vertsToBuffer(floatBuf);

			rasterizer->setVertexData(floatBuf, size);
			size = mesh->trisToBuffer(intBuf);
			rasterizer->sGLBufferData(size, intBuf);
			rasterizer->sGLDrawElements(size);
		}
		
		
	}

	void displayResult()
	{
		display->updateConsoleBuffer();
		display->show();
	}

	void setDt(float dt)
	{
		curDt = dt;
	}
private:
	
	tConsoleScreen* display;
	tSoftwareRasterizer* rasterizer;
	int* displayBuffer;
	int screenWidth;
	int screenHeight;
	int pxlSize;
	int* sGLColor;

	int intBuf[1000];
	float floatBuf[1000];

	float m2projData[16];
	tTransformMatrix* m2proj;

	float curDt;

	tScene* world;
	tCameraObject* cam;
};
#endif