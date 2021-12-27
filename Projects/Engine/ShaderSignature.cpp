#include "ShaderSignature.h"
#include "GraphicsCore.h"
#include "Debug.h"

void ShaderSignature::SetSignature(gsl::not_null<ID3DBlob*> shader)
{
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
	auto hr = D3DGetBlobPart(shader->GetBufferPointer(), shader->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &rootSignatureBlob);
	ENSURES(hr, "RootSignature �f�[�^�擾");

	// ���[�g�V�O�l�`���ݒ�
	hr = Graphics::g_pDevice->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
	ENSURES(hr, "RootSignature ����");
}

gsl::not_null<ID3D12RootSignature*> ShaderSignature::Get() const noexcept
{
	return m_pRootSignature.Get();
}
