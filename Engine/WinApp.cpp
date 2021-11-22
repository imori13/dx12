#include "WinApp.h"

namespace MyWindow
{
	uint32_t g_Width = 0;
	uint32_t g_Height = 0;
	HWND g_hWnd = nullptr;
	const wchar_t* g_windowName = nullptr;

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	bool Create(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		g_Width = width;
		g_Height = height;
		g_windowName = L"ƒ^ƒCƒgƒ‹";

		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = g_windowName;

		if(!RegisterClassEx(&windowClass))
		{ return false; }

		RECT rc = { 0, 0, static_cast<LONG>(g_Width), static_cast<LONG>(g_Height) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		g_hWnd = CreateWindow(g_windowName, g_windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							  rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

		if(!g_hWnd)
		{ return false; }

		ShowWindow(g_hWnd, nCmdShow);
		UpdateWindow(g_hWnd);

		return true;
	}

	bool Update()
	{
		MSG msg = {};
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message == WM_QUIT;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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