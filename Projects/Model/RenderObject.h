#pragma once
#include "UploadBuffer.h"
#include "Model.h"
#include "Texture.h"

class RenderObject
{
public:
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount);
	void Draw(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
private:
	ResourceHeap m_ResourceHeap;

	UploadBuffer m_VertexBuffer;
	UploadBuffer m_IndexBuffer;
	UploadBuffer m_TransformBuffer;
	UploadBuffer m_LightBuffer;
	UploadBuffer m_MaterialBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_VertexView;
	D3D12_INDEX_BUFFER_VIEW m_IndexView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_TransformView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_LightView;
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_MaterialView;
	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	std::vector<Transform*> m_Transforms;
	uint32_t m_IndexCount;
};
