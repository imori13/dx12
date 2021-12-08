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

		LOGLINE("����������-> %.2fms", TimeStamp::End());
	}

	bool UpdateApplication(IGameApp& game)
	{
		TimeStamp::Begin();

		// �X�V����
		{
			TimeStamp::Begin();

			game.Update(0);

			DataAverage::Set(L"Update", TimeStamp::End(), Average::Middle);
		}

		// �`�揈��
		{
			TimeStamp::Begin();

			game.RenderScene();

			DataAverage::Set(L"Render", TimeStamp::End(), Average::Middle);
		}

		// ��ʕ\��
		{
			TimeStamp::Begin();

			Display::Present(0);

			DataAverage::Set(L"Present", TimeStamp::End(), Average::Middle);
		}

		// Frame�ҋ@
		{
			TimeStamp::Begin();

			Timer::Update(g_IsSync);

			DataAverage::Set(L"SyncHz", TimeStamp::End(), Average::Middle);
		}

		// GPU�ҋ@
		{
			TimeStamp::Begin();

			Command::MoveToNextFrame();

			DataAverage::Set(L"GPUwait", TimeStamp::End(), Average::Middle);
		}

		DataAverage::Set(L"�X�V����", TimeStamp::End(), Average::Middle);

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
		// ���������[�N�`�F�b�N
#if defined(DEBUG) || defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(266);
#endif
		// �E�B���h�E������
		Window::InitWnd(hInstance, width, height);

		// �A�v��������
		InitializeApplication(game);

		LOGLINE("������ ���������� ������");

		do
		{
			// �E�B���h�E�X�V
			const auto flag = Window::Update();

			// �E�B���h�E��false�ŏI��
			if(flag) { break; }

			// �A�v���̍X�V
		} while(UpdateApplication(game));

		// �A�v���I��
		TerminateApplication(game);
		// �E�B���h�E�I��
		Window::TermWnd();

		LOGLINE("������ �I������ ������");

		return 0;
	}
}