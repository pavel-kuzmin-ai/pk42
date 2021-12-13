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
		m2c = new tTransformMatrix(m2cData);
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
		//tTransformMatrix* w2proj = cam->getW2Projection();
		tTransformMatrix* cam2proj = cam->getM2Projection();
		tTransformMatrix* world2cam = cam->getW2Mmatrix();
		
		std::shared_ptr<tMesh> mesh;
		//tMesh* mesh;
		for (auto keyVal : *(world->getObjectsPtr()))
		{
			tMeshObject* obj = keyVal.second;
			if (!obj->bIsVisible()) continue;
			//tTransformMatrix* transform = obj->getM2Wmatrix();
			tTransformMatrix* model2world = obj->getM2Wmatrix();
			mesh = obj->getMeshPtr();

			//Multiply(*w2proj, *transform, m2proj);
			Multiply(*world2cam, *model2world, m2c);
			//rasterizer->setModel2ViewMatrix(m2proj->getDataPtr());
			rasterizer->setModel2CamMatrix(m2c->getDataPtr());
			rasterizer->setCam2ViewMatrix(cam2proj->getDataPtr());
			int size = mesh->vertsToBuffer(floatBuf);

			rasterizer->setVertexData(floatBuf, size);
			rasterizer->setMaterial(obj->getMaterialPtr());
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

	int intBuf[10000];
	float floatBuf[10000];

	float m2projData[16];
	tTransformMatrix* m2proj;

	float m2cData[16];
	tTransformMatrix* m2c;


	float curDt;

	tScene* world;
	tCameraObject* cam;
};
#endif