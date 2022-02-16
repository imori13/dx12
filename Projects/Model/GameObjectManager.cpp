#include "GameObjectManager.h"

void GameObjectManager::Initialize()
{
	m_GameObjects.clear();
}

void GameObjectManager::Update()
{
	for(const auto& gameObject : m_GameObjects)
	{
		gameObject->Update();
	}
}

void GameObjectManager::Draw()
{
	for(const auto& gameObject : m_GameObjects)
	{
		gameObject->Draw();
	}
}

void GameObjectManager::AddObject(const std::shared_ptr<GameObject>& gameObject)
{
	m_GameObjects.push_back(gameObject);
}
