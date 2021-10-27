#ifndef SYSINPUT_H
#define SYSINPUT_H

#include "sysmsgbus.h"
#include <iostream>
#include "systemsexternals.h"
#include "engineConfig.h"
#include <sstream>
#include "input.h"
#include <unordered_map>
#include <string>




class sysInput : public Node
{
public:
	//typedef void(sysPlayerMechanics::*func)(std::string&);
	sysInput() {};
	sysInput(string s, engineConfig* _conf) :Node(s), conf(_conf) {};
	virtual ~sysInput() {};

	void startUp()
	{
		inp = new tInputHandler;
		bKeys = inp->getKeysPtr();
		bMouse = inp->getMousePtr();
		iMouseCoords = inp->getMouseCoordsPtr();
	}

	void shutDown()
	{
		delete inp;
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		inp->updateKeyboardState();
		if (bKeys[0x57])
		{
			message msg(sName, "p_forward");
			qOut->push(msg);
		}
		if (bKeys[0x53])
		{
			message msg(sName, "p_back");
			qOut->push(msg);
		}
		if (bKeys[0x41])
		{
			message msg(sName, "p_left");
			qOut->push(msg);
		}
		if (bKeys[0x44])
		{
			message msg(sName, "p_right");
			qOut->push(msg);
		}
		if (bKeys[0x10])
		{
			message msg(sName, "p_up");
			qOut->push(msg);
		}
		if (bKeys[0x11])
		{
			message msg(sName, "p_down");
			qOut->push(msg);
		}
		if (bKeys[0x01])
		{
			message msg(sName, "p_mouse_left");
			qOut->push(msg);
		}
		if (bKeys[0x02])
		{
			message msg(sName, "p_mouse_right");
			qOut->push(msg);
		}
		if (bKeys[0x04])
		{
			message msg(sName, "p_mouse_mid");
			qOut->push(msg);
		}
		if (bKeys[0x05])
		{
			message msg(sName, "p_mouse_x1");
			qOut->push(msg);
		}
		if (bKeys[0x06])
		{
			message msg(sName, "p_mouse_x2");
			qOut->push(msg);
		}

		std::stringstream ss;
		ss << "p_mousepos " << iMouseCoords[0] << " " << iMouseCoords[1]<<'\n';
		std::string cmd;

		std::getline(ss, cmd);
		message msg(sName, cmd);
		qOut->push(msg);
		
		return 0;
	}

	

private:
	tInputHandler* inp;
	bool* bKeys;
	bool* bMouse;
	int* iMouseCoords;
	engineConfig* conf;
};

#endif