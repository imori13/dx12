#include "ResourceManager.h"
#include "File.h"
#include "ObjLoader.h"
#include "TimeStamp.h"

namespace
{
	std::map<std::wstring, Texture> s_Textures;
	std::map<std::wstring, Model> s_Models;
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
		s_Models[path.FileName] = ObjLoader::LoadFile(path.FileName, path.ParentPath);

		const auto time = TimeStamp::End();
		LOGLINE(L"モデル[%s]読み込み時間 : %.2fms", path.FileName.c_str(), time);
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

	const Model& GetModel(const std::wstring_view fileName)
	{
		const auto& model = s_Models[fileName.data()];
		return model;
	}
}