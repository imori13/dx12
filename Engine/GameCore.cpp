#include "GameCore.h"
#include "WinApp.h"
#include "HogeApp.h"

namespace GameCore
{
	HogeApp app = HogeApp();

	void InitializeApplication()
	{
		app.InitApp();
	}

	bool UpdateApplication()
	{
		app.Render();

		return true;
	}

	void TerminateApplication()
	{
		app.TermApp();
	}

	int RunApplication(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height)
	{
		// ウィンドウ初期化
		auto hr = Window::WinApp::InitWnd(hInstance, nCmdShow, width, height);
		if(FAILED(hr))
		{ return false; }

		// アプリ初期化
		InitializeApplication();

		do
		{
			// ウィンドウ更新
			auto flag = Window::WinApp::Update();

			// ウィンドウがfalseで終了
			if(flag) { break; }

			// アプリの更新
		} while(UpdateApplication());

		// アプリ終了
		TerminateApplication();
		// ウィンドウ終了
		Window::WinApp::TermWnd();

		return 0;
	}
}