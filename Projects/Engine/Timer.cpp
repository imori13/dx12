#include "Timer.h"
#include "DataAverage.h"

namespace
{
	constexpr float s_SyncDestFrame = 1.f / 60.f;
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

	void Update(const bool frameWait)
	{
		double NewElapsed;
		do
		{
			QueryPerformanceCounter(&s_EndTimer);
			NewElapsed = static_cast<double>(s_EndTimer.QuadPart - s_StartTimer.QuadPart) / static_cast<double>(s_Frequency.QuadPart);
			g_FrameTime = gsl::narrow_cast<float>(NewElapsed - g_ElapsedTime);

		} while(frameWait && g_FrameTime < s_SyncDestFrame);

		g_ElapsedTime = NewElapsed;
	}
}