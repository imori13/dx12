#pragma once
#include "UploadBuffer.h"
#include "Model.h"
#include "Texture.h"

class RenderObject
{
public:
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, uint32_t objectCount);
	void Initialize();
	void Draw(DirectX::XMFLOAT3 pos,
			  DirectX::XMFLOAT3 rotate,
			  DirectX::XMFLOAT3 scale);
	void SendCommand(gsl::not_null<ID3D12GraphicsCommandList*> cmdList);
private:
	ResourceHeap m_ResourceHeap;

	UploadBuffer m_VertexBuffer;
	UploadBuffer m_IndexBuffer;
	std::vector<UploadBuffer> m_TransformBuffers;
	UploadBuffer m_LightBuffer;
	UploadBuffer m_MaterialBuffer;

	D3D12_GPU_DESCRIPTOR_HANDLE m_TextureGpuHandle;

	std::vector<Transform*> m_Transforms;
	std::vector<Transform*> m_WaitDraw;
	uint32_t m_IndexCount;

	uint32_t m_ObjectCount;
	uint32_t m_DrawIndex;
};
