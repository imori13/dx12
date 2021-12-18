#include "GameCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"
#include "TestModel.h"
#include "WinApp.h"
#include "Command.h"
#include "App_ImGui.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"
#include "ObjLoader.h"
#include "Renderer.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update(float deltaT) override;
	void RenderScene(void) override;
private:
	//TestModel model;
	//TestModel model2;
};

CREATE_APPLICATION(App, 1600, 900);

void App::Startup(void)
{
	App_ImGui::Initialize();

	std::wstring path = L"Textures/";
	//ResourceManager::LoadTexture(path + L"pixel.png");
	ResourceManager::LoadTexture(path + L"neko.jpg");
	//ResourceManager::LoadTexture(path + L"neko2.jpg");
	ResourceManager::LoadTexture(path + L"umaru.jpg");
	//ResourceManager::LoadTexture(path + L"spot_texture.png");
	ResourceManager::LoadTexture(path + L"gf_g36_dif_04.png");

	path = L"Models/";
	ResourceManager::LoadMesh(path + L"umaru.obj");
	ResourceManager::LoadMesh(path + L"Cube.obj");
	//ResourceManager::LoadMesh(path + L"Monkey.obj");
	//ResourceManager::LoadMesh(path + L"spot_triangulated.obj");
	ResourceManager::LoadMesh(path + L"g36.obj");

	ResourceManager::LoadShader(L"iMoriDefaultVS.cso");
	ResourceManager::LoadShader(L"iMoriDefaultPS.cso");

	PipelineInitializer::Initialize(L"iMoriDefaultVS.cso", L"iMoriDefaultPS.cso");

	Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg");
	Renderer::Load(L"g36", L"g36.obj", L"gf_g36_dif_04.png");
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();

	//model.OnTerm();
	//model2.OnTerm();
}

void App::Update(float deltaT)
{
	App_ImGui::Update();

	deltaT++;
}

void App::RenderScene(void)
{
	auto cmdList = Renderer::Begin();

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
	const float clearColor[4] = { 0.05f,0.05f,0.05f,1.0f };

	// RTVをクリア
	cmdList->ClearRenderTargetView(rtvHandle, gsl::make_span(clearColor).data(), 0, nullptr);
	// DSVをクリア
	cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->RSSetViewports(1, &Display::g_Viewport);
	cmdList->RSSetScissorRects(1, &Display::g_Scissor);

	float timer = Timer::g_ElapsedTime;

	for(auto i = 0u; i < 100; ++i)
	{
		Renderer::Draw(L"umaru",
				   { -1 + (float)cos(timer + i * 0.5f) * i * 0.2f,
				   -1 + (float)sin(timer + i * 0.5f) * i * 0.2f,
				   i * -3.f },
				   { timer + i,timer + i,timer + i },
				   { 0.01f + i * 0.001f,0.01f + i * 0.001f,0.01f + i * 0.001f });
	}

	for(auto i = 0u; i < 5; ++i)
	{
		Renderer::Draw(L"g36", { i - 2.f,0,-10}, {timer + i,timer + i,timer + i}, {5,5,5});
	}

	Renderer::SendCommand(cmdList);

	App_ImGui::Render(cmdList);

	{
		// リソースバリア
		auto barrier = GetTranslationBarrier(Display::g_RenderTargetBuffer.at(Display::g_FrameIndex).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		cmdList->ResourceBarrier(1, &barrier);
	}

	Renderer::End();

	App_ImGui::UpdateAdditionalPlatformWindows(cmdList);
}
