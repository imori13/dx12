#pragma once

struct ModelMeshVertex	// メッシュ用頂点
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;
};

struct alignas(256) ModelMaterial	// モデル用マテリアル
{
	DirectX::XMFLOAT3 Ambient;
	float Alpha;
	DirectX::XMFLOAT3 Diffuse;
	float Ni;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
};

struct ModelMesh	// モデル用メッシュ
{
	std::vector<ModelMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::wstring MaterialName;
};

struct Model	// モデル
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};