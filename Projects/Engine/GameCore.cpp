#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "Debug.h"
#include "GameCore.h"
#include "WinApp.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "Command.h"
#include "TimeStamp.h"
#include "Timer.h"
#include "DataAverage.h"
#include "Input.h"

namespace GameCore
{
	bool g_IsSync = true;

	void InitializeApplication(IGameApp& game)
	{
		omp_set_num_threads(8);
		Eigen::setNbThreads(1);
		Eigen::initParallel();
		omp_set_nested(1);
		omp_set_dynamic(256);

		Input::Initialize();
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
		Input::Update();
		Timer::Update();

		DataAverage::Set(L"FPS", Timer::g_FrameTime, Average::Low);

		TimeStamp::Begin();

		game.UpdateGUI();

		// 更新処理
		{
			TimeStamp::Begin();

			game.Update();

			DataAverage::Set(L"Update", TimeStamp::End(), Average::VeryLow);
		}

		// 描画処理
		{
			TimeStamp::Begin();

			game.RenderScene();

			DataAverage::Set(L"Render", TimeStamp::End(), Average::VeryLow);
		}

		// 画面表示
		{
			TimeStamp::Begin();

			Display::Present(g_IsSync);

			DataAverage::Set(L"Present", TimeStamp::End(),Average::VeryLow);
		}

		// GPU待機
		{
			TimeStamp::Begin();

			game.RenderGUI();
			Command::MoveToNextFrame();

			DataAverage::Set(L"GPUwait", TimeStamp::End(), Average::VeryLow);
		}

		DataAverage::Set(L"更新時間", TimeStamp::End(), Average::VeryLow);

		return !game.IsDone();
	}

	void TerminateApplication(IGameApp& game)
	{
		Command::WaitForGpu();

		Input::Terminate();
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

		LOGLINE("■■■ 初期化 開始 ■■■");

		// ウィンドウ初期化
		WinApp::InitWnd(hInstance, width, height);

		// アプリ初期化
		InitializeApplication(game);

		LOGLINE("■■■ 初期化 完了 ■■■");

		do
		{
			// ウィンドウ更新
			const auto flag = WinApp::Update();

			// ウィンドウがfalseで終了
			if(flag) { break; }

			// アプリの更新
		} while(UpdateApplication(game));
		
		LOGLINE("■■■ 終了処理 開始 ■■■");
		// アプリ終了
		TerminateApplication(game);
		// ウィンドウ終了
		WinApp::TermWnd();
		LOGLINE("■■■ 終了処理 完了 ■■■");

		return 0;
	}
}