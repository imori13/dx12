#pragma once
#include "ResourceHeap.h"
#include "UploadBuffer.h"
#include "Texture.h"

//struct Vertex
//{
//	DirectX::XMFLOAT3 Position;
//	DirectX::XMFLOAT2 Texcoord;
//};

#pragma warning (disable : 4324)
struct alignas(256) Transform
{
	DirectX::XMMATRIX World;		// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// 射影行列
};
#pragma warning (default : 4324)

struct alignas(256) Light
{
	DirectX::XMFLOAT4 LightPosition;
	DirectX::XMFLOAT4 Color;
};

class TestModel
{
public:
	TestModel() noexcept
		: m_pTransform(nullptr)
		, m_Position{}
		, m_Scale{}
		, m_Rotate{}
		, m_IndexCount(0)
	{

	}

	bool OnInit(std::wstring_view modelName);
	void SetTexture(const std::wstring_view textureName);
	void Update() noexcept;
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void OnTerm() noexcept;

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Scale;
	DirectX::XMFLOAT3 m_Rotate;
private:
	ResourceHeap m_CBV_SRVHeap;
	UploadBuffer m_VertexData;
	UploadBuffer m_IndexData;
	UploadBuffer m_TransformData;
	UploadBuffer m_LightData;
	UploadBuffer m_MaterialData;
	Texture m_Texture;
	uint32_t m_IndexCount;

	Transform* m_pTransform;	// UploadBufferのメモリを参照しているので、デストラクタで破棄される
};

