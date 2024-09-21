#pragma once
#include "Component.h"

// https://qiita.com/harayuu10/items/bf6d73353efa45212200

class Component;

class GameObject
{
public:
	GameObject() noexcept {};
	~GameObject()
	{

	};

	void Update();
	void Draw();

	template<class T> T* AddComponent()
	{
		T* buff = new T();

		m_ComponentList.push_back(buff);

		// 親を指定
		buff->Parent = this;
		buff->Start();

		return buff;
	}

	template<class T> T* GetComponent()
	{
		for(auto& component : m_ComponentList)
		{
			T* buff = dynamic_cast<T*>(component);

			if(buff != nullptr)
			{
				// GetComponent 成功
				return buffer;
			}
		}

		// GetComponent 失敗
		return nullptr;
	}

private:
	std::list<Component*> m_ComponentList;
};