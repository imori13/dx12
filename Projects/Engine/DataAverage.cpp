#include "DataAverage.h"
#include "Timer.h"

#include <map>

struct Value
{
	double Sum = {};
	std::list<float> ValueList = {};
};

namespace
{
	std::map<std::wstring, Value> averageList;
}

// ˆÚ“®•½‹Ï
void DataAverage::Set(const std::wstring& name, const float value, const Average limit)
{
	auto& pair = averageList[name];
	pair.Sum += pair.ValueList.emplace_back(value);

	// ŒÃ‚¢ƒf[ƒ^‚ð”jŠü‚·‚é
	while(pair.ValueList.size() * (Timer::g_FrameTime * 60) > static_cast<uint32_t>(limit))
	{
		pair.Sum -= pair.ValueList.front();
		pair.ValueList.pop_front();
	}
}

float DataAverage::Get(const std::wstring& name)
{
	const auto& pair = averageList[name];
	return gsl::narrow_cast<float>(pair.Sum / static_cast<double>(pair.ValueList.size()));
}