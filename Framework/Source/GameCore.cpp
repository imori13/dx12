#include "GameCore.h"
#include <assert.h>


HWND GameCore::m_hWnd = nullptr;

int GameCore::Run(D3DApp* app, HINSTANCE hInst, int nCmdShow)
{
	// ウィンドウクラスの設定
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInst;
	windowClass.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszClassName = L"DXClass";
	RegisterClassEx(&windowClass);

	// ウィンドウサイズの設定
	RECT windowRect = { 0,0,static_cast<LONG>(app->GetWidth()),static_cast<LONG>(app->GetHeight()) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウの生成
	m_hWnd = CreateWindow(
		windowClass.lpszClassName,
		app->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInst,
		nullptr);

	//InitializeApplication(app);

	// ウィンドウの表示
	ShowWindow(m_hWnd, nCmdShow);

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
	} while(true/*UpdateApplication(app)*/);	// falseでループ離脱

	//TerminateApplication(app);
	//Graphics::Shutdown();
	return 0;
}

LRESULT CALLBACK GameCore::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}