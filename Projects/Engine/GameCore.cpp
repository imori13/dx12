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
#include "Timer.h"
#include "DataAverage.h"

#include <boost/version.hpp>

namespace GameCore
{
	bool g_IsSync = true;

	void InitializeApplication(IGameApp& game)
	{
		Timer::Initialize();
		TimeStamp::Initialize();

		TimeStamp::Begin();

		Graphics::Initialize();
		Command::Initialize();
		Display::Initialize();
		game.Startup();

		LOGLINE("初期化時間-> %.2fms", TimeStamp::End());
	}

	bool UpdateApplication(IGameApp& game)
	{
		TimeStamp::Begin();

		// 更新処理
		{
			TimeStamp::Begin();

			game.Update(0);

			DataAverage::Set(L"Update", TimeStamp::End(), Average::Middle);
		}

		// 描画処理
		{
			TimeStamp::Begin();

			game.RenderScene();

			DataAverage::Set(L"Render", TimeStamp::End(), Average::Middle);
		}

		// 画面表示
		{
			TimeStamp::Begin();

			Display::Present(0);

			DataAverage::Set(L"Present", TimeStamp::End(), Average::Middle);
		}

		// Frame待機
		{
			TimeStamp::Begin();

			Timer::Update(g_IsSync);

			DataAverage::Set(L"SyncHz", TimeStamp::End(), Average::Middle);
		}

		// GPU待機
		{
			TimeStamp::Begin();

			Command::MoveToNextFrame();

			DataAverage::Set(L"GPUwait", TimeStamp::End(), Average::Middle);
		}

		DataAverage::Set(L"更新時間", TimeStamp::End(), Average::Middle);

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