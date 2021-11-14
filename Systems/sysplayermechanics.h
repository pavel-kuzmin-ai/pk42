#ifndef SYSPLAYERMECHANICS_H
#define SYSPLAYERMECHANICS_H

#include "sysmsgbus.h"
#include "world.h"
#include <iostream>
#include "systemsexternals.h"
#include "engineConfig.h"
#include <sstream>
#include "player.h"
#include <unordered_map>
#include <string>



void unpackStream(std::stringstream& ss, float* buf, int n)
{
	for (int i = 0; i < n; i++)
	{
		ss >> buf[i];
	}
}

void unpackString(std::string& s, float* buf, int n)
{
	std::stringstream ss;
	ss << s;
	for (int i = 0; i < n; i++)
	{
		ss>>buf[i];
	}
}

class sysPlayerMechanics : public Node
{
public:
	typedef void(sysPlayerMechanics::*func)(std::string&);
	sysPlayerMechanics() {};
	sysPlayerMechanics(string s, engineConfig* _conf) :Node(s), conf(_conf) {};
	virtual ~sysPlayerMechanics() {};

	void startUp()
	{
		player = new tPlayer;
		buildCmdDict();
	}

	void shutDown()
	{
		delete player;
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		
		while (!qIn->empty())
		{
			std::stringstream().swap(sscmd);
			message msg = qIn->getMsgAndPop();
			sscmd << msg.value();
			std::string cmd;
			sscmd >> cmd;
			

			if ((cmd[0] == 'p') &(cmd[1] == '_'))
			{
				auto iter = cmdFuncDict.find(cmd);
				if (iter != cmdFuncDict.end())
				{
					std::string params;
					std::getline(sscmd, params);
					(this->*iter->second)(params);
				}
			}
		}
		
		player->commitUpdates();

		
		player->getLocation(fBuf);
		std::stringstream ss;
		ss << "cameraposition " << fBuf[0] << " " << fBuf[1] << " " << fBuf[2]<< '\n';
		std::string cmd;

		std::getline(ss, cmd);
		message msg(sName, cmd, 1);
		qOut->push(msg);
		


		
		return 0;
	}

	void setWorld(tScene* _wrld)
	{
		world = _wrld;
		player->setCamera(_wrld->getCameraPtr());
	}


	void setLocation(std::string& s)
	{
		unpackString(s, fBuf, 3);
		player->setLocation(fBuf[0], fBuf[1], fBuf[2]);
	}

	void setAngles(std::string& s)
	{
		unpackString(s, fBuf, 3);
		player->setAngles(fBuf[0], fBuf[1], fBuf[2]);
	}

	void stepForward(std::string& s)
	{
		player->stepForward(curDt);
	}

	void stepBack(std::string& s)
	{
		player->stepBack(curDt);
	}

	void stepLeft(std::string& s)
	{
		player->stepLeft(curDt);
	}

	void stepRight(std::string& s)
	{
		player->stepRight(curDt);
	}

	void stepUp(std::string& s)
	{
		player->stepUp(curDt);
	}

	void stepDown(std::string& s)
	{
		player->stepDown(curDt);
	}

	void rotate(std::string& s)
	{
		unpackString(s, fBuf, 2);

		player->rotateBy(- angGain * fBuf[1], - angGain * fBuf[0], 0.f);
	}

	void buildCmdDict()
	{
		cmdFuncDict.emplace("p_pos", &sysPlayerMechanics::setLocation);
		cmdFuncDict.emplace("p_ang", &sysPlayerMechanics::setAngles);
		cmdFuncDict.emplace("p_forward", &sysPlayerMechanics::stepForward);
		cmdFuncDict.emplace("p_back", &sysPlayerMechanics::stepBack);
		cmdFuncDict.emplace("p_right", &sysPlayerMechanics::stepRight);
		cmdFuncDict.emplace("p_left", &sysPlayerMechanics::stepLeft);
		cmdFuncDict.emplace("p_up", &sysPlayerMechanics::stepUp);
		cmdFuncDict.emplace("p_down", &sysPlayerMechanics::stepDown);
		cmdFuncDict.emplace("p_mousepos", &sysPlayerMechanics::rotate);
	}


private:
	tScene* world;
	//tCameraObject* camera;
	tPlayer* player;
	engineConfig* conf;
	std::unordered_map<std::string, func> cmdFuncDict;
	std::stringstream sscmd;

	float angGain = 0.001;


	float fBuf[3];
	int iBuf[3];

};

#endif