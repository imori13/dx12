#pragma once
#include "UploadBuffer.h"
#include "Model.h"
#include "Texture.h"
#include "Matrix4x4.h"
#include "Command.h"
#include "Pipeline.h"
#include "Camera.h"
#include "ConstantBuffer.h"

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
	void Initialize(const Camera& camera) noexcept;
	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData);

private:
	ID3D12GraphicsCommandList* m_TexBundle;
	ID3D12GraphicsCommandList* m_ColliderBundle;
	Pipeline m_TexPipeline;
	Pipeline m_ColliderPipeline;

	ResourceHeap m_ResourceHeap;
	UploadBuffer m_VertexBuffer;
	UploadBuffer m_InstanceBuffer;
	UploadBuffer m_IndexBuffer;
	ConstantBuffer<CameraData> m_CameraBuffer;
	ConstantBuffer<LightData> m_LightBuffer;
	ConstantBuffer<ModelMaterial> m_MaterialBuffer;
	ConstantBuffer<OutlineData> m_OutlineBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	std::vector<DirectX::XMFLOAT4X4*> m_InstanceData;

	int32_t m_IndexCount;
	bool m_TextureExistence;
};