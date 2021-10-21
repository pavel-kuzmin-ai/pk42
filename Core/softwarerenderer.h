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
	}

	void setWorldPtr(tScene* _wrld) { world = _wrld; }
	void setCamPtr(tCameraObject* _cam) { cam = _cam; }

	void render()
	{
		rasterizer->sGLFreeOutput();
		tTransformMatrix* w2proj = cam->getW2Projection();

		float m2projData[16];
		tTransformMatrix m2proj(m2projData);
		for (auto keyVal : *(world->getObjectsPtr()))
		{
			tMeshObject* obj = keyVal.second;
			tTransformMatrix* transform = obj->getM2Wmatrix();
			std::shared_ptr<tMesh> mesh = obj->getMeshPtr();

			Multiply(*w2proj, *transform, &m2proj);
			rasterizer->setModel2ViewMatrix(m2proj.getDataPtr());
			int size = mesh->vertsToBuffer(floatBuf);

			rasterizer->setVertexData(floatBuf, size);
			size = mesh->trisToBuffer(intBuf);
			rasterizer->sGLBufferData(size, intBuf);
			rasterizer->sGLDrawElements(size);
		}
		
		
	}


	/*void updateWorld()
	{
		world[0].translate(0.001f, 0.f, 0.f);
		world[1].translate(0.f, 0.001f, 0.f);
		world[2].translate(0.001f, 0.001f, 0.f);
	}*/
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


	float curDt;

	tScene* world;
	tCameraObject* cam;
};
#endif