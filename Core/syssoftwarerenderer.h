#ifndef SYSSOFTWARERENDER_H
#define SYSSOFTWARERENDER_H

#include "sysmsgbus.h"
#include <iostream>
#include "consoledisplay.h"
#include "vec3.h"
#include "softwaregl.h"
#include <sstream>

class sysSoftwareRenderer : public Node
{
public:
	sysSoftwareRenderer() {};
	sysSoftwareRenderer(string s, engineConfig* _conf) :Node(s), conf(_conf){};
	virtual ~sysSoftwareRenderer() {};


	void startUp() 
	{
		display = new ConsoleScreen(conf);
		display->startUp();
		screenWidth = display->screenWidth();
		screenHeight = display->screenHeight();
		displayBuffer = display->bufRGB();

		rasterizer = new tRasterizer(screenWidth, screenHeight);
		rasterizer->initBuffers();
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
		world[0].coords.e[0] = world[0].coords.e[0] + 0.001;
		world[1].coords.e[1] = world[1].coords.e[1] + 0.001;
		world[2].coords.e[0] = world[2].coords.e[0] + 0.001;
		world[2].coords.e[1] = world[2].coords.e[1] + 0.001;

		render();

		for (int i = 0; i < screenWidth*screenHeight; i++)
		{
			int idx = i * 3;
			displayBuffer[i] = colorRGB(sGLColor[idx], sGLColor[idx + 1], sGLColor[idx + 2]);

		}

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
	ConsoleScreen* display;
	tRasterizer* rasterizer;
	colorRGB* displayBuffer;
	int screenWidth;
	int screenHeight;
	int* sGLColor;

	float curDt;

	tVertexData world[3] = { tVertexData(vec3<float>(0,0,0)),
							 tVertexData(vec3<float>(0,-1,0)) ,
	                         tVertexData(vec3<float>(-2,-2,0)) };
};
#endif