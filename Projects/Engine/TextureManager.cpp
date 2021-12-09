#include "TextureManager.h"

namespace
{
	std::map<std::wstring, Texture> s_Textures;
}

namespace TextureManager
{
	void LoadTexture(const std::wstring_view textureName)
	{
		auto& texture = s_Textures[textureName.data()];
		texture.CreateWIC(textureName.data());
	}
	const Texture& GetTexture(const std::wstring_view texutreName)
	{
		const auto& tex = s_Textures[texutreName.data()];
		return tex;
	}
}