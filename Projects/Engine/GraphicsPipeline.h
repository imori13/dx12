#pragma once
#include "ShaderData.h"

struct GraphicsPipeline
{
public:
	GraphicsPipeline() = default;
	GraphicsPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature) noexcept
		: m_pPipelineStateObject(pipelineState)
		, m_pRootSignature(pRootSignature)
	{
	}

	// Get
	const gsl::not_null<ID3D12PipelineState*> GetPipeline() const noexcept { return m_pPipelineStateObject.Get(); }
	const gsl::not_null<ID3D12RootSignature*> GetSignature() const noexcept { return m_pRootSignature.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineStateObject;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
};

namespace PipelineCreater
{
	void SetShader(const ShaderData& shaderData);
	void SetRasterizerDesc() noexcept;
	void SetBlendDesc();
	void SetDepthStencil(const bool enableDepth) noexcept;
	GraphicsPipeline CreatePipeline();
};