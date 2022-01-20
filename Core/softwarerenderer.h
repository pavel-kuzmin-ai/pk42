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
		display->setFRGB(rasterizer->getOutBufferFloat());
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
			//if (!(obj->bIsVisible())) continue;
			tTransformMatrix* model2world = obj->getM2Wmatrix();
			mesh = obj->getMeshPtr();

			Multiply(*world2cam, *model2world, m2c);
			rasterizer->setModel2CamMatrix(m2c->getDataPtr());
			rasterizer->setCam2ViewMatrix(cam2proj->getDataPtr());
			int sizeVerts = mesh->vertsToBuffer(floatBuf);

			rasterizer->setVertexData(floatBuf, sizeVerts);
			rasterizer->setMaterial(obj->getMaterialPtr());
			int size = mesh->trisVertsToBuffer(intBuf);
			rasterizer->sGLBufferVerts(size, intBuf);

			int sizeNormals = mesh->normsToBuffer(floatBuf);
			if (sizeNormals > 0)
			{
				rasterizer->setNormalsData(floatBuf, sizeNormals);
				rasterizer->sGLBufferNormals(size, intBuf);
			}
				

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