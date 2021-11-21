#include "WindowApp.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

WindowApp::WindowApp(HINSTANCE hInst, int nCmdShow, uint32_t width, uint32_t height)
	: m_Width(width)
	, m_Height(height)
	, m_hWnd(nullptr)
{
	// ウィンドウクラスの設定
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInst;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"Engine";
	RegisterClassEx(&windowClass);

	// ウィンドウサイズの設定
	RECT windowRect = { 0,0,static_cast<LONG>(m_Width),static_cast<LONG>(m_Height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウの生成
	m_hWnd = CreateWindow(
		windowClass.lpszClassName,
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInst,
		nullptr);

	// ウィンドウの表示
	ShowWindow(m_hWnd, nCmdShow);

	// ウィンドウのメッセージループ
	MSG msg = {};
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Return this part of the WM_QUIT message to Windows.
	//return static_cast<char>(msg.wParam);
}

WindowApp::~WindowApp()
{
}

uint32_t WindowApp::GetWidth() const
{ return m_Width; }

uint32_t WindowApp::GetHeight() const
{ return m_Height; }

HWND WindowApp::GetHWND() const
{ return m_hWnd; }
