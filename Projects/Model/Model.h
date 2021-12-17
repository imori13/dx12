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
	std::wstring MaterialName;
};

#pragma warning (disable : 4324)

struct alignas(256) ModelMaterial	// ���f���p�}�e���A��
{
	DirectX::XMFLOAT3 Ambient;
	float Alpha;
	DirectX::XMFLOAT3 Diffuse;
	float Ni;
	DirectX::XMFLOAT3 Specular;
	float Shininess;
};

#pragma warning (default : 4324)

struct Model	// ���f��
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};

#pragma warning (disable : 4324)

struct alignas(256) Transform
{
	DirectX::XMMATRIX World;		// ���[���h�s��
	DirectX::XMMATRIX View;		// �r���[�s��
	DirectX::XMMATRIX Proj;		// �ˉe�s��
};

struct alignas(256) Light
{
	DirectX::XMFLOAT4 LightPosition;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT4 CameraPosition;
};
#pragma warning (default : 4324)