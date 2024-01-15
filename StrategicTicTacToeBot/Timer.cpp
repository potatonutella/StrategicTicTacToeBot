#include "Timer.h"
#include <iostream>

Timer::Timer(std::string name)
{
	m_Name = name;
	m_Start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
	auto m_End = std::chrono::high_resolution_clock::now();
	if (m_Name != "") std::cout << "Function " << m_Name << " took " << std::chrono::duration<double, std::milli>(m_End - m_Start).count() << "ms.\n";
	else std::cout << std::chrono::duration<double, std::milli>(m_End - m_Start).count() << "\n";
}
