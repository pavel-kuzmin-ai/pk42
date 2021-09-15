#include "Core.h"
#include "engineState.h"
#include "engineConfig.h"

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
	display = new ConsoleScreen(&cEngineConfig);

	bus.subscribeClient(*sysB);
	bus.subscribeClient(*sysC);
	bus.subscribeClient(*sysD);
	bus.subscribeClient(*sysLogic);

	sysB->startUp();
	sysC->startUp();
	sysD->startUp();
	sysLogic->startUp();
	display->startUp();
	
	vGameSystems.push_back(sysB);

	bus.connectEngineState(&sEngineState);
	sEngineState.bEngineInitialized = true;
	std::cout << "engine initialized" << '\n';

	//sysB->runDetached();
	sysC->runDetached();
	sysD->runDetached();
	sysLogic->runDetached();

	display->buildScreen();
	//display->build_map();
	//display->display_colors();
}

void pk42Core::runGameLoop()
{
	int i = 0;

	int gran = 1;
	while (sEngineState.bEngineInitialized)
	{
		if (i % gran == 0)
		{
			position += 1;
			if (position > display->bufSize())
			{
				position = 0;
			}
		}
		step();

		i++;
	}
}
void pk42Core::step()
{
	for (auto sys: vGameSystems)
		sys->callSystemThreadsave();
	bus.callSystemThreadsave();

	colorRGB* colorBuf = display->bufRGB();

	//for (int i = position; i < 10; i++)
	//{
		colorBuf[position] = colorRGB(0, 0, 0);
		colorBuf[position + 1] = colorRGB(255, 0, 0);
		colorBuf[position + 2] = colorRGB(0, 255, 0);
		colorBuf[position + 3] = colorRGB(0, 0, 255);
		colorBuf[position + 4] = colorRGB(255, 255, 0);
		colorBuf[position + 5] = colorRGB(0, 255, 255);
		colorBuf[position + 6] = colorRGB(255, 0, 255);
		colorBuf[position + 7] = colorRGB(255, 255, 255);
		colorBuf[position + 8] = colorRGB(128, 32, 64);
		colorBuf[position + 9] = colorRGB(65, 92, 12);
	//}

		//std::cout << position << '\n';
	display->updateConsoleBuffer();
	display->show();

	
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
