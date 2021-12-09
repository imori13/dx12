#include "GameCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "TestModel.h"
#include "WinApp.h"
#include "Command.h"
#include "App_ImGui.h"
#include "Timer.h"
#include "ResourceManager.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update(float deltaT) override;
	void RenderScene(void) override;
private:
	TestModel model;
	TestModel model2;
};

CREATE_APPLICATION(App, 1280, 720);

void App::Startup(void)
{
	App_ImGui::Initialize();

	ResourceManager::LoadTexture(L"neko.jpg");
	ResourceManager::LoadTexture(L"neko2.jpg");

	ResourceManager::LoadShader(L"SimpleTexVS.cso");
	ResourceManager::LoadShader(L"SimpleTexPS.cso");

	model.OnInit();
	model.SetTexture(L"neko.jpg");
	model2.OnInit();
	model2.SetTexture(L"neko2.jpg");

	model.m_X = 0.9f;
	model2.m_X = -0.9f;
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();

	model.OnTerm();
	model2.OnTerm();
}

void App::Update(float deltaT)
{
	App_ImGui::Update();

	deltaT++;

	model.m_RotateAngle += 1.f * Timer::g_FrameTime;
	model2.m_RotateAngle += 2.f * Timer::g_FrameTime;

	model.Update();
	model2.Update();
}

void App::RenderScene(void)
{
	using namespace Graphics;
	auto cmdList = Command::Begin(Display::g_FrameIndex);

	EXPECTS(cmdList != nullptr);

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdList->ResourceBarrier(1, &barrier);
	}

	// レンダーターゲットの設定
	const auto rtvHandle = Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	const auto dsvHandle = Display::g_DepthStencilBuffer.at(Display::g_FrameIndex).GetCpuHandle();
	cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// クリアカラー
	const float clearColor[4] = { 0.0f,0.0f,1.0f,1.0f };

	// RTVをクリア
	cmdList->ClearRenderTargetView(rtvHandle, gsl::make_span(clearColor).data(), 0, nullptr);
	// DSVをクリア
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->RSSetViewports(1, &Display::g_Viewport);
	cmdList->RSSetScissorRects(1, &Display::g_Scissor);

	model.Render(cmdList.Get());
	model2.Render(cmdList.Get());

	App_ImGui::Render(cmdList.Get());

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);
	}


	Command::End();

	App_ImGui::UpdateAdditionalPlatformWindows(cmdList.Get());
}
