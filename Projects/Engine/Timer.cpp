#include "Timer.h"
#include "DataAverage.h"

namespace
{
	LARGE_INTEGER s_Frequency;
	LARGE_INTEGER s_StartTimer;
	LARGE_INTEGER s_EndTimer;
}

namespace Timer
{
	double g_ElapsedTime;
	float g_FrameTime;

	void Initialize()
	{
		QueryPerformanceFrequency(&s_Frequency);
		QueryPerformanceCounter(&s_StartTimer);
	}

	void Update()
	{
		QueryPerformanceCounter(&s_EndTimer);
		const double NewElapsed = static_cast<double>(s_EndTimer.QuadPart - s_StartTimer.QuadPart) / static_cast<double>(s_Frequency.QuadPart);
		g_FrameTime = gsl::narrow_cast<float>(NewElapsed - g_ElapsedTime);
		g_ElapsedTime = NewElapsed;
	}
}