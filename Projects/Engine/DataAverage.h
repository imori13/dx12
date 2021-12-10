#pragma once

#include <deque>

enum class Average
{
	None = 2,
	VeryLow = 10,
	Low = 100,
	Middle = 300,
	High = 600,
	VeryHigh = 1200,
};

namespace DataAverage
{
	void Set(const std::wstring_view name, const float value, const Average average = Average::Middle);
	float Get(const std::wstring_view name);
	std::vector<float> GetArray(const std::wstring_view name);
};