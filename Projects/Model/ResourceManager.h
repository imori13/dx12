#pragma once
#include "Texture.h"
#include "Model.h"

#include <map>

namespace ResourceManager
{
	void LoadShader(const std::wstring_view shaderName);
	void LoadTexture(const std::wstring_view textureName);
	void LoadObjModel(const std::wstring_view modelName);

	gsl::not_null<ID3DBlob*> GetShader(const std::wstring_view shaderName);
	const Texture& GetTexture(const std::wstring_view texutreName);
	const Model& GetModel(const std::wstring_view modelName);
};