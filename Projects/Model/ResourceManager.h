#pragma once
#include "Texture.h"
#include "Model.h"


namespace ResourceManager
{
	void LoadTexture(const std::wstring_view textureName);
	void LoadMesh(const std::wstring_view modelName);

	const Texture& GetTexture(const std::wstring_view texutreName);	
	const Model& GetMesh(const std::wstring_view modelName);
};