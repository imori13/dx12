#pragma once

struct ModelMeshVertex	// ���b�V���p���_
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;
};

struct alignas(256) ModelMaterial	// ���f���p�}�e���A��
{
	DirectX::XMFLOAT3 Ambient;
	float Alpha;
	DirectX::XMFLOAT3 Diffuse;
	float Ni;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
};

struct ModelMesh	// ���f���p���b�V��
{
	std::vector<ModelMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::wstring MaterialName;
};

struct Model	// ���f��
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};