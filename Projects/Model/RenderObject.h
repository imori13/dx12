#pragma once
#include "UploadBuffer.h"
#include "Model.h"
#include "Texture.h"
#include "Matrix4x4.h"
#include "Command.h"
#include "Pipeline.h"
#include "Camera.h"

class RenderObject
{
public:
	static bool DrawCollider;

public:
	RenderObject() noexcept
		: m_TextureGpuHandle{}
		, m_IndexCount(0)
		, m_TextureExistence{}
	{}
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount);
	void Create(const ModelMesh& mesh, int32_t objectCount);
	void Initialize(const Camera& camera) noexcept;
	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData);

private:
	ID3D12GraphicsCommandList* m_TexBandle;
	ID3D12GraphicsCommandList* m_ColliderBandle;
	Pipeline m_TexPipeline;
	Pipeline m_ColliderPipeline;

	ResourceHeap m_ResourceHeap;
	UploadBuffer m_VertexBuffer;
	UploadBuffer m_InstanceBuffer;
	UploadBuffer m_IndexBuffer;
	UploadBuffer m_CameraBuffer;
	UploadBuffer m_LightBuffer;
	UploadBuffer m_MaterialBuffer;
	UploadBuffer m_NotTexBuffer;
	UploadBuffer m_ColliderBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	CameraData* m_CameraData;
	LightData* m_LightData;
	std::vector<DirectX::XMFLOAT4X4*> m_InstanceData;

	int32_t m_IndexCount;
	bool m_TextureExistence;
};