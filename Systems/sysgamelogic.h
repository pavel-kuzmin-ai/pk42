#ifndef SYSGAMELOGIC_H
#define SYSGAMELOGIC_H

#include "sysmsgbus.h"
#include "world.h"
#include <iostream>

class sysGameLogic : public Node
{
public:
	sysGameLogic() {};
	sysGameLogic(string s) :Node(s) {};
	virtual ~sysGameLogic() {};

	void startUp() 
	{
		world = new tScene();
		world->addBox(0, 0, 0, 0.1, 0.2, 0.3, 0.0, 0.7, 0.2);
	}

	void shutDown()
	{
		delete world;
	}

	tScene* getWorldPtr()
	{
		return world;
	}

	int executeCommands(messageQueue* qIn, messageQueue* qOut)
	{
		return 0;
	}
private:
	tScene* world;
};





#endif

