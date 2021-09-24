#ifndef HIGHRESTIMER_H
#define HIGHRESTIMER_H

#include <windows.h>

LARGE_INTEGER Frequency;

unsigned long long readHigResTimer()
{
	LARGE_INTEGER CurTime;
	QueryPerformanceCounter(&CurTime);
	return (unsigned long long) CurTime.QuadPart;
}

unsigned long long readHighResTimerFrequency()
{
	LARGE_INTEGER CurF;
	QueryPerformanceFrequency(&CurF);
	return (unsigned long long) CurF.QuadPart;
}

#endif