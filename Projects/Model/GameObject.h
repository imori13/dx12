#pragma once
#include "Transform.h"
#include "Matrix4x4.h"

class GameObject
{
public:
	GameObject() noexcept {};
	virtual ~GameObject() {};

	GameObject(const GameObject&) = delete;
	GameObject(GameObject&&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject& operator=(GameObject&&) = delete;

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	//void Draw(Renderer renderer);	// ‚É‚µ‚½‚¢
//
//public:
//	Transform transfrom;

public:
	Matrix4x4* transform;
};