#ifndef MATHUTILS_H
#define MATHUTILS_H

float removePeriodic(float inp, float min, float period)
{
	inp -= min;
	return min + inp - period * (int)(inp / period);
}

#endif
