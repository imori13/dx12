#include "GameCore.h"

class TestScene : public GameCore::IGameApp
{
public:
	TestScene() {}
	~TestScene() {}

	virtual void Startup(void) override;
	virtual void Cleanup(void) override;
	virtual void Update(float deltaT) override;
	virtual void RenderScene(void) override;
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
}
