#pragma once

#include <deque>

enum class Average
{
	None = 2,
	VeryLow = 10,
	Low = 20,
	Middle = 30,
	High = 40,
	VeryHigh = 50,
};

namespace DataAverage
{
	void Set(const std::wstring_view name, const float value, const Average average = Average::Middle);
	float Get(const std::wstring_view name);
	std::vector<float> GetArray(const std::wstring_view name);
};