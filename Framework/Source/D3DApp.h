#pragma once
#include <cstdint>
#include <string>
#include <wchar.h>

class D3DApp
{
public:
	D3DApp(uint32_t width, uint32_t height, std::wstring name);
	~D3DApp();

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	const wchar_t* GetTitle() const { return m_Title.c_str(); }
private:
	uint32_t m_Width;
	uint32_t m_Height;
	std::wstring m_Title;
};

