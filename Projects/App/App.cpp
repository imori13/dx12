#include "GameCore.h"
#include "Display.h"
#include "Command.h"
#include "App_ImGui.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Timer.h"
#include "Input.h"
#include "Random.h"
#include "TimeStamp.h"
#include "Pipeline.h"

struct RenderData
{
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
	float rotationSpeed = 0;
};

class App : public GameCore::IGameApp
{
public:
	App() noexcept {};

	void Startup(void) override;
	void Cleanup(void) override;
	void Update() override;
	void UpdateGUI() override;
	void RenderScene(void) override;
	void RenderGUI(void) override;

	Camera camera;

	Matrix4x4 world;
	std::vector<RenderData> renderDataVector;
	std::vector<RenderData> renderDataVector2;
};

CREATE_APPLICATION(App, 1600, 900);

void App::Startup(void)
{
	App_ImGui::Initialize();

	SS_InputLayout inputLayout{};

	inputLayout.AddElement(0, SI_Semantic::POSITION, 0, SI_Format::Float3, SI_Class::Vertex);
	inputLayout.AddElement(0, SI_Semantic::NORMAL, 0, SI_Format::Float3, SI_Class::Vertex);
	inputLayout.AddElement(0, SI_Semantic::TEXCOORD, 0, SI_Format::Float2, SI_Class::Vertex);
	inputLayout.AddElement(0, SI_Semantic::TANGENT, 0, SI_Format::Float3, SI_Class::Vertex);
	inputLayout.AddElement(1, SI_Semantic::TEXCOORD, 1, SI_Format::Float4, SI_Class::Instance);
	inputLayout.AddElement(1, SI_Semantic::TEXCOORD, 2, SI_Format::Float4, SI_Class::Instance);
	inputLayout.AddElement(1, SI_Semantic::TEXCOORD, 3, SI_Format::Float4, SI_Class::Instance);
	inputLayout.AddElement(1, SI_Semantic::TEXCOORD, 4, SI_Format::Float4, SI_Class::Instance);

	PipelineState pipeline;

	{
		PS_ShaderState shaderState;
		shaderState.VertexShader.LoadShader(L"TexVS.cso");
		shaderState.PixelShader.LoadShader(L"TexPS.cso");
		shaderState.InputLayout = inputLayout;
		shaderState.RootSignature.SetSignature(shaderState.VertexShader.GetBlob());

		// Tex
		pipeline.ShaderState = shaderState;
		ResourceManager::LoadPipeline(L"Tex", pipeline.Create());
	}

	{
		PS_ShaderState shaderState;
		shaderState.VertexShader.LoadShader(L"NotTexVS.cso");
		shaderState.PixelShader.LoadShader(L"NotTexPS.cso");
		shaderState.InputLayout = inputLayout;
		shaderState.RootSignature.SetSignature(shaderState.VertexShader.GetBlob());

		// NotTex
		pipeline.ShaderState = shaderState;
		ResourceManager::LoadPipeline(L"NotTex", pipeline.Create());

		// Collider
		pipeline.Rasterizer.SetCulling(false);
		pipeline.Rasterizer.SetSolidMode(false);
		ResourceManager::LoadPipeline(L"Collider", pipeline.Create());
	}

	std::wstring path = L"Textures/";
	ResourceManager::LoadTexture(path + L"pixel.png");
	ResourceManager::LoadTexture(path + L"neko.jpg");
	ResourceManager::LoadTexture(path + L"neko2.jpg");
	ResourceManager::LoadTexture(path + L"umaru.jpg");
	ResourceManager::LoadTexture(path + L"gf_g36_dif_04.png");

	path = L"Models/";
	ResourceManager::LoadMesh(path + L"Cube.obj");
	//ResourceManager::LoadMesh(path + L"umaru.obj");
	//ResourceManager::LoadMesh(path + L"g36.obj");

	//PipelineInitializer::Initialize(L"iMoriDefaultVS.cso", L"iMoriDefaultPS.cso");

	//Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg");

	camera.Create(90, 0.01f, 1000.0f);

	constexpr int64_t count = 250000;
	constexpr int32_t range = 1000;
	constexpr int32_t min = -range;
	constexpr int32_t max = +range;

	renderDataVector.resize(count);
	renderDataVector2.resize(count);

	Random::Set(min, max);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector.at(i).position = Vector3(Random::Next(), Random::Next(), Random::Next());
	Random::Set(-1.0f, 1.0f);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector.at(i).rotation = Vector3(Random::Next(), Random::Next(), Random::Next()).normalized();
	Random::Set(0.1f, 1.5f);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector.at(i).scale = Vector3(Random::Next());
	Random::Set(-5.0f, 5.0f);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector.at(i).rotationSpeed = Random::Next();

	Random::Set(min, max);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector2.at(i).position = Vector3(Random::Next(), Random::Next(), Random::Next());
	Random::Set(-1.0f, 1.0f);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector2.at(i).rotation = Vector3(Random::Next(), Random::Next(), Random::Next()).normalized();
	Random::Set(0.1f, 1.25f);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector2.at(i).scale = Vector3(Random::Next());
	Random::Set(-5.0f, 5.0f);
	for(int64_t i = 0; i < count; ++i)
		renderDataVector2.at(i).rotationSpeed = Random::Next();

	Renderer::Load(L"Cube", L"Cube.obj", L"neko.jpg", count);
	Renderer::Load(L"Cube2", L"Cube.obj", L"neko2.jpg", count);
	//Renderer::Load(L"g36", L"g36.obj", L"gf_g36_dif_04.png", count);
	//Renderer::Load(L"umaru", L"umaru.obj", L"umaru.jpg", count);
}

void App::Cleanup(void)
{
	App_ImGui::Terminate();
}

void App::Update()
{
	camera.Update();

}

void App::UpdateGUI()
{
	App_ImGui::Update();
}

void App::RenderScene(void)
{
	auto cmdList = Renderer::Begin(camera);

	std::vector<Matrix4x4> matrix;
	matrix.resize(renderDataVector.size());

#pragma omp parallel for
	for(auto i = 0; i < matrix.size(); ++i)
	{
		auto& data = renderDataVector.at(i);

		matrix.at(i) = Matrix4x4::identity()
			.scale(data.scale)
			.rotateAxis(Vector3::up(), Timer::g_ElapsedTime * data.rotationSpeed * 0.1f)
			.rotation(data.rotation)
			.translation(data.position);
	}
	Renderer::Draw(cmdList, L"Cube", matrix);

	matrix.resize(renderDataVector2.size());
#pragma omp parallel for
	for(auto i = 0; i < matrix.size(); ++i)
	{
		auto& data = renderDataVector2.at(i);

		matrix.at(i) = Matrix4x4::identity()
			.scale(data.scale)
			.rotateAxis(Vector3::up(), Timer::g_ElapsedTime * data.rotationSpeed * 0.1f)
			.rotation(data.rotation)
			.translation(data.position);
	}
	Renderer::Draw(cmdList, L"Cube2", matrix);

	//constexpr uint32_t g36Count = 1;
	//constexpr float g36half = g36Count / 2.0f;
	//matrix.resize(g36Count);
	//for(auto i = 0u; i < g36Count; ++i)
	//{
	//	matrix.at(i) = Matrix4x4::identity()
	//		.scale(Vector3::one() * 70)
	//		.rotateAxis(Vector3::up(), Timer::g_ElapsedTime * 0.05f)
	//		.translation(Vector3((i - g36half + 0.5f) * 30.0f, -0.5f, 20));
	//}
	//Renderer::Draw(cmdList, L"g36", matrix);

	//constexpr uint32_t umaruCount = 2;
	//constexpr float umaruhalf = umaruCount / 2.0f;
	//matrix.resize(umaruCount);
	//for(auto i = 0u; i < umaruCount; ++i)
	//{
	//	matrix.at(i) = Matrix4x4::identity()
	//		.scale(Vector3::one() * 0.2f)
	//		.rotateAxis(Vector3::up(), Timer::g_ElapsedTime * 0.05f)
	//		.translation(Vector3((i - umaruhalf + 0.5f) * 30.0f, -0.5f, 20));
	//}

	//Renderer::Draw(cmdList, L"umaru", matrix);

	{
		TimeStamp::Stop();
		App_ImGui::Render(cmdList);
		TimeStamp::Resume();
	}

	Renderer::End(cmdList);
}

void App::RenderGUI(void)
{
	App_ImGui::UpdateAdditionalPlatformWindows(Command::GetMainCmdList());
}