#include "PipelineInitializer.h"

#include "InputElement.h"
#include "ResourceManager.h"

#include <map>

namespace
{
	std::map<std::wstring, PipelineStateObject> s_Pipelines = {};
}

const PipelineStateObject CreateDefault();


namespace PipelineInitializer
{
	void Initialize()
	{
		s_Pipelines[L"DefaultPipeline"] = CreateDefault();
	}

	const PipelineStateObject& GetPipeline(std::wstring_view pipelineName)
	{
		const auto& pipeline = s_Pipelines[pipelineName.data()];
		return pipeline;
	}
}

const PipelineStateObject CreateDefault()
{
	InputElement inputElement;
	inputElement.SetElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputElement.SetElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);

	const auto vsShader = ResourceManager::GetShader(L"SimpleTexVS.cso");
	const auto psShader = ResourceManager::GetShader(L"SimpleTexPS.cso");

	// ルートシグネチャ読み込み
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
	auto hr = D3DGetBlobPart(vsShader->GetBufferPointer(), vsShader->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &rootSignatureBlob);
	ENSURES(hr, L"RootSignature設定の取得");

	// ルートシグネチャ設定
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature = nullptr;
	hr = Graphics::g_pDevice->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
	ENSURES(hr, L"RootSignatureの生成");

	// パイプラインステート設定
	PipelineStateObject pipelineStateObject;
	pipelineStateObject.SetInputLayout(inputElement.Get());
	pipelineStateObject.SetRootSignature(pRootSignature.Get());
	pipelineStateObject.SetVertexShader(vsShader);
	pipelineStateObject.SetPixelShader(psShader);
	pipelineStateObject.SetRasterizerDesc();
	pipelineStateObject.SetBlendDesc();
	pipelineStateObject.SetDepthStencil(true);

	// パイプラインステート生成
	pipelineStateObject.Create();

	return pipelineStateObject;
}
