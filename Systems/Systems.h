#pragma once

#ifdef SYSTEMS_EXPORTS
#define SYSTEMS_API  __declspec(dllexport)   // export DLL information
#else
#define SYSTEMS_API  __declspec(dllimport)   // import DLL information
#endif 

#include <iostream>
#include "sysmsgbus.h"
#include "sysconsole.h"
#include "sysipc.h"
#include <queue>
#include <string>
#include "syscorelogic.h"
#include "sysgamelogic.h"
#include "syssoftwarerenderer.h"
#include "sysplayermechanics.h"
#include "sysinput.h"

//extern "C" {
//	void DLLDIR runEngine();
//	void  DLLDIR runConsole();
//};

class SYSTEMS_API coreSystem
{
public:
	coreSystem();
	~coreSystem();
	void startUp();
	void shutDown();
	void continuousRun();

protected:
	vector<Node*> vSystems;
	sysBus* bus;
};

class SYSTEMS_API pk42Core : public coreSystem
{
public:
	pk42Core();
	~pk42Core();
	void step(float dt);
	void runGameLoop();
	void startUp();
private:
	vector<Node*> vGameSystems;
	sysCout* sysB;
	sysMmapSaverFromMain* sysC;
	sysMmapLoaderFromChild* sysD;
	sysCoreLogic* sysProgLogic;
	sysGameLogic* sysLogic;
	sysSoftwareRenderer* sysDisplay;
	sysPlayerMechanics* sysPlayer;
	sysInput* sysInp;
	int position = 0;
};


class SYSTEMS_API pk42Console : public coreSystem
{
public:
	pk42Console();
	~pk42Console();
	void startUp();
private:
	sysMmapLoaderFromMain* sysB;
	sysCout* sysC;
	sysDetachedConsole* sysD;
	sysCoreLogic* sysProgLogic;
};
