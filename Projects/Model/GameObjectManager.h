#pragma once
#include "GameObject.h"
#include "Renderer.h"

class GameObjectManager
{
public:
	GameObjectManager() {};

	void Initialize();
	void Update();
	void Draw();

	void AddObject(const std::shared_ptr<GameObject>& gameObject);

private:
	std::list<std::shared_ptr<GameObject>> m_GameObjects;
};

