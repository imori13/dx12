#include "WinApp.h"

namespace
{
	HINSTANCE g_hInst = nullptr;
	const wchar_t* g_windowName = nullptr;
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

		g_hInst = hInstance;
		g_Width = width;
		g_Height = height;
		g_windowName = L"iMoriEngine";

		// �E�B���h�E�̐ݒ�
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = g_windowName;

		// �E�B���h�E�̓o�^
		bool flag = RegisterClassEx(&windowClass);
		ENSURES(flag, "Window�o�^");

		// �E�B���h�E�T�C�Y�𒲐�
		RECT rc = { 0, 0, gsl::narrow<LONG>(g_Width), gsl::narrow<LONG>(g_Height) };
		flag = AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		ENSURES(flag, "WindowRect����");

		// �E�B���h�E�𐶐�
		g_hWnd = CreateWindowW(g_windowName, g_windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							   rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
		ENSURES(g_hWnd != nullptr, "Window����");

		// �E�B���h�E��\��
		constexpr int shorParam = SW_SHOW;
		flag = ShowWindow(gsl::make_not_null(g_hWnd), shorParam);
		ENSURES(flag == false, "Window�\��");

		// �E�B���h�E���X�V
		flag = UpdateWindow(gsl::make_not_null(g_hWnd));
		ENSURES(flag, "Window�X�V");

		// �E�B���h�E�Ƀt�H�[�J�X��ݒ�
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
		// �E�B���h�E�̓o�^������
		if(g_hInst != nullptr)
		{ UnregisterClass(g_windowName, g_hInst); }

		g_hInst = nullptr;
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