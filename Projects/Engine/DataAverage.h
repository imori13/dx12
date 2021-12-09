#pragma once

enum class Average
{
	None = 1,
	VeryLow = 25,
	Low = 100,
	Middle = 300,
	High = 600,
	VeryHigh = 1200,
};

namespace DataAverage
{
	void Set(const std::wstring_view name, const float value, const Average average = Average::Middle);
	float Get(const std::wstring_view name);
};