#pragma once

struct ModelMeshVertex	// ���b�V���p���_
{
	DirectX::XMFLOAT3 Position;
	//DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	//DirectX::XMFLOAT3 Tangent;
};

struct ModelMaterial	// ���f���p�}�e���A��
{
	DirectX::XMFLOAT3 Diffuse;
	DirectX::XMFLOAT3 Specular;
	float Alpha;
	float Shininess;
	std::wstring DiffuseMap;
};

struct ModelMesh	// ���f���p���b�V��
{
	std::vector<ModelMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	uint32_t MaterialId;
};

struct Model	// ���f��
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};