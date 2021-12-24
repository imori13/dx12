#pragma once

#include <deque>

enum class Average
{
	None = 2,
	VeryLow = 5,
	Low = 10,
	Middle = 20,
	High = 30,
	VeryHigh = 50,
};

namespace DataAverage
{
	void Set(const std::wstring_view name, const float value, const Average average = Average::Middle);
	float Get(const std::wstring_view name);
	std::vector<float> GetArray(const std::wstring_view name);
};