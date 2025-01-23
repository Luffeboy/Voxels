#pragma once
#include <chrono>
#include <iostream>

class Time
{
private:
	static std::chrono::system_clock::time_point previousTime;
public:
	static float DeltaTime;
	static void UpdateDeltaTime();

	class Timer
	{
	private:
		std::chrono::system_clock::time_point startTime;
		std::chrono::system_clock::time_point endTime;
		const char* m_name;
	public:
		Timer(const char* name = "Timer")
		{
			m_name = name;
			Start();
		}
		void Start()
		{
			startTime = std::chrono::system_clock::now();
		}
		void Stop()
		{
			endTime = std::chrono::system_clock::now();
			PrintElapsedTime();
		}
		void PrintElapsedTime()
		{
			std::chrono::duration<double> elapsedSeconds = endTime - startTime;
			std::cout << m_name << ": " << elapsedSeconds.count() << " seconds" << std::endl;
		}
	};
};