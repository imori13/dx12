#pragma once
#include "Texture.h"
#include "Model.h"
#include "Pipeline.h"

namespace ResourceManager
{
	void LoadPipeline(const std::wstring_view pipelineName, Pipeline pipeline);
	void LoadTexture(const std::wstring_view textureName);
	void LoadMesh(const std::wstring_view modelName);
	void LoadSkinMesh(const std::wstring_view modelName);

	Pipeline GetPipeline(const std::wstring_view pipelineName);
	Texture GetTexture(const std::wstring_view texutreName);
	Model GetMesh(const std::wstring_view modelName);
	SkeletonModel GetSkinMesh(const std::wstring_view modelName);
};