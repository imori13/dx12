#pragma once
#include "Matrix4x4.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

class RenderObject
{
public:
	static bool DrawCollider;

public:
	RenderObject() noexcept(false);
	void Create(const ModelMesh& mesh, const ModelMaterial& material, const Texture& texture, int32_t objectCount);
	void Initialize(const Camera& camera);
	void DrawArray(gsl::not_null<ID3D12GraphicsCommandList*> cmdList, gsl::span<Matrix4x4> matrixData);
private:
	class Impl;
	std::shared_ptr<Impl> m_pImpl;
};