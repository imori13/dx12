#include "ResourceManager.h"
#include "File.h"
#include "ObjLoader.h"
#include "TimeStamp.h"
#include "AssimpTest.h"

#include <map>

namespace
{
	std::map<std::wstring, Texture> s_Textures;
	std::map<std::wstring, Model> s_ObjModels;
	std::map<std::wstring, Model> s_AssimpModels;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3DBlob>> s_Shaders;
}

namespace ResourceManager
{
	void LoadShader(const std::wstring_view shaderName)
	{
		const auto& path = File::LoadPath(shaderName);
		auto& shader = s_Shaders[path.FileName];

		const auto hr = D3DReadFileToBlob(path.RelativePath.c_str(), shader.GetAddressOf());
		ENSURES(L"Shader読み込み [ %s ]", path.RelativePath);
	}

	void LoadTexture(const std::wstring_view textureName)
	{
		const auto& path = File::LoadPath(textureName);

		auto& texture = s_Textures[path.FileName];
		texture.CreateWIC(path.RelativePath);
	}

	void LoadObjModel(const std::wstring_view modelName)
	{
		TimeStamp::Begin();

		const auto& path = File::LoadPath(modelName);
		s_ObjModels[path.FileName] = ObjLoader::LoadFile(path.FileName, path.ParentPath);

		const auto time = TimeStamp::End();
		LOGLINE(L"モデル[%s]読み込み時間 : %.2fms", path.FileName.c_str(), time);
	}

	void LoadMesh(const std::wstring_view modelName)
	{
		TimeStamp::Begin();

		const auto& path = File::LoadPath(modelName);
		auto& model = s_AssimpModels[path.FileName];

		const bool flag = AssimpTest::LoadMesh(path.RelativePath, model.ModelMeshes, model.ModelMaterials);
		ENSURES(flag, "読み込み[ %s ] : %d", path.FileName.c_str(), flag);

		const auto time = TimeStamp::End();
		LOGLINE("モデル[%s]読み込み時間 : %.2fms", path.FileName.c_str(), time);
	}

	gsl::not_null<ID3DBlob*> GetShader(const std::wstring_view shaderName)
	{
		const auto& shader = s_Shaders[shaderName.data()];
		return shader.Get();
	}

	const Texture& GetTexture(const std::wstring_view texutreName)
	{
		const auto& texture = s_Textures[texutreName.data()];
		return texture;
	}

	const Model& GetObjModel(const std::wstring_view fileName)
	{
		const auto& model = s_ObjModels[fileName.data()];
		return model;
	}

	const Model& GetMesh(const std::wstring_view modelName)
	{
		return s_AssimpModels[modelName.data()];
	}
}