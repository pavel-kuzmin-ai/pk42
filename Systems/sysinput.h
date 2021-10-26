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
		
		return 0;
	}

	

private:
	tInputHandler* inp;
	bool* bKeys;
	engineConfig* conf;
};

#endif