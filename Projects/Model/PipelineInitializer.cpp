#include "PipelineInitializer.h"

#include "InputElement.h"
#include "ResourceManager.h"

#include <map>

namespace
{
	std::map<std::wstring, PipelineStateObject> s_Pipelines = {};
}

const PipelineStateObject CreateDefault(std::wstring_view vsName, std::wstring_view psName);


namespace PipelineInitializer
{
	void Initialize(std::wstring_view vsName, std::wstring_view psName)
	{
		s_Pipelines[L"DefaultPipeline"] = CreateDefault(vsName, psName);
	}

	const PipelineStateObject& GetPipeline(std::wstring_view pipelineName)
	{
		const auto& pipeline = s_Pipelines[pipelineName.data()];
		return pipeline;
	}
}

const PipelineStateObject CreateDefault(std::wstring_view vsName, std::wstring_view psName)
{
	InputElement inputElement;
	inputElement.SetVertexElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputElement.SetVertexElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	inputElement.SetVertexElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
	inputElement.SetVertexElement("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT);
	inputElement.SetInstanceElement("TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT);
	inputElement.SetInstanceElement("TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT);
	inputElement.SetInstanceElement("TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT);
	inputElement.SetInstanceElement("TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT);
	//inputElement.SetInstanceElement("INSTANCECOLOR", DXGI_FORMAT_R32G32B32_FLOAT, 1);

	const auto vsShader = ResourceManager::GetShader(vsName);
	const auto psShader = ResourceManager::GetShader(psName);

	// ルートシグネチャ読み込み
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
	auto hr = D3DGetBlobPart(vsShader->GetBufferPointer(), vsShader->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &rootSignatureBlob);
	ENSURES(hr, "RootSignature設定の取得");

	// ルートシグネチャ設定
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature = nullptr;
	hr = Graphics::g_pDevice->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));
	ENSURES(hr, "RootSignatureの生成");

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
