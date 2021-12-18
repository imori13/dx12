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
};

CREATE_APPLICATION(App, 1600, 900);

void App::Startup(void)
{
	App_ImGui::Initialize();

	ResourceManager::LoadTexture(L"Models/ArmoredMaiden/body_tex.tga");
	ResourceManager::LoadTexture(L"Models/ArmoredMaiden/equipment_tex.tga");
	ResourceManager::LoadTexture(L"Models/ArmoredMaiden/face_tex.tga");
	ResourceManager::LoadTexture(L"Models/ArmoredMaiden/hair_tex.tga");
	ResourceManager::LoadTexture(L"Models/ArmoredMaiden/pistol_tex.tga");
	ResourceManager::LoadTexture(L"Models/ArmoredMaiden/sword_tex.tga");

	std::wstring path = L"Textures/";
	ResourceManager::LoadTexture(path + L"pixel.png");
	ResourceManager::LoadTexture(path + L"neko.jpg");
	ResourceManager::LoadTexture(path + L"neko2.jpg");
	ResourceManager::LoadTexture(path + L"umaru.jpg");
	ResourceManager::LoadTexture(path + L"gf_g36_dif_04.png");

	path = L"Models/";
	ResourceManager::LoadMesh(path + L"ArmoredMaiden/ArmoredMaiden.fbx");
	ResourceManager::LoadMesh(path + L"umaru.obj");
	ResourceManager::LoadMesh(path + L"Cube.obj");
	//ResourceManager::LoadMesh(path + L"g36.obj");
	ResourceManager::LoadMesh(path + L"005.obj");

	ResourceManager::LoadShader(L"iMoriDefaultVS.cso");
	ResourceManager::LoadShader(L"iMoriDefaultPS.cso");

	PipelineInitializer::Initialize(L"iMoriDefaultVS.cso", L"iMoriDefaultPS.cso");

	//Renderer::Load(L"ArmoredMaiden", L"ArmoredMaiden.fbx", L"body_tex.tga");
	Renderer::Load(L"ArmoredMaiden", L"ArmoredMaiden.fbx");
	//Renderer::Load(L"005", L"005.obj", L"pixel.png");
	//Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg");
	//Renderer::Load(L"g36", L"g36.obj", L"gf_g36_dif_04.png");
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();
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

	//Renderer::Draw(L"005", { 0,-1,0 }, { (float)Timer::g_ElapsedTime*0.1f,(float)Timer::g_ElapsedTime,(float)Timer::g_ElapsedTime * 0.1f }, { 1,1,1 });
	Renderer::Draw(L"ArmoredMaiden", { 0,-1,0 }, { 0,(float)Timer::g_ElapsedTime * 0.1f,0 }, { 0.01f,0.01f,0.01f });

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
