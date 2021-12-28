#pragma once
#include "Texture.h"
#include "Model.h"
#include "GraphicsPipeline.h"

namespace ResourceManager
{
	void LoadPipeline(const std::wstring_view pipelineName, GraphicsPipeline pipeline);
	void LoadTexture(const std::wstring_view textureName);
	void LoadMesh(const std::wstring_view modelName);

	GraphicsPipeline GetPipeline(const std::wstring_view pipelineName) noexcept;
	Texture GetTexture(const std::wstring_view texutreName) noexcept;
	Model GetMesh(const std::wstring_view modelName) noexcept;
};