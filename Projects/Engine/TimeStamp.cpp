#include "TimeStamp.h"

namespace
{
	constexpr uint64_t SECOND = 1;
	constexpr uint64_t MILLI_SECOND = 1000;
	constexpr uint64_t MODULES = 1000000;

	LARGE_INTEGER s_Frequency;
	std::vector<LARGE_INTEGER> s_StartTimers;
	LARGE_INTEGER s_EndTimer;
}

namespace TimeStamp
{
	void Initialize() noexcept
	{
		QueryPerformanceFrequency(&s_Frequency);
	}

	void Begin()
	{
		QueryPerformanceCounter(&s_StartTimers.emplace_back());
	}

	float End() noexcept
	{
		QueryPerformanceCounter(&s_EndTimer);
		const auto value = static_cast<float>(s_EndTimer.QuadPart - s_StartTimers.back().QuadPart) / static_cast<float>(s_Frequency.QuadPart) * MILLI_SECOND;
		s_StartTimers.pop_back();
		return value;
	}
}