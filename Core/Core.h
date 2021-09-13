#pragma once

#ifdef DLLDIR_EX
#define DLLDIR  __declspec(dllexport)   // export DLL information
#else
#define DLLDIR  __declspec(dllimport)   // import DLL information
#endif 

#include <iostream>
#include "msgBus.h"
#include "console.h"
#include "logging.h"
#include "ipc.h"
#include <queue>
#include <string>
#include "CoreLogic.h"

//extern "C" {
//	void DLLDIR runEngine();
//	void  DLLDIR runConsole();
//};

class DLLDIR coreSystem
{
public:
	coreSystem();
	~coreSystem();
	void shutDown();
	void continuousRun();

protected:
	vector<Node*> vSystems;
	sysBus bus;
};

class DLLDIR pk42Core : public coreSystem
{
public:
	pk42Core();
	~pk42Core();
	void step();
	void runGameLoop();
	void startUp();
private:
	vector<Node*> vGameSystems;
	sysCout* sysB;
	sysMmapSaverFromMain* sysC;
	sysMmapLoaderFromChild* sysD;
	sysCoreLogic* sysLogic;
};


class DLLDIR pk42Console : public coreSystem
{
public:
	pk42Console();
	~pk42Console();
	void startUp();
private:
	sysMmapLoaderFromMain* sysB;
	sysCout* sysC;
	sysDetachedConsole* sysD;
	sysCoreLogic* sysLogic;
};
