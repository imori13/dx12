#pragma once
#include "Texture.h"
#include "Model.h"


namespace ResourceManager
{
	void LoadShader(const std::wstring_view shaderName);
	void LoadTexture(const std::wstring_view textureName);
	void LoadMesh(const std::wstring_view modelName);

	gsl::not_null<ID3DBlob*> GetShader(const std::wstring_view shaderName);
	const Texture& GetTexture(const std::wstring_view texutreName);	
	const Model& GetMesh(const std::wstring_view modelName);
};