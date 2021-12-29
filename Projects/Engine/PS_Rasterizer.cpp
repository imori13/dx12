#include "PS_Rasterizer.h"

void PS_Rasterizer::Default() noexcept
{
	SetSolidMode(true);
	SetCulling(true);
	m_RasterizerDesc.FrontCounterClockwise = FALSE;			// ���v��蒸�_��O�ʂƂ���
	m_RasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;	// �s�N�Z���[�x�����B����Z�\�[�g�p
	m_RasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	m_RasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	m_RasterizerDesc.DepthClipEnable = FALSE;			// �����Ɋ�Â��N���b�s���O
	m_RasterizerDesc.MultisampleEnable = FALSE;		// �}���`�T���v�����Oon/off
	m_RasterizerDesc.AntialiasedLineEnable = FALSE;	// �A���`�G�C���A�V���Oon/off
	m_RasterizerDesc.ForcedSampleCount = 0;
	m_RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// ���X�^���C�U�̓h�͈͍L����

	m_SampleDesc.Count = 1;
	m_SampleDesc.Quality = 0;

	m_SampleMask = UINT_MAX;
}

void PS_Rasterizer::SetSolidMode(bool flag) noexcept
{
	m_RasterizerDesc.FillMode = (flag) ? (D3D12_FILL_MODE_SOLID) : (D3D12_FILL_MODE_WIREFRAME);
}

void PS_Rasterizer::SetCulling(bool flag) noexcept
{
	m_RasterizerDesc.CullMode = (flag) ? (D3D12_CULL_MODE_BACK) : (D3D12_CULL_MODE_NONE);
}

void PS_Rasterizer::SetMultiSample(bool flag, uint32_t count, uint32_t quality) noexcept
{
	m_RasterizerDesc.MultisampleEnable = flag;
	m_SampleDesc.Count = count;
	m_SampleDesc.Quality = quality;
}
