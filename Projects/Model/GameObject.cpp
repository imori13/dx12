#include "GameObject.h"

void GameObject::Update()
{
	for(auto& component : m_ComponentList)
	{
		component->Update();
	}
}

void GameObject::Draw()
{
	for(auto& component : m_ComponentList)
	{
		component->Draw();
	}
}
