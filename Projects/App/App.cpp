#include "GameCore.h"
#include "Display.h"
#include "Command.h"
#include "App_ImGui.h"
#include "ResourceManager.h"
#include "PipelineInitializer.h"
#include "Renderer.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Timer.h"
#include "Input.h"
#include "Random.h"

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update(float deltaT) override;
	void RenderScene(void) override;

	Camera camera;
	Matrix4x4 world;

	std::vector<Vector3> positionVector;
};

CREATE_APPLICATION(App, 1600, 900);

void App::Startup(void)
{
	App_ImGui::Initialize();

	std::wstring path = L"Textures/";
	ResourceManager::LoadTexture(path + L"pixel.png");
	ResourceManager::LoadTexture(path + L"neko.jpg");
	ResourceManager::LoadTexture(path + L"neko2.jpg");
	ResourceManager::LoadTexture(path + L"umaru.jpg");

	path = L"Models/";
	ResourceManager::LoadMesh(path + L"umaru.obj");
	ResourceManager::LoadMesh(path + L"Cube.obj");

	ResourceManager::LoadShader(L"iMoriDefaultVS.cso");
	ResourceManager::LoadShader(L"iMoriDefaultPS.cso");

	PipelineInitializer::Initialize(L"iMoriDefaultVS.cso", L"iMoriDefaultPS.cso");

	//Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg");

	camera.Create(90, 0.01f, 1000.0f);

	constexpr uint32_t count = 90000;
	constexpr int32_t range = 100;
	constexpr int32_t min = -range;
	constexpr int32_t max = +range;
	positionVector.reserve(count);
	Random::Set(min, max);
	for(auto i = 0u; i < count; ++i)
	{
		positionVector.emplace_back(Vector3(Random::Next(), Random::Next(), Random::Next()));
	}

	Renderer::Load(L"Cube", L"Cube.obj", L"neko.jpg", count);
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();
}

void App::Update(float deltaT)
{
	deltaT++;	// Œx‰ñ”ð—p

	App_ImGui::Update();

	camera.Update();

	static Vector3 position(0, -1, 0);
	//static Vector3 scale(0.02f);
	static Vector3 scale(1);
	static Vector3 rotation(0, 1, 0);

	world = Matrix4x4::Identity();
	world *= Matrix4x4::Scale(scale);
	world *= Matrix4x4::RotateAxis(rotation, Timer::g_ElapsedTime * 1.0f);
	//world *= Matrix4x4::Translate(position);

	for(const auto& pos : positionVector)
	{
		Renderer::Draw(L"Cube", world * Matrix4x4::Translate(pos), camera);
	}
	//Renderer::Draw(L"umaru", world* Matrix4x4::Translate(Vector3(5, -1, 0)), camera);
	//Renderer::Draw(L"umaru", world * Matrix4x4::Translate(Vector3(-5, -1, 0)), camera);
	//Renderer::Draw(L"umaru", world * Matrix4x4::Translate(Vector3(0, -1, 5)), camera);
	//Renderer::Draw(L"umaru", world * Matrix4x4::Translate(Vector3(0, -1, -5)), camera);
	//Renderer::Draw(L"umaru", world * Matrix4x4::Translate(Vector3(0, 4, 0)), camera);
	//Renderer::Draw(L"umaru", world * Matrix4x4::Translate(Vector3(0, -6, 0)), camera);

	//App_ImGui::Begin("Parameter");
	//App_ImGui::DragSlider("camera pos", &camera.m_Position);
	//App_ImGui::DragSlider("camera rot", &camera.m_Rotation);
	//App_ImGui::DragSlider("obj pos", &position);
	//App_ImGui::DragSlider("obj rot", &rotation);
	//App_ImGui::DragSlider("obj sca", &scale);

	//rotation.Normalize();

	//App_ImGui::End();
}

void App::RenderScene(void)
{
	auto cmdList = Renderer::Begin();

	Renderer::SendCommand(cmdList);
	App_ImGui::Render(cmdList);
	Renderer::End(cmdList);

	App_ImGui::UpdateAdditionalPlatformWindows(cmdList);
}
