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

float fArrMin(const float* inArr, int size)
{
	float out = inArr[0];
	for (int i = 1; i < size; i++)
	{
		float tmp = inArr[i];
		if (tmp < out) out = tmp;
	}
	return out;
}

float fArrMax(const float* inArr, int size)
{
	float out = inArr[0];
	for (int i = 1; i < size; i++)
	{
		float tmp = inArr[i];
		if (tmp > out) out = tmp;
	}
	return out;
}

int argMin(const float* inArr, int size)
{
	int out = 0;
	float outVal = inArr[out];
	for (int i = 1; i < size; i++)
	{
		float tmp = inArr[i];
		if (tmp < outVal)
		{
			out = tmp;
			outVal = inArr[out];
		}
	}
	return out;
}


int argMax(const float* inArr, int size)
{
	int out = 0;
	float outVal = inArr[out];
	for (int i = 1; i < size; i++)
	{
		float tmp = inArr[i];
		if (tmp > outVal)
		{
			out = tmp;
			outVal = inArr[out];
		}
	}
	return out;
}
#endif
