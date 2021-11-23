#include "WinApp.h"

namespace Window
{
	uint32_t g_Width = 0;
	uint32_t g_Height = 0;
	HWND g_hWnd = nullptr;

	static HINSTANCE g_hInst=nullptr;
	static const wchar_t* g_windowName = nullptr;

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	bool WinApp::InitWnd(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		g_hInst = hInstance;
		g_Width = width;
		g_Height = height;
		g_windowName = L"�^�C�g��";

		// �E�B���h�E�̐ݒ�
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = g_windowName;

		// �E�B���h�E�̓o�^
		if(!RegisterClassEx(&windowClass))
		{ return false; }

		// �E�B���h�E�T�C�Y�𒲐�
		RECT rc = { 0, 0, static_cast<LONG>(g_Width), static_cast<LONG>(g_Height) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		// �E�B���h�E�𐶐�
		g_hWnd = CreateWindow(g_windowName, g_windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							  rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

		if(!g_hWnd)
		{ return false; }

		// �E�B���h�E��\��
		ShowWindow(g_hWnd, nCmdShow);

		// �E�B���h�E���X�V
		UpdateWindow(g_hWnd);

		// �E�B���h�E�Ƀt�H�[�J�X��ݒ�
		SetFocus(g_hWnd);

		return true;
	}

	bool WinApp::Update()
	{
		MSG msg = {};
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message == WM_QUIT;
	}

	void WinApp::TermWnd()
	{
		// �E�B���h�E�̓o�^������
		if(g_hInst != nullptr)
		{ UnregisterClass(g_windowName, g_hInst); }

		g_hInst = nullptr;
		g_hWnd = nullptr;
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