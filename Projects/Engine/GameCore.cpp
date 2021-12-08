#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameCore.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "Command.h"
#include "TimeStamp.h"

#include <boost/version.hpp>

namespace GameCore
{
	float g_DrawSumT;
	float g_WriteT;
	float g_FrameWaitT;
	float g_GpuWaitT;

	void InitializeApplication(IGameApp& game)
	{
		TimeStamp::Initialize();

		TimeStamp::Begin(L"初期化処理");

		Graphics::Initialize();
		Command::Initialize();
		Display::Initialize();
		game.Startup();

		TimeStamp::End(L"初期化処理");
	}

	bool UpdateApplication(IGameApp& game)
	{
		TimeStamp::Begin(L"描画処理");

		// 更新処理
		{
			TimeStamp::Begin(L"描画書き込み");

			game.Update(0);
		}

		// 描画処理
		{
			game.RenderScene();

			g_WriteT = TimeStamp::End(L"描画書き込み");
		}

		// 画面表示
		{
			TimeStamp::Begin(L"画面表示");

			Display::Present(1);

			g_FrameWaitT = TimeStamp::End(L"画面表示");
		}

		// GPU待機
		{
			TimeStamp::Begin(L"GPU待機");

			Command::MoveToNextFrame();

			g_GpuWaitT = TimeStamp::End(L"GPU待機");
		}

		g_DrawSumT = TimeStamp::End(L"描画処理");

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