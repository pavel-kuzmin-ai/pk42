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
		world->addBox(0.f, 0.f, 0.f, 0.1f, 0.2f, 0.3f, 0.0f, 0.0f, 0.0f);
	}

	void shutDown()
	{
		delete world;
		Node::shutDown();
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

