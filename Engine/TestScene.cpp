#include "GameCore.h"
#include "GraphicsCore.h"
#include "Display.h"
#include "TranslationBarrirUtil.h"

class TestScene : public GameCore::IGameApp
{
public:
	TestScene() {}
	~TestScene() {}

	virtual void Startup(void) override;
	virtual void Cleanup(void) override;
	virtual void Update(float deltaT) override;
	virtual void RenderScene(void) override;
private:
};

CREATE_APPLICATION(TestScene, 1280, 720);

void TestScene::Startup(void)
{
}

void TestScene::Cleanup(void)
{
}

void TestScene::Update(float deltaT)
{
	deltaT++;
}

void TestScene::RenderScene(void)
{
	using namespace Graphics;
	auto cmdList = g_pCmdList;

	// リソースバリアの設定
	auto barrier = GetTranslationBarrier(g_pColorBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// リソースバリア
	cmdList->ResourceBarrier(1, &barrier);

	// レンダーターゲットの設定
	auto handle = g_RtvHeap.GetCpuHandle();
	handle.ptr += (static_cast<uint64_t>(Display::g_FrameIndex)) * g_RtvHeap.GetIncrementSize();
	cmdList->OMSetRenderTargets(1, &handle, FALSE, nullptr);

	// クリアカラー
	float clearColor[] = { 0.0f,0.0f,1.0f,1.0f };

	// レンダーターゲットビューをクリア
	cmdList->ClearRenderTargetView(handle, clearColor, 0, nullptr);

	// リソースバリアの設定
	barrier = GetTranslationBarrier(g_pColorBuffer[Display::g_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// リソースバリア
	cmdList->ResourceBarrier(1, &barrier);
}
