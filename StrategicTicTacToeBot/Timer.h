#pragma once
#include <string>
#include <chrono>

class Timer
{
public:
	Timer(std::string name);
	~Timer();

private:
	std::string m_Name;
	std::chrono::steady_clock::time_point m_Start;
};

