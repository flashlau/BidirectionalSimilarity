/**
* Code Author: Unknown
* Time Generate: 7/2/2011, PM 09:21
*/

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <time.h>

class Benchmark
{
private:
	clock_t start, finish; 
	float  duration;

public:
	Benchmark();
	void Start();
	void Stop();
	void Reset();
	float GetDuration();
	void PrintDurationMsg();
};

#endif