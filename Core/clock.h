#ifndef CLOCK_H
#define CLOCK_H

#include "highrestimer.h"


class tClock
{
public:
	tClock(float startTimeSeconds = 0.0f) : 
		uTimeCycles(secondsToCycles(startTimeSeconds)),
		fTimeScale(1.0f),
		bIsPaused(false)
	{};
	~tClock() {};

	inline unsigned long long secondsToCycles(float timeSeconds)
	{
		return (unsigned long long)( timeSeconds * fCyclesPerSecond );
	}

	float cyclesToSeconds(unsigned long long timeCycles)
	{
		return (float)(timeCycles / fCyclesPerSecond);
	}
	
	void init()
	{
		fCyclesPerSecond = (float)readHighResTimerFrequency();
	}

	unsigned long long getTimeCycles() const
	{
		return uTimeCycles;
	}

	float calcDeltaSeconds(const tClock& other)
	{
		unsigned long long dt = getTimeCycles() - other.getTimeCycles();
		return cyclesToSeconds(dt);
	}

	void update(float dtRealSeconds)
	{
		if (!bIsPaused)
		{
			unsigned long long dtScaledCycles = secondsToCycles(dtRealSeconds * fTimeScale);
			uTimeCycles += dtScaledCycles;
		}

	}

	void startMeasure()
	{
		uStartedTicks = readHigResTimer();
	}

	void checkAndSwapMeasure()
	{
		uEndTicks = readHigResTimer();
		deltaTicks = uEndTicks - uStartedTicks;
		float dt = cyclesToSeconds(deltaTicks);
		updateClock(dt);
		uStartedTicks = uEndTicks;		
	}


	void setPaused(bool paused)
	{
		bIsPaused = paused;
	}


	bool IsPaused()
	{
		return bIsPaused;
	}

	void setTimeScale(float scale)
	{
		fTimeScale = scale;
	}

	float getTimeScale()
	{
		return fTimeScale;
	}

	void singleStep()
	{
		update(1.0f / 30.0f);
	}

	void updateClock(float dtRealSeconds)
	{
		if (dtRealSeconds > 1.0f)
		{
			singleStep();
		}
		else
		{
			update(dtRealSeconds);
		}
	}


private:
	unsigned long long uTimeCycles;
	float fTimeScale;
	bool bIsPaused;

	unsigned long long uStartedTicks{ 0 }, uEndTicks{ 0 }, deltaTicks{ 0 };

	float fCyclesPerSecond;
};

#endif