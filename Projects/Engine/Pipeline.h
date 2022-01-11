#pragma once
#include "PS_ShaderState.h"
#include "PS_DepthStencil.h"
#include "PS_Rasterizer.h"
#include "PS_RenderTarget.h"

struct Pipeline
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;
};

struct PipelineState
{
	PS_ShaderState ShaderState;
	PS_DepthStencil DepthStencil;
	PS_Rasterizer Rasterizer;
	PS_RenderTarget RenderTarget;

	Pipeline Create();
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetState();
};