#include "pch.h"
#include "GameCore.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "TestModel.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) noexcept override;
	void Update(float deltaT) override;
	void RenderScene(void) override;
private:
	TestModel model;
	TestModel model2;
};

CREATE_APPLICATION(App, 1280, 720);

void App::Startup(void)
{
	model.OnInit();
	model2.OnInit();
}

void App::Cleanup(void) noexcept
{
	model.OnTerm();
	model2.OnTerm();
}

void App::Update(float deltaT)
{
	deltaT++;

	model.Update();
	model2.Update();
}

void App::RenderScene(void)
{
	using namespace Graphics;
	auto cmdList = g_Command.Begin(Display::g_FrameIndex);

	Expects(cmdList != nullptr);

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

	model.Render(cmdList.Get());
	model2.Render(cmdList.Get());

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);
	}

	g_Command.Finish();
}
