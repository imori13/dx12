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
		omp_set_num_threads(16);
		omp_set_nested(1);
		omp_set_dynamic(256);

		Eigen::setNbThreads(1);
		Eigen::initParallel();

		Input::Initialize();
		Timer::Initialize();
		TimeStamp::Initialize();

		TimeStamp::Begin();

		Graphics::Initialize();
		Command::Initialize();
		Display::Initialize();
		game.Startup();

		LOGLINE("๚ปิ-> %.2fms", TimeStamp::End());
	}

	bool UpdateApplication(IGameApp& game)
	{
		Input::Update();
		Timer::Update();

		DataAverage::Set(L"FPS", Timer::g_FrameTime, Average::Low);

		TimeStamp::Begin();

		game.UpdateGUI();

		// XV
		{
			TimeStamp::Begin();

			game.Update();

			DataAverage::Set(L"Update", TimeStamp::End(), Average::VeryLow);
		}

		// `ๆ
		{
			TimeStamp::Begin();

			game.RenderScene();

			DataAverage::Set(L"Render", TimeStamp::End(), Average::VeryLow);
		}

		// ๆส\ฆ
		{
			TimeStamp::Begin();

			Display::Present(g_IsSync);

			DataAverage::Set(L"Present", TimeStamp::End(),Average::VeryLow);
		}

		// GPUา@
		{
			TimeStamp::Begin();

			game.RenderGUI();
			Command::MoveToNextFrame();

			DataAverage::Set(L"GPUwait", TimeStamp::End(), Average::VeryLow);
		}

		DataAverage::Set(L"XVิ", TimeStamp::End(), Average::None);

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
		// [N`FbN
#if defined(DEBUG) || defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(266);
#endif

		LOGLINE("กกก ๚ป Jn กกก");

		// EBhE๚ป
		WinApp::InitWnd(hInstance, width, height);

		// Av๚ป
		InitializeApplication(game);

		LOGLINE("กกก ๚ป ฎน กกก");

		do
		{
			// EBhEXV
			const auto flag = WinApp::Update();

			// EBhEชfalseลIน
			if(flag) { break; }

			// AvฬXV
		} while(UpdateApplication(game));
		
		LOGLINE("กกก Iน Jn กกก");
		// AvIน
		TerminateApplication(game);
		// EBhEIน
		WinApp::TermWnd();
		LOGLINE("กกก Iน ฎน กกก");

		return 0;
	}
}