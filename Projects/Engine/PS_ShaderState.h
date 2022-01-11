#pragma once
#include "SS_Binary.h"
#include "SS_InputLayout.h"
#include "SS_RootSignature.h"

struct PS_ShaderState
{
	SS_Binary VertexShader;
	SS_Binary PixelShader;
	SS_InputLayout InputLayout;
	SS_RootSignature RootSignature;
};