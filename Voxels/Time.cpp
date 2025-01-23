#include "Time.h"

float Time::DeltaTime = 0.0f;
std::chrono::system_clock::time_point Time::previousTime = std::chrono::system_clock::now();

void Time::UpdateDeltaTime()
{
	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedSeconds = endTime - previousTime;
	DeltaTime = elapsedSeconds.count();
	previousTime = endTime;
}