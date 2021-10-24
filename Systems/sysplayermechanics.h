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

typedef void(*func)(std::stringstream*);

void unpackStream(std::stringstream& ss, float* buf, int n)
{
	for (int i = 0; i < n; i++)
	{
		ss >> buf[i];
	}
}

class sysPlayerMechanics : public Node
{
public:
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
					(*iter->second)(&sscmd);
				}

			}
			
		}
		player->commitUpdates();

		
		return 0;
	}

	void setWorld(tScene* _wrld)
	{
		world = _wrld;
		player->setCamera(_wrld->getCameraPtr());
	}


	void setLocation(std::stringstream* ss)
	{
		unpackStream(*ss, fBuf, 3);
		player->setLocation(fBuf[0], fBuf[1], fBuf[2]);
	}

	void setAngles(std::stringstream* ss)
	{
		unpackStream(*ss, fBuf, 3);
		player->setAngles(fBuf[0], fBuf[1], fBuf[2]);
	}

	void stepForward(std::stringstream ss)
	{
		unpackStream(ss, fBuf, 0);
		player->stepForward(curDt);
	}

	void stepBack(std::stringstream ss)
	{
		unpackStream(ss, fBuf, 0);
		player->stepBack(curDt);
	}

	void stepLeft(std::stringstream ss)
	{
		unpackStream(ss, fBuf, 0);
		player->stepLeft(curDt);
	}

	void stepRight(std::stringstream ss)
	{
		unpackStream(ss, fBuf, 0);
		player->stepRight(curDt);
	}

	void stepUp(std::stringstream ss)
	{
		unpackStream(ss, fBuf, 0);
		player->stepUp(curDt);
	}

	void stepDown(std::stringstream ss)
	{
		unpackStream(ss, fBuf, 0);
		player->stepDown(curDt);
	}

	void buildCmdDict()
	{
		cmdFuncDict.emplace("p_pos", &sysPlayerMechanics::setLocation);
		cmdFuncDict.emplace("p_ang", &sysPlayerMechanics::setAngles);
	}


private:
	tScene* world;
	//tCameraObject* camera;
	tPlayer* player;
	float curDt;
	engineConfig* conf;
	std::unordered_map<std::string, func> cmdFuncDict;
	std::stringstream sscmd;


	float fBuf[3];
	int iBuf[3];

};

#endif