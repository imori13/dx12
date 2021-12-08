#pragma once

enum class Average
{
	None = 1,
	Low = 50,
	Middle = 250,
	High = 700,
	VeryHigh = 1500,
};

namespace DataAverage
{
	void Set(const std::wstring& name, const float value, const Average average);
	float Get(const std::wstring& name);
};