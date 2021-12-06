#pragma once
#include "ResourceHeap.h"
#include "UploadBuffer.h"
#include "Texture.h"
#include "PipelineStateObject.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Texcoord;
};

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;	// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// 射影行列
};

class TestModel
{
public:
	TestModel() noexcept
		: m_pRootSignature(nullptr)
		, m_Viewport{}
		, m_Scissor{}
	{

	}

	bool OnInit(const std::wstring& texturePath);
	void Update() noexcept;
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void OnTerm() noexcept;

	float m_RotateAngle;
private:
	ResourceHeap m_CbvHeap;
	UploadBuffer m_VertexData;
	UploadBuffer m_IndexData;
	UploadBuffer m_ConstantData;
	Texture m_Texture;
	PipelineStateObject pipelineStateObject;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;

	Transform* m_pTransform;	// UploadBufferのメモリを参照しているので、デストラクタで破棄される

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_Scissor;
};

