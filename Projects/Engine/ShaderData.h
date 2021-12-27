#pragma once
#include "ShaderBinary.h"
#include "ShaderInput.h"
#include "ShaderSignature.h"

struct ShaderData
{
	ShaderBinary VertexShader;
	ShaderBinary PixelShader;
	ShaderInput ShaderInput;
	ShaderSignature ShaderSignature;
};