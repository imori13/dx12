#pragma once
#include "UploadBuffer.h"
#include "Model.h"
#include "Texture.h"
#include "Matrix4x4.h"
#include "PipelineInitializer.h"
#include "Command.h"

struct RenderData
{
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
};

class RenderObject
{
public:
	RenderObject() noexcept
		: m_TextureGpuHandle{}
		, m_IndexCount(0)
	{}
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount);
	void Initialize(const Matrix4x4& view, const Matrix4x4& projection) noexcept;
	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, const Matrix4x4& world, gsl::span<Vector3> position);
private:
	ID3D12GraphicsCommandList* m_Bandle;
	PipelineStateObject m_Pipeline;

	ResourceHeap m_ResourceHeap;
	UploadBuffer m_VertexBuffer;
	UploadBuffer m_InstanceBuffer;
	UploadBuffer m_IndexBuffer;
	UploadBuffer m_CameraBuffer;
	UploadBuffer m_LightBuffer;
	UploadBuffer m_MaterialBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	CameraData* m_CameraData;
	std::vector<DirectX::XMFLOAT4X4*> m_InstanceData;

	int32_t m_IndexCount;
};