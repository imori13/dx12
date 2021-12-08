#pragma once

enum class Average
{
	Default = 1000,
	Low = 100,
	Middle = 1000,
	High = 10000,
};

namespace DataAverage
{
	void Set(const std::wstring& name, const float value, const Average average);
	float Get(const std::wstring& name);
};