/**
* Code Author: Unknown
* Time Generate: 7/2/2011, PM 09:21
*/

#include "Benchmark.h"

#include <stdio.h>

Benchmark::Benchmark()
{
}

void Benchmark::Start()
{
	start = clock();
}

void Benchmark::Stop()
{
	finish = clock();
	duration = (float)(finish - start) / CLOCKS_PER_SEC;
}

void Benchmark::Reset()
{

}

float Benchmark::GetDuration()
{
	return duration;
}

void Benchmark::PrintDurationMsg()
{
	printf("Time Duration: %f sec\n", duration);
}