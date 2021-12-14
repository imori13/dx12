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

class TestModel
{
public:
	TestModel() noexcept
		: m_pTransform(nullptr)
		, m_RotateAngle(0)
	{

	}

	bool OnInit(std::wstring_view modelName);
	void SetTexture(const std::wstring_view textureName);
	void Update() noexcept;
	void Render(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
	void OnTerm() noexcept;

	float m_X{}, m_Y{}, m_Z{};
	float m_RotateAngle{};
private:
	ResourceHeap m_CBV_SRVHeap;
	UploadBuffer m_VertexData;
	UploadBuffer m_IndexData;
	UploadBuffer m_ConstantData;
	Texture m_Texture;

	Transform* m_pTransform;	// UploadBufferのメモリを参照しているので、デストラクタで破棄される
};

