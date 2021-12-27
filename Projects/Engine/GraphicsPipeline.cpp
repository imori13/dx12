#include "GraphicsPipeline.h"
#include "GraphicsCore.h"
#include "Debug.h"

namespace
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_State;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> s_pPipelineStateObject;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> s_pRootSignature;
}

namespace PipelineCreater
{
	void SetShader(const ShaderData& shaderData)
	{
		s_pRootSignature = shaderData.ShaderSignature.Get();

		m_State.VS = shaderData.VertexShader.GetBytecode();
		m_State.PS = shaderData.PixelShader.GetBytecode();
		m_State.InputLayout = shaderData.ShaderInput.GetLayout();
		m_State.pRootSignature = shaderData.ShaderSignature.Get();
	}

	void SetRasterizerDesc() noexcept
	{
		D3D12_RASTERIZER_DESC descRS;
		descRS.FillMode = D3D12_FILL_MODE_SOLID;		// ���C���[�t���[��or�h��Ԃ�
		descRS.CullMode = D3D12_CULL_MODE_NONE;			// ���ʃJ�����Oon/off
		descRS.FrontCounterClockwise = FALSE;			// ���v��蒸�_��O�ʂƂ���
		descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;	// �s�N�Z���[�x�����B����Z�\�[�g�p
		descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		descRS.DepthClipEnable = FALSE;			// �����Ɋ�Â��N���b�s���O
		descRS.MultisampleEnable = FALSE;		// �}���`�T���v�����Oon/off
		descRS.AntialiasedLineEnable = FALSE;	// �A���`�G�C���A�V���Oon/off
		descRS.ForcedSampleCount = 0;
		descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// ���X�^���C�U�̓h�͈͍L����

		m_State.RasterizerState = descRS;
	}

	void SetBlendDesc()
	{
		// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�(�������Ȃǂ��\)
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		// �u�����h�X�e�[�g�̐ݒ�
		D3D12_BLEND_DESC descBS;
		descBS.AlphaToCoverageEnable = FALSE;
		descBS.IndependentBlendEnable = FALSE;
		for(UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{ gsl::at(descBS.RenderTarget, i) = descRTBS; }

		m_State.BlendState = descBS;
	}

	void SetDepthStencil(const bool enableDepth) noexcept
	{
		D3D12_DEPTH_STENCIL_DESC descDSS = {};
		descDSS.DepthEnable = enableDepth;
		descDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		descDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	// �[�x�e�X�g�̔�r�֌W
		descDSS.StencilEnable = FALSE;

		m_State.DepthStencilState = descDSS;
	}

	GraphicsPipeline CreatePipeline()
	{
		m_State.SampleMask = UINT_MAX;
		m_State.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_State.NumRenderTargets = 1;
		m_State.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_State.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		m_State.SampleDesc.Count = 1;
		m_State.SampleDesc.Quality = 0;

		// �p�C�v���C���X�e�[�g�𐶐�
		const auto hr = Graphics::g_pDevice->CreateGraphicsPipelineState(&m_State, IID_PPV_ARGS(s_pPipelineStateObject.GetAddressOf()));
		ENSURES(hr, "PipelineStateObject����");

		return GraphicsPipeline(s_pPipelineStateObject, s_pRootSignature);
	}
}
