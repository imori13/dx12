#pragma once
#include "ResourceHeap.h"
#include "UploadBuffer.h"
#include "Texture.h"
#include "Model.h"

class TestModel
{
public:
	TestModel() noexcept
		: m_pTransform(nullptr)
		, m_Position{}
		, m_Scale{}
		, m_Rotate{}
		, m_TextureGpuHandle{}
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
	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	Transform* m_pTransform;	// UploadBuffer�̃��������Q�Ƃ��Ă���̂ŁA�f�X�g���N�^�Ŕj�������
};

