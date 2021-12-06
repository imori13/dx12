#include "Window.h"
#include "..\Engine\Debug.h"
#include <GSL\gsl>
#include <boost/format.hpp>

namespace UIEditor
{
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) noexcept;

	void Window::Create(const std::wstring_view windowName, HWND parentHWND, const uint32_t width, const uint32_t height, const bool isChild)
	{
		m_hInst = ::GetModuleHandle(nullptr);
		m_Width = width;
		m_Height = height;
		m_WindowName = windowName.data();

		int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, m_WindowName.c_str(), -1, (char*)NULL, 0, NULL, NULL);
		CHAR* cpMultiByte = new CHAR[iBufferSize];
		WideCharToMultiByte(CP_OEMCP, 0, m_WindowName.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL);
		std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
		delete[] cpMultiByte;

		// ウィンドウの設定
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = m_hInst;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = oRet.c_str();

		// ウィンドウの登録
		bool flag = RegisterClassEx(&windowClass);
		ENSURES(flag, "Window登録");

		const uint64_t windowStyle = (isChild) ? (WS_CHILD | WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX) : (WS_OVERLAPPEDWINDOW);

		// ウィンドウサイズを調整
		RECT rc = { 0, 0, gsl::narrow<LONG>(m_Width), gsl::narrow<LONG>(m_Height) };
		flag = AdjustWindowRect(&rc, windowStyle, FALSE);
		ENSURES(flag, "WindowRect調整");

		// ウィンドウを生成
		m_hWnd = CreateWindowW(m_WindowName.c_str(), m_WindowName.c_str(), windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
							   rc.right - rc.left, rc.bottom - rc.top, parentHWND, nullptr, m_hInst, nullptr);
		ENSURES(m_hWnd != nullptr, "Window生成");

		// ウィンドウを表示
		constexpr int shorParam = SW_SHOW;
		flag = ShowWindow(gsl::make_not_null(m_hWnd), shorParam);

		// ウィンドウを更新
		flag = UpdateWindow(gsl::make_not_null(m_hWnd));
		ENSURES(flag, "Window更新");

		// ウィンドウにフォーカスを設定
		SetFocus(m_hWnd);
	}

	bool Window::Update()
	{
		MSG msg = {};
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message == WM_QUIT;
	}

	void Window::Term()
	{
		// ウィンドウの登録を解除
		if(m_hInst != nullptr)
		{ UnregisterClassW(m_WindowName.c_str(), m_hInst); }

		m_hInst = nullptr;
		m_hWnd = nullptr;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch(message)
		{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}
}