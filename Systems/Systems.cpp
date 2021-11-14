#include "Systems.h"


engineState sEngineState;
engineConfig cEngineConfig;

coreSystem::coreSystem() {}
coreSystem::~coreSystem(void) {}
void coreSystem::startUp()
{

	//sysB = new sysCout("system out");
	bus = new sysBus("msg bus");
}
void coreSystem::shutDown() { bus->shutDown(); }
void coreSystem::continuousRun(){bus->continuousRun();}



pk42Core::pk42Core() {}
pk42Core::~pk42Core(void) {}
void pk42Core::startUp(bool bWithConsole)
{
	coreSystem::startUp();
	//sysB = new sysCout("system out");
	if (bWithConsole) 
	{
		sysC = new sysMmapSaverFromMain("system mmap save from main");
		sysD = new sysMmapLoaderFromChild("system mmap load from child");
	}
	
	sysProgLogic = new sysCoreLogic("core logic");
	sysLogic = new sysGameLogic("game logic");
	sysDisplay = new sysSoftwareRenderer("renderer", &cEngineConfig);
	sysPlayer = new sysPlayerMechanics("player", &cEngineConfig);
	sysInp = new sysInput("input", &cEngineConfig);

	//bus.subscribeClient(*sysB);
	if (bWithConsole)
	{
		bus->subscribeClient(*sysC);
		bus->subscribeClient(*sysD);
	}
	
	bus->subscribeClient(*sysProgLogic);
	bus->subscribeClient(*sysLogic);
	bus->subscribeClient(*sysDisplay);
	bus->subscribeClient(*sysPlayer);
	bus->subscribeClient(*sysInp);
	
	bus->connectEngineState(&sEngineState);


	//sysB->startUp();
	if (bWithConsole)
	{
		sysC->startUp();
		sysD->startUp();
	}
	
	sysProgLogic->startUp();
	sysLogic->startUp();
	sysDisplay->startUp();
	sysPlayer->startUp();
	sysInp->startUp();
	
	vGameSystems.push_back(sysLogic);
	vGameSystems.push_back(sysDisplay);
	vGameSystems.push_back(sysPlayer);
	vGameSystems.push_back(sysInp);


	sysDisplay->setWorld(sysLogic->getWorldPtr());
	sysPlayer->setWorld(sysLogic->getWorldPtr());
	sEngineState.bEngineInitialized = true;
	std::cout << "engine initialized" << '\n';

	//sysB->runDetached();
	if (bWithConsole)
	{
		sysC->runDetached();
		sysD->runDetached();
	}
	
	sysProgLogic->runDetached();
	//sysDisplay->runDetached();

	//display->buildScreen();
	//display->build_map();
	//display->display_colors();
}

void pk42Core::runGameLoop()
{
	IClock clockCur;
	clockCur.init();
	clockCur.startMeasure();
	float dt = 1.0f / 30.0f;
	while (sEngineState.bEngineInitialized)
	{
		
		step(dt);
		dt = clockCur.checkAndSwapMeasure();
	}
}
void pk42Core::step(float dt)
{
	//for (auto sys: vGameSystems)
	//	sys->callSystemThreadsave();
	sysInp->callSystemThreadsave();
	sysPlayer->setDt(dt);
	sysPlayer->callSystemThreadsave();
	sysDisplay->setDt(dt);
	sysDisplay->callSystemThreadsave();
	bus->callSystemThreadsave();
}

pk42Console::pk42Console(){};
pk42Console::~pk42Console(void) {};
void pk42Console::startUp()
{
	coreSystem::startUp();
	sysB = new sysMmapLoaderFromMain("system mmap loader from main console");
	sysC = new sysCout("system out console");
	sysD = new sysDetachedConsole("console");
	sysProgLogic = new sysCoreLogic("core logic");
	bus->subscribeClient(*sysB);
	bus->subscribeClient(*sysC);
	bus->subscribeClient(*sysD);
	bus->subscribeClient(*sysProgLogic);
	sysB->startUp();
	sysC->startUp();
	sysD->startUp();
	sysProgLogic->startUp();

	bus->connectEngineState(&sEngineState);
	sEngineState.bEngineInitialized = true;
	std::cout << "console initialized" << '\n';

	sysB->runDetached();
	sysC->runDetached();
	sysD->runDetached();
	sysProgLogic->runDetached();
}
