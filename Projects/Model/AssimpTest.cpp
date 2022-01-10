#include "AssimpTest.h"
#include "Debug.h"
#include "Matrix4x4.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/iostreams/code_converter.hpp>

#pragma warning (push)
#pragma warning (disable : 26445)
#pragma warning (disable : 26812)

namespace
{
	constexpr uint32_t VERTEX_COUNT = 3u;
}

namespace AssimpTest
{
	// 文字コード変換
	std::string ToString(std::wstring_view value)
	{
		const auto length = WideCharToMultiByte(CP_ACP, 0u, value.data(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> buffer = std::vector<char>(length);
		WideCharToMultiByte(CP_UTF8, 0u, value.data(), -1, buffer.data(), length, nullptr, nullptr);

		std::string result(buffer.data());
		return result;
	}

	std::wstring ToWstring(std::string_view value)
	{
		const auto length = MultiByteToWideChar(CP_UTF8, 0, value.data(), -1, nullptr, 0);
		std::vector<wchar_t> buffer = std::vector<wchar_t>(length);
		MultiByteToWideChar(CP_UTF8, 0, value.data(), -1, buffer.data(), length);

		std::wstring result(buffer.data());
		return result;
	}

	// メッシュ変換
	void ParseMesh(ModelMesh* destMesh, const aiMesh& pSourceMesh)
	{
		// マテリアル番号
		destMesh->MaterialId = pSourceMesh.mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// メモリ確保
		destMesh->Vertices.resize(pSourceMesh.mNumVertices);
		destMesh->Indices.resize(static_cast<uint64_t>(pSourceMesh.mNumFaces) * VERTEX_COUNT);

		// 頂点を格納
		const auto& vertexSpan = gsl::make_span(pSourceMesh.mVertices, pSourceMesh.mNumVertices);
		const auto& normalSpan = gsl::make_span(pSourceMesh.mNormals, pSourceMesh.mNumVertices);
		const auto& texcoordSpan = gsl::make_span(pSourceMesh.mTextureCoords[0], pSourceMesh.mNumVertices);
		const auto& tangentSpan = gsl::make_span(pSourceMesh.mTangents, pSourceMesh.mNumVertices);
		for(auto i = 0u; i < pSourceMesh.mNumVertices; ++i)
		{
			const auto& pPosition = vertexSpan[i];
			const auto& pNormal = normalSpan[i];
			const auto& pTexCoord = (pSourceMesh.HasTextureCoords(0)) ? (texcoordSpan[i]) : zero3D;
			const auto& pTangent = (pSourceMesh.HasTangentsAndBitangents()) ? (tangentSpan[i]) : zero3D;

			destMesh->Vertices.at(i) =
			{
				DirectX::XMFLOAT3(pPosition.x,pPosition.y,pPosition.z),
				DirectX::XMFLOAT3(pNormal.x, pNormal.y, pNormal.z),
				DirectX::XMFLOAT2(pTexCoord.x, pTexCoord.y),
				DirectX::XMFLOAT3(pTangent.x, pTangent.y, pTangent.z)
			};
		}

		// インデックスを格納
		const auto& faces = gsl::make_span(pSourceMesh.mFaces, pSourceMesh.mNumFaces);
		for(uint64_t i = 0u; i < faces.size(); ++i)
		{
			const auto& indicesSpan = gsl::make_span(faces[i].mIndices, faces[i].mNumIndices);

			destMesh->Indices.at(i * VERTEX_COUNT + 0) = indicesSpan[0];
			destMesh->Indices.at(i * VERTEX_COUNT + 1) = indicesSpan[1];
			destMesh->Indices.at(i * VERTEX_COUNT + 2) = indicesSpan[2];
		}
	}

	// メッシュ変換
	void ParseMesh(SkeletonMesh* destMesh, const aiMesh& pSourceMesh)
	{
		// マテリアル番号
		destMesh->MaterialId = pSourceMesh.mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// メモリ確保
		destMesh->Vertices.resize(pSourceMesh.mNumVertices);
		destMesh->Indices.resize(static_cast<uint64_t>(pSourceMesh.mNumFaces) * VERTEX_COUNT);

		// 頂点を格納
		const auto& vertexSpan = gsl::make_span(pSourceMesh.mVertices, pSourceMesh.mNumVertices);
		const auto& normalSpan = gsl::make_span(pSourceMesh.mNormals, pSourceMesh.mNumVertices);
		const auto& texcoordSpan = gsl::make_span(pSourceMesh.mTextureCoords[0], pSourceMesh.mNumVertices);
		const auto& tangentSpan = gsl::make_span(pSourceMesh.mTangents, pSourceMesh.mNumVertices);
		const auto& boneSpan = gsl::make_span(pSourceMesh.mBones, pSourceMesh.mNumBones);
		for(auto i_vertex = 0u; i_vertex < pSourceMesh.mNumVertices; ++i_vertex)
		{
			const auto& pPosition = vertexSpan[i_vertex];
			const auto& pNormal = normalSpan[i_vertex];
			const auto& pTexCoord = (pSourceMesh.HasTextureCoords(0)) ? (texcoordSpan[i_vertex]) : zero3D;
			const auto& pTangent = (pSourceMesh.HasTangentsAndBitangents()) ? (tangentSpan[i_vertex]) : zero3D;

			destMesh->Vertices.at(i_vertex) =
			{
				DirectX::XMFLOAT3(pPosition.x,pPosition.y,pPosition.z),
				DirectX::XMFLOAT3(pNormal.x, pNormal.y, pNormal.z),
				DirectX::XMFLOAT2(pTexCoord.x, pTexCoord.y),
				DirectX::XMFLOAT3(pTangent.x, pTangent.y, pTangent.z)
			};

			// ボーン変換
			for(auto i_vertexbone = 0; i_vertexbone < MAX_BONE_INFLUENCE; ++i_vertexbone)
			{
				destMesh->Vertices.at(i_vertex).id[i_vertexbone] = -1;
				destMesh->Vertices.at(i_vertex).weight[i_vertexbone] = 0.0f;
			}
		}

		// インデックスを格納
		const auto& faces = gsl::make_span(pSourceMesh.mFaces, pSourceMesh.mNumFaces);
		for(uint64_t i = 0u; i < faces.size(); ++i)
		{
			const auto& indicesSpan = gsl::make_span(faces[i].mIndices, faces[i].mNumIndices);

			destMesh->Indices.at(i * VERTEX_COUNT + 0) = indicesSpan[0];
			destMesh->Indices.at(i * VERTEX_COUNT + 1) = indicesSpan[1];
			destMesh->Indices.at(i * VERTEX_COUNT + 2) = indicesSpan[2];
		}
	}

	// マテリアル変換
	void ParseMaterial(ModelMaterial* destMaterial, const aiMaterial& pSourceMaterial)
	{
		// テクスチャ名
		{
			aiString textureName;
			if(pSourceMaterial.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName) == AI_SUCCESS)
			{
				destMaterial->DiffuseMap = ToWstring(textureName.C_Str());
			}
		}

		// 環境反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial.Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
			{
				destMaterial->Ambient.x = color.r;
				destMaterial->Ambient.y = color.g;
				destMaterial->Ambient.z = color.b;
			}
		}

		// 拡散反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				destMaterial->Diffuse.x = color.r;
				destMaterial->Diffuse.y = color.g;
				destMaterial->Diffuse.z = color.b;
			}
		}

		// 鏡面反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial.Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			{
				destMaterial->Specular.x = color.r;
				destMaterial->Specular.y = color.g;
				destMaterial->Specular.z = color.b;
			}
		}

		// 鏡面反射強度
		{
			auto shininess = 0.0f;
			if(pSourceMaterial.Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{ destMaterial->Shininess = shininess; }
		}
	}

	void LoadBones(SkeletonMesh* destMesh, const aiMesh& pSourceMesh)
	{
		std::map<std::string, BoneInfo> bone_map{};
		uint32_t bone_counter{};

		const auto& boneSpan = gsl::make_span(pSourceMesh.mBones, pSourceMesh.mNumBones);
		for(auto i_bone = 0; i_bone < boneSpan.size(); ++i_bone)
		{
			const auto& bone = boneSpan[i_bone];
			const auto& name = bone->mName.C_Str();

			int bone_id = -1;

			// read bone
			if(bone_map.find(name) == bone_map.end())
			{
				// convert to xmfloat4x4
				const auto& m = bone->mOffsetMatrix;
				const DirectX::XMFLOAT4X4 xmmatrix(
					m.a1, m.a2, m.a3, m.a4,
					m.b1, m.b2, m.b3, m.b4,
					m.c1, m.c2, m.c3, m.c4,
					m.d1, m.d2, m.d3, m.d4);

				BoneInfo boneinfo = {};
				boneinfo.id = bone_counter;
				boneinfo.matrix = xmmatrix;
				// add
				bone_map[name] = boneinfo;
				// set bone_id
				bone_id = bone_counter;

				++bone_counter;
			}
			else
			{
				bone_id = bone_map[name].id;
			}

			ENSURES(bone_id != -1);

			// write boneinfo to vertex
			const auto& weightSpan = gsl::make_span(bone->mWeights, bone->mNumWeights);
			for(auto i_weight = 0; i_weight < weightSpan.size(); ++i_weight)
			{
				// weight data
				const auto& weight = weightSpan[i_weight];

				// vertex
				auto& vertex = destMesh->Vertices.at(weight.mVertexId);

				// insert data in an empty space
				for(int i_vertex = 0; i_vertex < MAX_BONE_INFLUENCE; ++i_vertex)
				{
					const auto& is_space = vertex.id[i_vertex] > 10000;
					if(is_space)	// default space
					{
						vertex.id[i_vertex] = bone_id;
						vertex.weight[i_vertex] = weight.mWeight;
						break;	// id inserted
					}
				}
			}
		}

		// ボーン抽出 終了
	}

	bool AssimpTest::LoadMesh(Model* model, std::wstring_view fileName)
	{
		if(fileName.empty())
		{ return false; }

		std::string path = ToString(fileName);

		Assimp::Importer importer;
		uint32_t flag = 0u;
		flag |= aiProcess_ConvertToLeftHanded;
		flag |= aiProcess_Triangulate;			// 三角頂点
		flag |= aiProcess_FlipUVs;				// UV上下反転
		//flag |= aiProcess_PreTransformVertices;	// ノードグラフなしで調整
		flag |= aiProcess_CalcTangentSpace;		// タンジェント計算
		//flag |= aiProcess_GenSmoothNormals;		// 法線をスムーズ
		//flag |= aiProcess_GenUVCoords;
		//flag |= aiProcess_RemoveRedundantMaterials;	// 冗長なマテリアルを削除
		//flag |= aiProcess_OptimizeMeshes;		// メッシュ数を最適化

		flag |= aiProcess_Triangulate;
		flag |= aiProcess_FlipUVs;
		flag |= aiProcess_JoinIdenticalVertices;
		flag |= aiProcess_OptimizeMeshes;
		flag |= aiProcess_LimitBoneWeights;
		flag |= aiProcess_RemoveRedundantMaterials;

		// ファイルの読み込み
		const aiScene& pScene = *importer.ReadFile(path.data(), flag);

		const auto meshSpan = gsl::make_span(pScene.mMeshes, pScene.mNumMeshes);
		const auto materialSpan = gsl::make_span(pScene.mMaterials, pScene.mNumMaterials);

		// メモリ確保
		model->ModelMeshes.clear();
		model->ModelMeshes.resize(meshSpan.size());
		model->ModelMaterials.clear();
		model->ModelMaterials.resize(materialSpan.size());

		// メッシュデータを変換
		for(auto i = 0u; i < model->ModelMeshes.size(); ++i)
		{
			ParseMesh(&model->ModelMeshes.at(i), *meshSpan[i]);
			//LoadBones(&model->ModelMeshes.at(i), *meshSpan[i]);
		}

		// マテリアルデータを変換
		for(auto i = 0u; i < model->ModelMaterials.size(); ++i)
		{
			ParseMaterial(&model->ModelMaterials.at(i), *materialSpan[i]);
		}

		// 正常終了
		return true;
	}

	bool LoadSkinMesh(SkeletonModel* model, std::wstring_view fileName)
	{
		if(fileName.empty())
		{ return false; }

		std::string path = ToString(fileName);

		Assimp::Importer importer;
		uint32_t flag = 0u;
		flag |= aiProcess_ConvertToLeftHanded;
		flag |= aiProcess_Triangulate;			// 三角頂点
		flag |= aiProcess_FlipUVs;				// UV上下反転
		//flag |= aiProcess_PreTransformVertices;	// ノードグラフなしで調整
		flag |= aiProcess_CalcTangentSpace;		// タンジェント計算
		//flag |= aiProcess_GenSmoothNormals;		// 法線をスムーズ
		//flag |= aiProcess_GenUVCoords;
		//flag |= aiProcess_RemoveRedundantMaterials;	// 冗長なマテリアルを削除
		//flag |= aiProcess_OptimizeMeshes;		// メッシュ数を最適化

		flag |= aiProcess_Triangulate;
		flag |= aiProcess_FlipUVs;
		flag |= aiProcess_JoinIdenticalVertices;
		flag |= aiProcess_OptimizeMeshes;
		flag |= aiProcess_LimitBoneWeights;
		flag |= aiProcess_RemoveRedundantMaterials;

		// ファイルの読み込み
		const aiScene& pScene = *importer.ReadFile(path.data(), flag);

		const auto meshSpan = gsl::make_span(pScene.mMeshes, pScene.mNumMeshes);
		const auto materialSpan = gsl::make_span(pScene.mMaterials, pScene.mNumMaterials);

		// メモリ確保
		model->ModelMeshes.clear();
		model->ModelMeshes.resize(meshSpan.size());
		model->ModelMaterials.clear();
		model->ModelMaterials.resize(materialSpan.size());

		// メッシュデータを変換
		for(auto i = 0u; i < model->ModelMeshes.size(); ++i)
		{
			ParseMesh(&model->ModelMeshes.at(i), *meshSpan[i]);
			LoadBones(&model->ModelMeshes.at(i), *meshSpan[i]);
		}

		// マテリアルデータを変換
		for(auto i = 0u; i < model->ModelMaterials.size(); ++i)
		{
			ParseMaterial(&model->ModelMaterials.at(i), *materialSpan[i]);
		}

		// 正常終了
		return true;
	}
}

#pragma warning (pop)