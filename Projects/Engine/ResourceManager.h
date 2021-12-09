#pragma once
#include "Texture.h"

#include <map>

namespace ResourceManager
{
	void LoadTexture(const std::wstring_view textureName);
	void LoadShader(const std::wstring_view shaderName);

	const Texture& GetTexture(const std::wstring_view texutreName);
	ID3DBlob* GetShader(const std::wstring_view shaderName);
};