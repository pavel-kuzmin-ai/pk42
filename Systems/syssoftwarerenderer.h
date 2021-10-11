#ifndef SYSSOFTWARERENDER_H
#define SYSSOFTWARERENDER_H

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
		renderer = new ISoftwareRenderer(conf->screenWidth, conf->screenHeight, conf->pxlSize);
		renderer->startUp();
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		renderer->updateWorld();
	    renderer->render();
		renderer->displayResult();
		

		string output;
		std::stringstream ss;
		ss << "render done, fps = " << 1./curDt;
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
	ISoftwareRenderer* renderer;
	float curDt;
};
#endif