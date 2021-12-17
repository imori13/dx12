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
	std::wstring MaterialName;
};

#pragma warning (disable : 4324)

struct alignas(256) ModelMaterial	// モデル用マテリアル
{
	DirectX::XMFLOAT3 Ambient;
	float Alpha;
	DirectX::XMFLOAT3 Diffuse;
	float Ni;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
};

#pragma warning (default : 4324)

struct Model	// モデル
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};

#pragma warning (disable : 4324)

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;		// ワールド行列
	DirectX::XMMATRIX View;		// ビュー行列
	DirectX::XMMATRIX Proj;		// 射影行列
};

struct alignas(256) Light
{
	DirectX::XMFLOAT4 LightPosition;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT4 CameraPosition;
};
#pragma warning (default : 4324)