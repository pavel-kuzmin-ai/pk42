#ifndef SYSSOFTWARERENDER_H
#define SYSSOFTWARERENDER_H

#include "sysmsgbus.h"
#include <iostream>
#include "consoledisplay.h"
#include "vec3.h"


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
	}

	void render()
	{	}



	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		render();
		display->updateConsoleBuffer();
		//display->show();
		message msg(sName, "render done");
		qOut->push(msg);	
		return 0;
	}
private:
	engineConfig* conf;
	ConsoleScreen* display;
};
#endif