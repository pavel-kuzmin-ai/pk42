#include "Core.h"
#include "engineState.h"
#include "engineConfig.h"
#include "ConsoleDisplay.h"

engineState sEngineState;
engineConfig cEngineConfig;

coreSystem::coreSystem() {}
coreSystem::~coreSystem(void) {}
void coreSystem::shutDown() { bus.shutDown(); }
void coreSystem::continuousRun(){bus.continuousRun();}



pk42Core::pk42Core() {}
pk42Core::~pk42Core(void) {}
void pk42Core::startUp()
{
	
	sysB = new sysCout("system out");
	sysC = new sysMmapSaverFromMain("system mmap save from main");
	sysD = new sysMmapLoaderFromChild("system mmap load from child");
	sysLogic = new sysCoreLogic("core logic");
	bus.subscribeClient(*sysB);
	bus.subscribeClient(*sysC);
	bus.subscribeClient(*sysD);
	bus.subscribeClient(*sysLogic);

	sysB->startUp();
	sysC->startUp();
	sysD->startUp();
	sysLogic->startUp();

	vGameSystems.push_back(sysB);

	bus.connectEngineState(&sEngineState);
	sEngineState.bEngineInitialized = true;
	std::cout << "engine initialized" << '\n';

	//sysB->runDetached();
	sysC->runDetached();
	sysD->runDetached();
	sysLogic->runDetached();
}

void pk42Core::runGameLoop()
{
	while (sEngineState.bEngineInitialized)
	{
		step();
	}
}
void pk42Core::step()
{
	for (auto sys: vGameSystems)
		sys->callSystemThreadsave();
	bus.callSystemThreadsave();
	
}

pk42Console::pk42Console(){};
pk42Console::~pk42Console(void) {};
void pk42Console::startUp()
{
	sysB = new sysMmapLoaderFromMain("system mmap loader from main console");
	sysC = new sysCout("system out console");
	sysD = new sysDetachedConsole("console");
	sysLogic = new sysCoreLogic("core logic");
	bus.subscribeClient(*sysB);
	bus.subscribeClient(*sysC);
	bus.subscribeClient(*sysD);
	bus.subscribeClient(*sysLogic);
	sysB->startUp();
	sysC->startUp();
	sysD->startUp();
	sysLogic->startUp();

	bus.connectEngineState(&sEngineState);
	sEngineState.bEngineInitialized = true;
	std::cout << "console initialized" << '\n';

	sysB->runDetached();
	sysC->runDetached();
	sysD->runDetached();
	sysLogic->runDetached();
}
