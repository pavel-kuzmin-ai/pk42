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

	void shutDown()
	{
		renderer->shutDown();
		delete renderer;
		Node::shutDown();
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		//renderer->updateWorld();

		
	    renderer->render();
		renderer->displayResult();
		

		string output;
		std::stringstream ss;
		ss << "render done, fps = " << 1./curDt;
		std::getline(ss, output);
		message msg(sName, output);
		//qOut->push(msg);	
		return 0;
	}

	

	void setWorld(tScene* _wrld)
	{
		renderer->setWorldPtr(_wrld);
		setCamera(_wrld->getCameraPtr());
	}

	void setCamera(tCameraObject* _cam)
	{
		_cam->setCamera(conf->screenWidth / conf->screenHeight);
		renderer->setCamPtr(_cam);
	}
private:
	engineConfig* conf;
	ISoftwareRenderer* renderer;
};
#endif