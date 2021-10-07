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
void pk42Core::startUp()
{
	coreSystem::startUp();
	//sysB = new sysCout("system out");
	sysC = new sysMmapSaverFromMain("system mmap save from main");
	sysD = new sysMmapLoaderFromChild("system mmap load from child");
	sysLogic = new sysCoreLogic("core logic");
	sysDisplay = new sysSoftwareRenderer("renderer", &cEngineConfig);

	//bus.subscribeClient(*sysB);
	bus->subscribeClient(*sysC);
	bus->subscribeClient(*sysD);
	bus->subscribeClient(*sysLogic);
	bus->subscribeClient(*sysDisplay);

	//sysB->startUp();
	sysC->startUp();
	sysD->startUp();
	sysLogic->startUp();
	sysDisplay->startUp();
	
	vGameSystems.push_back(sysDisplay);

	bus->connectEngineState(&sEngineState);
	sEngineState.bEngineInitialized = true;
	std::cout << "engine initialized" << '\n';

	//sysB->runDetached();
	sysC->runDetached();
	sysD->runDetached();
	sysLogic->runDetached();
	//sysDisplay->runDetached();

	//display->buildScreen();
	//display->build_map();
	//display->display_colors();
}

void pk42Core::runGameLoop()
{
	//IClock clockCur, clockPrev;
	//clockCur.init();
	//clockPrev = clockCur;
	//clockCur.startMeasure();
	float dt = 1.0f / 30.0f;
	while (sEngineState.bEngineInitialized)
	{
		
		step(dt);
		//clockCur.checkAndSwapMeasure();
		//dt = clockCur.calcDeltaSeconds(clockPrev);
		//clockPrev = clockCur;
	}
}
void pk42Core::step(float dt)
{
	//for (auto sys: vGameSystems)
	//	sys->callSystemThreadsave();
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
	sysLogic = new sysCoreLogic("core logic");
	bus->subscribeClient(*sysB);
	bus->subscribeClient(*sysC);
	bus->subscribeClient(*sysD);
	bus->subscribeClient(*sysLogic);
	sysB->startUp();
	sysC->startUp();
	sysD->startUp();
	sysLogic->startUp();

	bus->connectEngineState(&sEngineState);
	sEngineState.bEngineInitialized = true;
	std::cout << "console initialized" << '\n';

	sysB->runDetached();
	sysC->runDetached();
	sysD->runDetached();
	sysLogic->runDetached();
}
