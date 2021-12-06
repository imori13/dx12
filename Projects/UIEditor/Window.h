#pragma once
#include <cstdint>
#include <windows.h>
#include <string>

namespace UIEditor
{
	class Window
	{
	public:
		uint32_t m_Width;
		uint32_t m_Height;
		HWND m_hWnd;
		HINSTANCE m_hInst;
		std::wstring m_WindowName;

		void Create(const std::wstring_view windowName, HWND parentHWND, const uint32_t width, const uint32_t height, const bool isChild = false);
		bool Update();
		void Term();
	};
}