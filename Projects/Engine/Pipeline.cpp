#include "Pipeline.h"
#include "GraphicsCore.h"
#include "Debug.h"

namespace
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC s_State;
}

Pipeline PipelineState::Create()
{
	Pipeline pipeline;

	const auto blob = ShaderState.RootSignature.GetBytecode();
	auto hr = Graphics::g_pDevice->CreateRootSignature(0, blob.pShaderBytecode, blob.BytecodeLength, IID_PPV_ARGS(&pipeline.Signature));
	ENSURES(hr, "RootSignatureê∂ê¨");

	auto state = GetState();
	state.pRootSignature = pipeline.Signature.Get();
	hr = Graphics::g_pDevice->CreateGraphicsPipelineState(&state, IID_PPV_ARGS(&pipeline.Pipeine));
	ENSURES(hr, "PipelineStateObjectê∂ê¨");

	return pipeline;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineState::GetState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC state = {};

	state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// ShaderState
	state.VS = ShaderState.VertexShader.GetBytecode();
	state.PS = ShaderState.PixelShader.GetBytecode();
	state.InputLayout = ShaderState.InputLayout.GetLayout();

	// DepthStencil
	state.DepthStencilState = DepthStencil.GetDesc();
	state.DSVFormat = DepthStencil.GetFormat();

	// Rasterrizer
	state.RasterizerState = Rasterizer.GetRasterizerDesc();
	state.SampleDesc = Rasterizer.GetSampleDesc();
	state.SampleMask = Rasterizer.GetSampleMask();

	// RenderTarget
	state.BlendState = RenderTarget.GetBlendDesc();
	state.NumRenderTargets = RenderTarget.GetRtvNum();
	for(auto i = 0; i < RenderTarget.GetRtvNum(); ++i)
		gsl::at(state.RTVFormats, i) = RenderTarget.GetRtvFormat().at(i);

	return state;
}
