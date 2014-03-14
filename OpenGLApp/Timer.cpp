#include "Timer.h"
#include <windows.h>

double Timer::invFrequency = 0.0f;

Timer::Timer()
{
	LARGE_INTEGER largeInteger;

	if (invFrequency == 0.0f)
	{
		QueryPerformanceFrequency(&largeInteger);
		invFrequency = double(largeInteger.QuadPart);
		if (invFrequency > 0.0f)
		{
			invFrequency = 1000.0f / invFrequency;
		}
	}

	QueryPerformanceCounter(&largeInteger);
	start = double(largeInteger.QuadPart);
}

void Timer::Reset()
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	start = double(largeInteger.QuadPart);
}

float Timer::GetMilliseconds() const
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	double count = double(largeInteger.QuadPart);
	float ms = float(invFrequency * (count - start));
	return ms;
}
