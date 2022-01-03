#pragma once
#include "Model.h"
#include "Texture.h"
#include "Matrix4x4.h"
#include "Command.h"
#include "Pipeline.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class RenderObject
{
public:
	static bool DrawCollider;

public:
	RenderObject() noexcept
		: m_TextureGpuHandle{}
	{}
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount);
	void Initialize(const Camera& camera) noexcept;
	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData);

private:
	ID3D12GraphicsCommandList* m_TexBundle;
	ID3D12GraphicsCommandList* m_ColliderBundle;
	Pipeline m_TexPipeline;
	Pipeline m_ColliderPipeline;

	ResourceHeap m_ResourceHeap;

	VertexBuffer<ModelMeshVertex> m_VertexBuffer;
	VertexBuffer<DirectX::XMFLOAT4X4> m_InstanceBuffer;
	IndexBuffer<uint32_t> m_IndexBuffer;

	ConstantBuffer<CameraData> m_CameraBuffer;
	ConstantBuffer<LightData> m_LightBuffer;
	ConstantBuffer<ModelMaterial> m_MaterialBuffer;

	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;
};