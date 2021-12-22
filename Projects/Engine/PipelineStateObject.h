#pragma once
#include "GraphicsCore.h"

class PipelineStateObject
{
public:
	PipelineStateObject() noexcept
		: m_pPipelineStateObject(nullptr)
		, m_State{}
		, isCreated(false)
	{
		m_State.SampleMask = UINT_MAX;
		m_State.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_State.NumRenderTargets = 1;
		m_State.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_State.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		m_State.SampleDesc.Count = 1;
		m_State.SampleDesc.Quality = 0;
	}

	// Create
	void Create();
	// Get
	const gsl::not_null<ID3D12PipelineState*> Get() const noexcept { return m_pPipelineStateObject.Get(); }
	const gsl::not_null<ID3D12RootSignature*> GetSignature() const noexcept { return m_pRootSignature.Get(); }

	void SetInputLayout(const gsl::span<const D3D12_INPUT_ELEMENT_DESC> inputLayout);
	void SetRootSignature(const gsl::not_null<ID3D12RootSignature*> rootSignature);
	void SetVertexShader(const gsl::not_null<ID3DBlob*> vsShader);
	void SetPixelShader(const gsl::not_null<ID3DBlob*> psShader);
	void SetRasterizerDesc() noexcept;
	void SetBlendDesc();
	void SetDepthStencil(const bool enableDepth) noexcept;

private:
	const D3D12_RASTERIZER_DESC GetDefaultRasterizerDesc() const noexcept;
	const D3D12_BLEND_DESC GetDefaultBlendDesc() const;
	const D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(bool enableDepth) const noexcept;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineStateObject;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_State;
	bool isCreated;
};

