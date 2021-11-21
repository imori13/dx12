#pragma once

#include <stdint.h>
#include <windows.h>

class WindowApp
{
public:
	WindowApp(HINSTANCE hInst, int nCmdShow, uint32_t width, uint32_t height);
	~WindowApp();

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	HWND GetHWND() const;
private:
	uint32_t m_Width;
	uint32_t m_Height;
	HWND m_hWnd;
};

