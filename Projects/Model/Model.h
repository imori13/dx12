#pragma once

struct ModelMeshVertex	// メッシュ用頂点
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;
};

struct ModelMesh	// モデル用メッシュ
{
	std::vector<ModelMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	uint32_t MaterialId = 0;
};

#pragma warning (disable : 4324)
struct alignas(256) ModelMaterial	// モデル用マテリアル
{
	DirectX::XMFLOAT3 Ambient = { 0.1f,0.1f,0.1f };
	float _{};
	DirectX::XMFLOAT3 Diffuse = { 0.5f,0.5f,0.5f };
	float Alpha = 1;
	DirectX::XMFLOAT3 Specular = { 0.1f,0.1f,0.1f };
	float Shininess = 0;

	std::wstring DiffuseMap;
};
#pragma warning (default : 4324)

struct Model	// モデル
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};

#pragma warning (disable : 4324)
struct alignas(256) CameraData
{
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Proj;
};

struct alignas(256) LightData
{
	DirectX::XMFLOAT3 LightDirection;
	float _1{};
	DirectX::XMFLOAT3 LightColor;
	float _2{};
	DirectX::XMFLOAT3 CameraPosition;
	float _3{};
	DirectX::XMFLOAT3 CameraDirection;
	float _4{};
};
#pragma warning (default : 4324)