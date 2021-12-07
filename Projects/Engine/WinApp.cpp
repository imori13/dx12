#include "WinApp.h"

namespace
{
	HINSTANCE s_hInst = nullptr;
	std::wstring s_windowName;
}

namespace Window
{
	uint32_t g_Width = 0;
	uint32_t g_Height = 0;
	HWND g_hWnd = nullptr;

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM) noexcept;

	void InitWnd(HINSTANCE hInstance, uint32_t width, uint32_t height)
	{
		if(hInstance != nullptr)
			hInstance = ::GetModuleHandle(nullptr);

		s_hInst = hInstance;
		g_Width = width;
		g_Height = height;
		s_windowName = L"iMoriEngine";

		// ウィンドウの設定
		WNDCLASSEXW windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = s_windowName.c_str();

		// ウィンドウの登録
		bool flag = RegisterClassExW(&windowClass);
		ENSURES(flag, "Window登録");

		// ウィンドウサイズを調整
		RECT rc = { 0, 0, gsl::narrow<LONG>(g_Width), gsl::narrow<LONG>(g_Height) };
		flag = AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		ENSURES(flag, "WindowRect調整");

		// ウィンドウを生成
		g_hWnd = CreateWindowW(s_windowName.c_str(), s_windowName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							   rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
		ENSURES(g_hWnd != nullptr, "Window生成");

		// ウィンドウを表示
		constexpr int shorParam = SW_SHOW;
		flag = ShowWindow(gsl::make_not_null(g_hWnd), shorParam);
		ENSURES(flag == false, "Window表示");

		// ウィンドウを更新
		flag = UpdateWindow(gsl::make_not_null(g_hWnd));
		ENSURES(flag, "Window更新");

		// ウィンドウにフォーカスを設定
		SetFocus(g_hWnd);
	}

	bool Update() noexcept
	{
		MSG msg = {};
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message == WM_QUIT;
	}

	void TermWnd() noexcept
	{
		// ウィンドウの登録を解除
		if(s_hInst != nullptr)
		{ UnregisterClassW(s_windowName.c_str(), s_hInst); }

		s_hInst = nullptr;
		g_hWnd = nullptr;
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