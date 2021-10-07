#ifndef SYSSOFTWARERENDER_H
#define SYSSOFTWARERENDER_H
#ifdef DLLDIR2_EX
#define DLLDIR2  __declspec(dllexport)   // export DLL information
#else
#define DLLDIR2  __declspec(dllimport)   // import DLL information
#endif 
#include "sysmsgbus.h"
#include <iostream>
#include <sstream>
#include "engineConfig.h"
#include "systemsexternals.h"

class sysSoftwareRenderer : public Node
{
public:
	sysSoftwareRenderer() {};
	sysSoftwareRenderer(string s, engineConfig* _conf) :Node(s), conf(_conf){};
	virtual ~sysSoftwareRenderer() {};


	void startUp() 
	{
		display = new IConsoleScreen(conf->screenWidth, conf->screenHeight, conf->pxlSize);
		display->startUp();
		screenWidth = display->screenWidth();
		screenHeight = display->screenHeight();
		displayBuffer = display->bufRGB();

		rasterizer = new ISoftwareRasterizer(screenWidth, screenHeight);
		rasterizer->initBuffers();
		rasterizer->setOutBuffer(displayBuffer);
		sGLColor = rasterizer->bufColor();
	}

	void render()
	{	
		rasterizer->sGLFreeOutput();
		rasterizer->sGLBufferData(3, world);
		rasterizer->sGLDrawElements(3);
	}



	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		world[0].translate(0.001f, 0.f, 0.f);
		world[1].translate(0.f, 0.001f, 0.f);
		world[2].translate(0.001f, 0.001f, 0.f);

		render();

		//for (int i = 0; i < screenWidth*screenHeight; i++)
		//{
		//	int idx = i * 3;
		//	displayBuffer[i] = colorRGB(sGLColor[idx], sGLColor[idx + 1], sGLColor[idx + 2]);
		//
		//}

		display->updateConsoleBuffer();
		display->show();

		string output;
		std::stringstream ss;
		ss << "render done, fps = " << 1./curDt;
		//ss >> output;
		std::getline(ss, output);
		message msg(sName, output);
		qOut->push(msg);	
		return 0;
	}

	void setDt(float dt)
	{
		curDt = dt;
	}
private:
	engineConfig* conf;
	IConsoleScreen* display;
	ISoftwareRasterizer* rasterizer;
	//colorRGB* displayBuffer;
	int* displayBuffer;
	int screenWidth;
	int screenHeight;
	int* sGLColor;

	float curDt;

	IVertexData world[3] = { IVertexData(0,0,0),
							 IVertexData(0,-1,0) ,
							 IVertexData(-2,-2,0) };
};
#endif