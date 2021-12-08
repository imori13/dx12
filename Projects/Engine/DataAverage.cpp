#include "DataAverage.h"

#include <map>

namespace
{
	std::map<std::wstring, std::list<float>> performanceView;
}

void DataAverage::Set(const std::wstring& name, const float value, const Average average)
{
	auto& list = performanceView[name];
	list.push_back(value);
	if(list.size() > static_cast<uint32_t>(average))
	{ list.pop_front(); }
}

float DataAverage::Get(const std::wstring& name)
{
	const auto& list = performanceView[name];
	float sum = 0;
	for(const auto element : list)
	{
		sum += element;
	}
	return sum / list.size();
}