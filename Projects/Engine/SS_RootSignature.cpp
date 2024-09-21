#include "SS_RootSignature.h"
#include "Debug.h"

void SS_RootSignature::SetSignature(gsl::not_null<ID3DBlob*> shader)
{
	const auto hr = D3DGetBlobPart(shader->GetBufferPointer(), shader->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &m_pRootSignatureBlob);
	ENSURES(hr, "RootSignature データ取得");
}