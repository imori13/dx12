#include "DataAverage.h"
#include "Timer.h"

#include <map>

struct Value
{
	double Sum = {};
	std::vector<float> ValueList = {};
};

namespace
{
	std::map<std::wstring, Value> averageList;
}

// 単純移動平均
void DataAverage::Set(const std::wstring_view name, const float value, const Average limit)
{
	auto& pair = averageList[name.data()];
	pair.Sum += pair.ValueList.emplace_back(value);

	// 移動平均の要素数を"時間単位"で管理する
	while(pair.ValueList.size() * (Timer::g_FrameTime * 60) >= static_cast<uint32_t>(limit))
	{
		pair.Sum -= pair.ValueList.front();
		pair.ValueList.erase(pair.ValueList.begin());
	}
}

float DataAverage::Get(const std::wstring_view name)
{
	const auto& pair = averageList[name.data()];
	return gsl::narrow_cast<float>(pair.Sum / static_cast<double>(pair.ValueList.size()));
}

std::vector<float> DataAverage::GetArray(const std::wstring_view name)
{
	const auto& deque = averageList[name.data()].ValueList;
	return deque;
}
