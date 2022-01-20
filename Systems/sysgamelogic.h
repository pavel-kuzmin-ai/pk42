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
		int ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\box.obj");
		//int ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		world->spawnObject(ind, 0, 0, -5);

		ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		world->spawnObject(ind, 0, 0, -5);

		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\box.obj");
		//world->spawnObject(ind, -2, 0, -5);

		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		//world->spawnObject(ind, -2, 2, -5);

		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		//world->spawnObject(ind, 0, -2, -5);



		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		//world->spawnObject(ind, 0, 2, -5);

		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		//world->spawnObject(ind, 2, -2, -5);

		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		//world->spawnObject(ind, 2, 0, -5);

		//ind = world->addMeshObject("E:\\git\\pk42\\TestAssets\\sphere.obj");
		//world->spawnObject(ind, 2, 2, -5);


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
		qIn->erase();
		return 0;
	}
private:
	tScene* world;
};





#endif

