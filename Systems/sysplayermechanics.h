#ifndef SYSPLAYERMECHANICS_H
#define SYSPLAYERMECHANICS_H

#include "sysmsgbus.h"
#include "world.h"
#include <iostream>
#include "systemsexternals.h"
#include "engineConfig.h"
#include <sstream>

class sysPlayerMechanics : public Node
{
public:
	sysPlayerMechanics() {};
	sysPlayerMechanics(string s, engineConfig* _conf) :Node(s), conf(_conf) {};
	virtual ~sysPlayerMechanics() {};

	void startUp()
	{
	}

	void shutDown()
	{
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		
		while (!qIn->empty())
		{
			message msg = qIn->getMsgAndPop();
			std::stringstream ss;
			std::string cmd;
			ss >> cmd;
			if (cmd == "camerapos")
			{
				string output;
				std::stringstream ss;
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						ss << camera->getW2Projection()->getValue(i, j)<<" ";
					}
				}
				ss << '\n';
				
				std::getline(ss, output);
				message msg3(sName, output);
				qOut->push(msg3);

				float x, y, z;
				ss >> x;
				ss >> y;
				ss >> z;

				camera->setLocation(x, y, z);
				camera->UpdateTransforms();

				
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						ss << camera->getW2Projection()->getValue(i, j) << " ";
					}
				}
				ss << '\n';

				
				std::getline(ss, output);
				message msg2(sName, output);
				qOut->push(msg);
			}

			if (cmd == "cameraang")
			{
				float x, y, z;
				ss >> x;
				ss >> y;
				ss >> z;

				camera->setAngles(x, y, z);
				camera->UpdateTransforms();
			}

			string output;
			ss << "my message: ";
			std::getline(ss, output);
			message msg4(sName, output);
		    qOut->push(msg4);
			qOut->push(msg);
			
		}
		
		return 0;
	}

	void setWorld(tScene* _wrld)
	{
		world = _wrld;
		setCamera(_wrld->getCameraPtr());
	}

	void setCamera(tCameraObject* _cam)
	{
		camera = _cam;
	}
private:
	tScene* world;
	tCameraObject* camera;
	float curDt;
	engineConfig* conf;
};

#endif