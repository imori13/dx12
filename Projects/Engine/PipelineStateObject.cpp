#include "PipelineStateObject.h"

// Set
void PipelineStateObject::SetInputLayout(const gsl::span<const D3D12_INPUT_ELEMENT_DESC> inputLayout)
{ m_State.InputLayout = { inputLayout.data(), gsl::narrow<uint32_t>(inputLayout.size()) }; }

void PipelineStateObject::SetRootSignature(const gsl::not_null<ID3D12RootSignature*> rootSignature)
{ m_State.pRootSignature = rootSignature; }

void PipelineStateObject::SetVertexShader(const gsl::not_null<ID3DBlob*> vsShader)
{ m_State.VS = { vsShader->GetBufferPointer(), vsShader->GetBufferSize() }; }

void PipelineStateObject::SetPixelShader(const gsl::not_null<ID3DBlob*> psShader)
{ m_State.PS = { psShader->GetBufferPointer(), psShader->GetBufferSize() }; }

void PipelineStateObject::SetRasterizerDesc() noexcept
{ m_State.RasterizerState = GetDefaultRasterizerDesc(); }

void PipelineStateObject::SetBlendDesc()
{ m_State.BlendState = GetDefaultBlendDesc(); }

void PipelineStateObject::SetDepthStencil(const bool enableDepth) noexcept
{ m_State.DepthStencilState = GetDepthStencilDesc(enableDepth); }

// Get
const D3D12_RASTERIZER_DESC PipelineStateObject::GetDefaultRasterizerDesc() const noexcept
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

	return descRS;
}

const D3D12_BLEND_DESC PipelineStateObject::GetDefaultBlendDesc() const
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

	return descBS;
}

const D3D12_DEPTH_STENCIL_DESC PipelineStateObject::GetDepthStencilDesc(bool enableDepth) const noexcept
{
	// �[�x�X�e���V���X�e�[�g�̐ݒ�
	D3D12_DEPTH_STENCIL_DESC descDSS = {};
	descDSS.DepthEnable = enableDepth;
	descDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	descDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	// �[�x�e�X�g�̔�r�֌W
	descDSS.StencilEnable = FALSE;

	return descDSS;
}
