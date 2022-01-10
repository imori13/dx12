#pragma once

#pragma warning (push)
#pragma warning (disable : 4324)

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

struct alignas(256) ModelMaterial	// モデル用マテリアル
{
	DirectX::XMFLOAT4 Color = { 1.0f,1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 Ambient = { 0.25f,0.25f,0.25f };
	float _1{};
	DirectX::XMFLOAT3 Diffuse = { 0.75f,0.75f,0.75f };
	float _2{};
	DirectX::XMFLOAT3 Specular = { 0.25f,0.25f,0.25f };
	float Shininess = 0;

	std::wstring DiffuseMap;
};

struct Model	// モデル
{
	std::vector<ModelMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};

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

struct alignas(256) OutlineData
{
	DirectX::XMFLOAT4 OutlineColor;
};

//struct VertexBoneData
//{
//	uint8_t id;
//	float weight;
//};

constexpr uint32_t MAX_BONE_INFLUENCE = 4;

struct SkeletonVertex	// メッシュ用頂点
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;
	uint32_t id[MAX_BONE_INFLUENCE];
	float weight[MAX_BONE_INFLUENCE];
};

struct Bone
{
	const char* name;
	DirectX::XMFLOAT4X4 matrix;
};

struct BoneInfo
{
	uint32_t id;
	DirectX::XMFLOAT4X4 matrix;
};

struct SkeletonMesh	// モデル用メッシュ
{
	std::vector<SkeletonVertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<Bone> bones;
	uint32_t MaterialId = 0;
};

struct SkeletonModel	// モデル
{
	std::vector<SkeletonMesh> ModelMeshes;
	std::vector<ModelMaterial> ModelMaterials;
};

#pragma warning (pop)