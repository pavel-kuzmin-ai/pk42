#ifndef MATHUTILS_H
#define MATHUTILS_H

float removePeriodic(float inp, float min, float period)
{
	if (inp > min)
	{
		inp = inp - min;
		return min + inp - period * (int)(inp / period);
	}

	if (inp < min)
	{
		inp = min - inp;
		return min + period - (inp - period * (int)(inp / period));
	}


	return inp;
	
}

#endif
