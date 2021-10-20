#ifndef ENGINESTATE_H
#define ENGINESTATE_H


struct engineState
{
	bool bEngineInitialized = false;
	bool bPaused = false;
	float fTimeScale = 1.f;
};

#endif