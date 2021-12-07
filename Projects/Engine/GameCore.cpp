#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameCore.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "Command.h"

#include <boost/version.hpp>

namespace GameCore
{
	void InitializeApplication(IGameApp& game)
	{
		Graphics::Initialize();
		Command::Initialize();
		Display::Initialize();

		game.Startup();
	}

	bool UpdateApplication(IGameApp& game)
	{
		game.Update(0);
		game.RenderScene();

		Display::Present(1);
		Command::MoveToNextFrame();

		return !game.IsDone();
	}

	void TerminateApplication(IGameApp& game)
	{
		Command::WaitForGpu();

		game.Cleanup();
		Display::Terminate();
		Graphics::Terminate();
	}

	bool IGameApp::IsDone(void) noexcept
	{
		return false;
	}

	int RunApplication(IGameApp& game, HINSTANCE hInstance, uint32_t width, uint32_t height)
	{
		// メモリリークチェック
#if defined(DEBUG) || defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(266);
#endif
		// ウィンドウ初期化
		Window::InitWnd(hInstance, width, height);

		// アプリ初期化
		InitializeApplication(game);

		LOGLINE("■■■ 初期化完了 ■■■");

		do
		{
			// ウィンドウ更新
			const auto flag = Window::Update();

			// ウィンドウがfalseで終了
			if(flag) { break; }

			// アプリの更新
		} while(UpdateApplication(game));

		// アプリ終了
		TerminateApplication(game);
		// ウィンドウ終了
		Window::TermWnd();

		LOGLINE("■■■ 終了処理 ■■■");

		return 0;
	}
}