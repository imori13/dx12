#pragma once
#include "UploadBuffer.h"
#include "Model.h"
#include "Texture.h"
#include "Matrix4x4.h"

class RenderObject
{
public:
	RenderObject() noexcept
		: m_TextureGpuHandle{}
		, m_IndexCount(0)
		, m_ObjectCount(0)
		, m_DrawIndex(0)
	{}
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount);
	void Initialize();
	void Draw(const Matrix4x4 world, const Matrix4x4 view, const Matrix4x4 projection, gsl::span<Vector3> positions);
	void SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
private:
	ResourceHeap m_ResourceHeap;

	UploadBuffer m_VertexBuffer;
	UploadBuffer m_IndexBuffer;
	std::vector<UploadBuffer> m_TransformBuffers;
	UploadBuffer m_CameraBuffer;
	UploadBuffer m_LightBuffer;
	UploadBuffer m_MaterialBuffer;

	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	std::vector<Transform*> m_Transforms;
	std::vector<Transform*> m_WaitDraw;
	CameraBuff* m_CameraData;
	uint32_t m_IndexCount;

	uint32_t m_ObjectCount;
	uint32_t m_DrawIndex = 0;
};
