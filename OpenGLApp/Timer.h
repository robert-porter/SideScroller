#pragma once


class Timer
{
public:

	Timer();

	void Reset();

	float GetMilliseconds() const;

private:
	double start;
	static double invFrequency;
};
