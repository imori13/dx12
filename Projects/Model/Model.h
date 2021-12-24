#pragma once

struct ModelMeshVertex	// ���b�V���p���_
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;
};

struct ModelMesh	// ���f���p���b�V��
{
	std::vector<ModelMeshVertex> Vertices;
	std::vector<uint32_t> Indices;
	uint32_t MaterialId = 0;
};

#pragma warning (disable : 4324)

struct alignas(256) ModelMaterial	// ���f���p�}�e���A��
{
	DirectX::XMFLOAT3 Ambient = { 0.1f,0.1f,0.1f };
	float Alpha = 1;
	DirectX::XMFLOAT3 Diffuse = { 0.5f,0.5f,0.5f };
	float Ni = 0;
	DirectX::XMFLOAT3 Specular = { 0.1f,0.1f,0.1f };
	float Shininess = 0;

	std::wstring DiffuseMap;
};

#pragma warning (default : 4324)

struct Model	// ���f��
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};

#pragma warning (disable : 4324)

struct alignas(256) CameraData
{
	DirectX::XMFLOAT4X4 View;		// �r���[�s��
	DirectX::XMFLOAT4X4 Proj;		// �ˉe�s��
};

struct alignas(256) LightData
{
	DirectX::XMFLOAT4 LightPosition;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT4 CameraPosition;
};
#pragma warning (default : 4324)