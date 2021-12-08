#pragma once

namespace TimeStamp
{
	void Initialize() noexcept;
	void Begin(const std::wstring& watchName = L"");
	float End(const std::wstring& watchName = L"");
}