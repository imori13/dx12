﻿#include "ResourceManager.h"
#include "File.h"
#include "ObjLoader.h"
#include "TimeStamp.h"
#include "AssimpTest.h"
#include "Debug.h"

#include <unordered_map>

namespace
{
	std::unordered_map<std::wstring, Texture> s_Textures;
	std::unordered_map<std::wstring, Pipeline> s_Pipelines;
	std::unordered_map<std::wstring, Model> s_AssimpModels;
	std::unordered_map<std::wstring, SkeletonModel> s_SkeletonModels;
}

namespace ResourceManager
{
	void LoadPipeline(const std::wstring_view pipelineName, Pipeline pipeline)
	{
		s_Pipelines.try_emplace(pipelineName.data());
		s_Pipelines.at(pipelineName.data()) = pipeline;
	}

	void LoadTexture(const std::wstring_view textureName)
	{
		const auto& path = File::LoadPath(textureName);

		s_Textures.try_emplace(path.FileName);
		auto& texture = s_Textures.at(path.FileName);

		if(path.Extension == L".tga")
			texture.CreateTGA(path.RelativePath);
		else
			texture.CreateWIC(path.RelativePath);
	}

	void LoadMesh(const std::wstring_view modelName)
	{
		TimeStamp::Begin();

		const auto& path = File::LoadPath(modelName);

		s_AssimpModels.try_emplace(path.FileName);
		auto& model = s_AssimpModels.at(path.FileName);

		bool flag = false;

		//if(path.Extension == L".obj")
		//	flag = ObjLoader::LoadFile(model, path.FileName, path.ParentPath);
		//else
		flag = AssimpTest::LoadMesh(&model, path.RelativePath);

		LOGLINE(L"%s 読み込み[ %s ] 頂点数[ %d ] インデックス数[ %d ]",
				path.FileName.c_str(),
				(flag) ? (L"成功") : (L"失敗"),
				model.ModelMeshes.at(0).Vertices.size(),
				model.ModelMeshes.at(0).Indices.size());

		const auto time = TimeStamp::End();
		LOGLINE(L"モデル[%s]読み込み時間 : %.2fms", path.FileName.c_str(), time);
	}

	void LoadSkinMesh(const std::wstring_view modelName)
	{
		TimeStamp::Begin();

		const auto& path = File::LoadPath(modelName);

		s_SkeletonModels.try_emplace(path.FileName);
		auto& model = s_SkeletonModels.at(path.FileName);

		bool flag = false;

		flag = AssimpTest::LoadSkinMesh(&model, path.RelativePath);

		LOGLINE(L"%s 読み込み[ %s ] 頂点数[ %d ] インデックス数[ %d ]",
				path.FileName.c_str(),
				(flag) ? (L"成功") : (L"失敗"),
				model.ModelMeshes.at(0).Vertices.size(),
				model.ModelMeshes.at(0).Indices.size());

		const auto time = TimeStamp::End();
		LOGLINE(L"モデル[%s]読み込み時間 : %.2fms", path.FileName.c_str(), time);
	}

	Pipeline GetPipeline(const std::wstring_view pipelineName)
	{
		return s_Pipelines.at(pipelineName.data());
	}

	Texture GetTexture(const std::wstring_view texutreName)
	{
		return s_Textures.at(texutreName.data());
	}

	Model GetMesh(const std::wstring_view modelName)
	{
		return s_AssimpModels.at(modelName.data());
	}
	SkeletonModel GetSkinMesh(const std::wstring_view modelName)
	{
		return s_SkeletonModels.at(modelName.data());
	}
}