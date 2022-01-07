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
	// �����R�[�h�ϊ�
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

	// ���b�V���ϊ�
	void ParseMesh(ModelMesh* destMesh, const aiMesh& pSourceMesh)
	{
		// �}�e���A���ԍ�
		destMesh->MaterialId = pSourceMesh.mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// �������m��
		destMesh->Vertices.resize(pSourceMesh.mNumVertices);
		destMesh->Indices.resize(static_cast<uint64_t>(pSourceMesh.mNumFaces) * VERTEX_COUNT);

		// ���_���i�[
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

		// �C���f�b�N�X���i�[
		const auto& faces = gsl::make_span(pSourceMesh.mFaces, pSourceMesh.mNumFaces);
		for(uint64_t i = 0u; i < faces.size(); ++i)
		{
			const auto& indicesSpan = gsl::make_span(faces[i].mIndices, faces[i].mNumIndices);

			destMesh->Indices.at(i * VERTEX_COUNT + 0) = indicesSpan[0];
			destMesh->Indices.at(i * VERTEX_COUNT + 1) = indicesSpan[1];
			destMesh->Indices.at(i * VERTEX_COUNT + 2) = indicesSpan[2];
		}
	}

	// ���b�V���ϊ�
	void ParseMesh(SkeletonMesh* destMesh, const aiMesh& pSourceMesh)
	{
		// �}�e���A���ԍ�
		destMesh->MaterialId = pSourceMesh.mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// �������m��
		destMesh->Vertices.resize(pSourceMesh.mNumVertices);
		destMesh->Indices.resize(static_cast<uint64_t>(pSourceMesh.mNumFaces) * VERTEX_COUNT);

		// ���_���i�[
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
		}

		// �C���f�b�N�X���i�[
		const auto& faces = gsl::make_span(pSourceMesh.mFaces, pSourceMesh.mNumFaces);
		for(uint64_t i = 0u; i < faces.size(); ++i)
		{
			const auto& indicesSpan = gsl::make_span(faces[i].mIndices, faces[i].mNumIndices);

			destMesh->Indices.at(i * VERTEX_COUNT + 0) = indicesSpan[0];
			destMesh->Indices.at(i * VERTEX_COUNT + 1) = indicesSpan[1];
			destMesh->Indices.at(i * VERTEX_COUNT + 2) = indicesSpan[2];
		}
	}

	// �}�e���A���ϊ�
	void ParseMaterial(ModelMaterial* destMaterial, const aiMaterial& pSourceMaterial)
	{
		// �e�N�X�`����
		{
			aiString textureName;
			if(pSourceMaterial.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName) == AI_SUCCESS)
			{
				destMaterial->DiffuseMap = ToWstring(textureName.C_Str());
			}
		}

		// �����ː���
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial.Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
			{
				destMaterial->Ambient.x = color.r;
				destMaterial->Ambient.y = color.g;
				destMaterial->Ambient.z = color.b;
			}
		}

		// �g�U���ː���
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				destMaterial->Diffuse.x = color.r;
				destMaterial->Diffuse.y = color.g;
				destMaterial->Diffuse.z = color.b;
			}
		}

		// ���ʔ��ː���
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial.Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			{
				destMaterial->Specular.x = color.r;
				destMaterial->Specular.y = color.g;
				destMaterial->Specular.z = color.b;
			}
		}

		// ���ʔ��ˋ��x
		{
			auto shininess = 0.0f;
			if(pSourceMaterial.Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{ destMaterial->Shininess = shininess; }
		}
	}

	void LoadBones(ModelMesh* destMesh, const aiMesh& pSourceMesh)
	{
		//auto& bones = destMesh->Vertices.at(i_vertex).Bones;
		//for(auto i_bone = 0; i_bone < bones.size(); ++i_vertex)
		//{
		//	// �e���{�[�����K��l�ȏ�Ȃ疳��
		//	if(i_bone >= boneSpan[i_bone]->mNumWeights)
		//	{
		//		LOGLINE("%d�Ԗڂ̃{�[���𖳎����܂���", i_bone);
		//		continue;
		//	}
		//	bones.at(i_bone).id = boneSpan[i_bone]->mNumWeights[];
		//	bones.at(i_bone).weight = ;
		//}

		const auto& boneSpan = gsl::make_span(pSourceMesh.mBones, pSourceMesh.mNumBones);

		// vector��bone�̐��ɍ��킹��resize
		//destMesh->bones.resize(pSourceMesh.mNumBones);

		for(auto i = 0; i < boneSpan.size(); ++i)
		{
			const auto& bone = boneSpan[i];
			
			const auto& name = bone->mName.C_Str();
			//destMesh->bones.at(i).name = bone->mName.C_Str();
			//destMesh->bones.at(i).matrix = bone->mOffsetMatrix.;

			const auto& weightSpan = gsl::make_span(bone->mWeights, bone->mNumWeights);
			for(auto j = 0; j < weightSpan.size(); ++j)
			{
				const auto& weight = weightSpan[j];

				auto& vertexId = weight.mVertexId;
				auto& w = weight.mWeight;
			}
		}
	}

	bool AssimpTest::LoadMesh(Model* model, std::wstring_view fileName)
	{
		if(fileName.empty())
		{ return false; }

		std::string path = ToString(fileName);

		Assimp::Importer importer;
		uint32_t flag = 0u;
		flag |= aiProcess_ConvertToLeftHanded;
		flag |= aiProcess_Triangulate;			// �O�p���_
		flag |= aiProcess_FlipUVs;				// UV�㉺���]
		//flag |= aiProcess_PreTransformVertices;	// �m�[�h�O���t�Ȃ��Œ���
		flag |= aiProcess_CalcTangentSpace;		// �^���W�F���g�v�Z
		//flag |= aiProcess_GenSmoothNormals;		// �@�����X���[�Y
		//flag |= aiProcess_GenUVCoords;
		//flag |= aiProcess_RemoveRedundantMaterials;	// �璷�ȃ}�e���A�����폜
		//flag |= aiProcess_OptimizeMeshes;		// ���b�V�������œK��

		flag |= aiProcess_Triangulate;
		flag |= aiProcess_FlipUVs;
		flag |= aiProcess_JoinIdenticalVertices;
		flag |= aiProcess_OptimizeMeshes;
		flag |= aiProcess_LimitBoneWeights;
		flag |= aiProcess_RemoveRedundantMaterials;

		// �t�@�C���̓ǂݍ���
		const aiScene& pScene = *importer.ReadFile(path.data(), flag);

		const auto meshSpan = gsl::make_span(pScene.mMeshes, pScene.mNumMeshes);
		const auto materialSpan = gsl::make_span(pScene.mMaterials, pScene.mNumMaterials);

		// �������m��
		model->ModelMeshes.clear();
		model->ModelMeshes.resize(meshSpan.size());
		model->ModelMaterials.clear();
		model->ModelMaterials.resize(materialSpan.size());

		// ���b�V���f�[�^��ϊ�
		for(auto i = 0u; i < model->ModelMeshes.size(); ++i)
		{
			ParseMesh(&model->ModelMeshes.at(i), *meshSpan[i]);
			LoadBones(&model->ModelMeshes.at(i), *meshSpan[i]);
		}

		// �}�e���A���f�[�^��ϊ�
		for(auto i = 0u; i < model->ModelMaterials.size(); ++i)
		{
			ParseMaterial(&model->ModelMaterials.at(i), *materialSpan[i]);
		}

		// ����I��
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
		flag |= aiProcess_Triangulate;			// �O�p���_
		flag |= aiProcess_FlipUVs;				// UV�㉺���]
		//flag |= aiProcess_PreTransformVertices;	// �m�[�h�O���t�Ȃ��Œ���
		flag |= aiProcess_CalcTangentSpace;		// �^���W�F���g�v�Z
		//flag |= aiProcess_GenSmoothNormals;		// �@�����X���[�Y
		//flag |= aiProcess_GenUVCoords;
		//flag |= aiProcess_RemoveRedundantMaterials;	// �璷�ȃ}�e���A�����폜
		//flag |= aiProcess_OptimizeMeshes;		// ���b�V�������œK��

		flag |= aiProcess_Triangulate;
		flag |= aiProcess_FlipUVs;
		flag |= aiProcess_JoinIdenticalVertices;
		flag |= aiProcess_OptimizeMeshes;
		flag |= aiProcess_LimitBoneWeights;
		flag |= aiProcess_RemoveRedundantMaterials;

		// �t�@�C���̓ǂݍ���
		const aiScene& pScene = *importer.ReadFile(path.data(), flag);

		const auto meshSpan = gsl::make_span(pScene.mMeshes, pScene.mNumMeshes);
		const auto materialSpan = gsl::make_span(pScene.mMaterials, pScene.mNumMaterials);

		// �������m��
		model->ModelMeshes.clear();
		model->ModelMeshes.resize(meshSpan.size());
		model->ModelMaterials.clear();
		model->ModelMaterials.resize(materialSpan.size());

		// ���b�V���f�[�^��ϊ�
		for(auto i = 0u; i < model->ModelMeshes.size(); ++i)
		{
			ParseMesh(&model->ModelMeshes.at(i), *meshSpan[i]);
			//LoadBones(&model->ModelMeshes.at(i) ,*meshSpan[i]);
		}

		// �}�e���A���f�[�^��ϊ�
		for(auto i = 0u; i < model->ModelMaterials.size(); ++i)
		{
			ParseMaterial(&model->ModelMaterials.at(i), *materialSpan[i]);
		}

		// ����I��
		return true;

		return false;
	}
}

#pragma warning (pop)