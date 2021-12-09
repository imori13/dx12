#pragma once
#include "Texture.h"

#include <map>

namespace TextureManager
{
	void LoadTexture(const std::wstring_view textureName);
	const Texture& GetTexture(const std::wstring_view texutreName);
};

