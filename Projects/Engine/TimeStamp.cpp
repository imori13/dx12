#include "TimeStamp.h"

#include <map>

namespace
{
	constexpr uint64_t SECOND = 1;
	constexpr uint64_t MILLI_SECOND = 1000;
	constexpr uint64_t MODULES = 1000000;

	LARGE_INTEGER s_NativeFrequency;
	std::map<std::wstring, LARGE_INTEGER> s_StartTimers;
	LARGE_INTEGER s_EndTimer;
}

namespace TimeStamp
{
	void Initialize() noexcept
	{
		QueryPerformanceFrequency(&s_NativeFrequency);
	}

	void Begin(const std::wstring& watchName)
	{
		QueryPerformanceCounter(&s_StartTimers[watchName]);
	}

	float End(const std::wstring& watchName)
	{
		QueryPerformanceCounter(&s_EndTimer);
		const auto value = static_cast<float>(s_EndTimer.QuadPart - s_StartTimers[watchName].QuadPart) / static_cast<float>(s_NativeFrequency.QuadPart) * MILLI_SECOND;
		LOGLINE("%s : %3.4f ms", (watchName.size() > 0) ? (watchName.c_str()) : (L"_"), value);
		return value;
	}
}