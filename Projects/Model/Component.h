#pragma once
#include "GameObject.h"

class GameObject;

class Component
{
public:
	Component() {}
	virtual ~Component() {}

	virtual void Start() {}
	virtual void Update() {}
	virtual void Draw() {}
private:
	GameObject* m_GameObject;
};