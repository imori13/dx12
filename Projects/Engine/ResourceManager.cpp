#include "ResourceManager.h"
#include "FileSearch.h"

namespace
{
	const std::wstring LoadPath(const std::wstring_view name)
	{
		std::wstring path;
		const bool flag = SearchFilePath(name.data(), path);
		ENSURES(flag == true, L"ファイルロード [ %s ]", path.c_str());
		return path;
	}

	std::map<std::wstring, Texture> s_Textures;
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3DBlob>> s_Shaders;
}

namespace ResourceManager
{
	void LoadTexture(const std::wstring_view textureName)
	{
		auto& texture = s_Textures[textureName.data()];
		texture.CreateWIC(LoadPath(textureName));

	}
	void LoadShader(const std::wstring_view shaderName)
	{
		const auto& path = LoadPath(shaderName);
		auto& shader = s_Shaders[shaderName.data()];

		const auto hr = D3DReadFileToBlob(path.c_str(), shader.GetAddressOf());
		ENSURES(hr, L"Shader読み込み [ %s ]", path.c_str());
	}

	const Texture& GetTexture(const std::wstring_view texutreName)
	{
		const auto& texture = s_Textures[texutreName.data()];
		return texture;
	}
	gsl::not_null<ID3DBlob*> GetShader(const std::wstring_view shaderName)
	{
		const auto& shader = s_Shaders[shaderName.data()];
		return shader.Get();
	}
}