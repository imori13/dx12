#include "TimeStamp.h"
#include <boost/timer/timer.hpp>
#include "Debug.h"

namespace
{
	std::vector<boost::timer::cpu_timer> s_CPUTimers;

	constexpr float MODULE = 1000000.0f;
}

namespace TimeStamp
{
	void Begin()
	{
		s_CPUTimers.emplace_back().start();
	}

	float End() noexcept
	{
		const auto& timer = s_CPUTimers.back();

		// 大きな待機時間は無視する narrow_cast
		const float retVal = gsl::narrow_cast<float>(timer.elapsed().wall);

		s_CPUTimers.pop_back();

		return retVal / MODULE;
	}

	void Stop() noexcept
	{
		s_CPUTimers.back().stop();
	}

	void Resume() noexcept
	{
		s_CPUTimers.back().resume();
	}
}