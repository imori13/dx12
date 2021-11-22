#include "GameCore.h"


namespace GameCore
{
	void InitializeApplication(IGameApp& game)
	{
	}

	void TerminateApplication(IGameApp& game)
	{
	}

	bool UpdateApplication(IGameApp& game)
	{

		return 0;
	}

	bool IGameApp::IsDone(void)
	{

		return 0;
	}

	HWND g_hWnd = nullptr;
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	int RunApplication(IGameApp& app, const wchar_t* className, HINSTANCE hInst, int nCmdShow)
	{
		// ウィンドウクラスの設定
		WNDCLASSEX windowClassEX = { 0 };
		windowClassEX.cbSize = sizeof(WNDCLASSEX);
		windowClassEX.style = CS_HREDRAW | CS_VREDRAW;
		windowClassEX.lpfnWndProc = WndProc;
		windowClassEX.hInstance = hInst;
		windowClassEX.hIcon = LoadIcon(hInst, IDI_APPLICATION);
		windowClassEX.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClassEX.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClassEX.lpszMenuName = nullptr;
		windowClassEX.lpszClassName = className;
		RegisterClassEx(&windowClassEX);

		// ウィンドウサイズの設定
		RECT rc = { 0,0,static_cast<LONG>(1270),static_cast<LONG>(720) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		// ウィンドウの生成
		g_hWnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							  rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);

		ASSERT(g_hWnd != 0);

		InitializeApplication(app);

		// ウィンドウの表示
		ShowWindow(g_hWnd, nCmdShow);

		// ウィンドウのメッセージループ
		do
		{
			MSG msg = {};
			while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if(msg.message == WM_QUIT)
				break;
		} while(UpdateApplication(app));	// falseでループ離脱

		TerminateApplication(app);
		//Graphics::Shutdown();

		return 0;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
			//case WM_SIZE:
			//    Display::Resize((UINT)(UINT64)lParam & 0xFFFF, (UINT)(UINT64)lParam >> 16);
			//    break;

			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}
}