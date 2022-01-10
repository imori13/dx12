#include "ResourceManager.h"
#include "File.h"
#include "AssimpTest.h"
#include "Debug.h"

#include <unordered_map>

namespace
{
	std::unordered_map<std::wstring, Texture> s_Textures;
	std::unordered_map<std::wstring, Pipeline> s_Pipelines;
	std::unordered_map<std::wstring, Model> s_AssimpModels;
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

		// 存在するなら無視
		if(s_Textures.find(path.FileName) != s_Textures.end())
		{
			LOGLINE(L"WARNING : 重複キー skip load texture [%s]", path.FileName.c_str());
			return;
		}

		s_Textures.try_emplace(path.FileName);
		auto& texture = s_Textures.at(path.FileName);

		if(path.Extension == L".tga")
			texture.CreateTGA(path.RelativePath);
		else
			texture.CreateWIC(path.RelativePath);
	}

	void LoadMesh(const std::wstring_view modelName)
	{
		const auto& path = File::LoadPath(modelName);

		s_AssimpModels.try_emplace(path.FileName);
		auto& model = s_AssimpModels.at(path.FileName);

		bool flag = false;

		flag = AssimpTest::LoadMesh(&model, path.RelativePath);
		ENSURES(flag, L"モデル読み込み [%s]", path.RelativePath);

		for(const auto& mat : model.ModelMaterials)
		{
			const auto& texturepath = path.ParentPath + mat.DiffuseMap;
			if(!File::Exists(texturepath))
			{
				LOGLINE(L"WARNING : model[%s] に紐づけられたtexture[%s] が見つかりませんでした。",
						path.RelativePath,
						texturepath);
				return;
			}

			if(!mat.DiffuseMap.empty())
			{ LoadTexture(texturepath); }
		}
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
}