#include "GameCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "TestModel.h"
#include "WinApp.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(Window::g_hWnd);
	ImGui_ImplDX12_Init(Graphics::g_pDevice.Get(), FRAME_COUNT,
						DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Graphics::g_ResourceHeap.Get(),
						Graphics::g_ResourceHeap.GetCPUHandle(0),
						Graphics::g_ResourceHeap.GetGPUHandle(0));

	std::wstring path = L"/Resources/Textures/";
	model.OnInit(path + L"neko.jpg");
	model2.OnInit(path + L"neko2.jpg");
}

void App::Cleanup(void)
{
	model.OnTerm();
	model2.OnTerm();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void App::Update(float deltaT)
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	deltaT++;

	model.m_RotateAngle += 0.025f;
	model2.m_RotateAngle += 0.010f;

	model.Update();
	model2.Update();
}

void App::RenderScene(void)
{
	ImGui::Render();

	using namespace Graphics;
	auto cmdList = g_Command.Begin(Display::g_FrameIndex);

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

	model.Render(cmdList.Get());
	model2.Render(cmdList.Get());

	cmdList->SetDescriptorHeaps(1, Graphics::g_ResourceHeap.GetHeapAddress());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);
	}

	g_Command.Finish();
}
