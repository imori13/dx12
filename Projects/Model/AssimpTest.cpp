#include "AssimpTest.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/iostreams/code_converter.hpp>

namespace
{
	constexpr uint32_t VERTEX_COUNT = 3u;
}

namespace AssimpTest
{
	void ParseMesh(ModelMesh& destMesh, const aiMesh* pSourceMesh)
	{
		// �}�e���A���ԍ�
		//destMesh.MaterialId = pSourceMesh->mMaterialIndex;

		aiVector3D zero3D(0.0f, 0.0f, 0.0f);

		// �������m��
		destMesh.Vertices.resize(pSourceMesh->mNumVertices);
		destMesh.Indices.resize(static_cast<uint64_t>(pSourceMesh->mNumFaces) * VERTEX_COUNT);

		// ���_���i�[
		for(auto i = 0u; i < pSourceMesh->mNumVertices; ++i)
		{
			const auto pPosition = &(pSourceMesh->mVertices[i]);
			const auto pNormal = &(pSourceMesh->mNormals[i]);
			const auto pTexCoord = (pSourceMesh->HasTextureCoords(0)) ? &(pSourceMesh->mTextureCoords[0][i]) : &zero3D;
			const auto pTangent = (pSourceMesh->HasTangentsAndBitangents()) ? &(pSourceMesh->mTangents[i]) : &zero3D;

			destMesh.Vertices.at(i) =
			{
				DirectX::XMFLOAT3(pPosition->x,pPosition->y,pPosition->z),
				DirectX::XMFLOAT3(pNormal->x, pNormal->y, pNormal->z),
				DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y),
				DirectX::XMFLOAT3(pTangent->x, pTangent->y, pTangent->z)
			};
		}

		// �C���f�b�N�X���i�[
		for(uint64_t i = 0u; i < pSourceMesh->mNumFaces; ++i)
		{
			const auto& face = pSourceMesh->mFaces[i];

			destMesh.Indices.at(i * VERTEX_COUNT + 0) = face.mIndices[0];
			destMesh.Indices.at(i * VERTEX_COUNT + 1) = face.mIndices[1];
			destMesh.Indices.at(i * VERTEX_COUNT + 2) = face.mIndices[2];
		}
	}

	void ParseMaterial(ModelMaterial& destMaterial, const aiMaterial* pSourceMaterial)
	{
		// �����ː���
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
			{
				destMaterial.Ambient.x = color.r;
				destMaterial.Ambient.y = color.g;
				destMaterial.Ambient.z = color.b;
			}
			else
			{
				destMaterial.Ambient.x = 0.5f;
				destMaterial.Ambient.y = 0.5f;
				destMaterial.Ambient.z = 0.5f;
			}
		}

		// �g�U���ː���
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				destMaterial.Diffuse.x = color.r;
				destMaterial.Diffuse.y = color.g;
				destMaterial.Diffuse.z = color.b;
			}
			else
			{
				destMaterial.Diffuse.x = 0.5f;
				destMaterial.Diffuse.y = 0.5f;
				destMaterial.Diffuse.z = 0.5f;
			}
		}

		// ���ʔ��ː���
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if(pSourceMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			{
				destMaterial.Specular.x = color.r;
				destMaterial.Specular.y = color.g;
				destMaterial.Specular.z = color.b;
			}
			else
			{
				destMaterial.Specular.x = 0.5f;
				destMaterial.Specular.y = 0.5f;
				destMaterial.Specular.z = 0.5f;
			}
		}

		// ���ʔ��ˋ��x
		{
			auto shininess = 0.0f;
			if(pSourceMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{ destMaterial.Shininess = shininess; }
			else
			{ destMaterial.Shininess = 0.0f; }
		}

		//// �f�B�t���[�Y�}�b�v
		//{
		//	aiString path;
		//	if(pSourceMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		//	{ destMaterial.DiffuseMap = std::string(path.C_Str()); }
		//	else
		//	{ destMaterial.DiffuseMap.clear(); }
		//}
	}

	std::string ToUTF8(std::wstring_view value)
	{
		auto length = WideCharToMultiByte(CP_UTF8, 0u, value.data(), -1, nullptr, 0, nullptr, nullptr);
		auto buffer = new char[length];
		WideCharToMultiByte(CP_UTF8, 0u, value.data(), -1, buffer, length, nullptr, nullptr);

		std::string result(buffer);
		delete[] buffer;
		buffer = nullptr;
		return result;
	}

	bool AssimpTest::LoadMesh(std::wstring_view fileName, std::vector<ModelMesh>& meshes, std::vector<ModelMaterial>& materials)
	{
		if(fileName.empty())
		{ return false; }

		std::string path = ToUTF8(fileName);

		Assimp::Importer importer;
		uint32_t flag = 0u;
		flag |= aiProcess_Triangulate;	// �O�p���_
		flag |= aiProcess_PreTransformVertices;	// �m�[�h�O���t�Ȃ��Œ���
		flag |= aiProcess_CalcTangentSpace;	// �^���W�F���g�v�Z
		flag |= aiProcess_GenSmoothNormals;	// �@�����X���[�Y
		flag |= aiProcess_GenUVCoords;
		flag |= aiProcess_RemoveRedundantMaterials;	// �璷�ȃ}�e���A�����폜
		flag |= aiProcess_OptimizeMeshes;	// ���b�V�������œK��

		// �t�@�C���̓ǂݍ���
		auto pScene = importer.ReadFile(path.data(), flag);
		ENSURES(pScene != nullptr, "�t�@�C���ǂݍ��� [ %s ] : ", path.data());

		// �������m��
		meshes.clear();
		meshes.resize(pScene->mNumMeshes);
		materials.clear();
		materials.resize(pScene->mNumMaterials);

		// ���b�V���f�[�^��ϊ�
		for(auto i = 0u; i < meshes.size(); ++i)
		{
			const auto pMesh = pScene->mMeshes[i];
			ParseMesh(meshes[i], pMesh);
		}

		// �}�e���A���f�[�^��ϊ�
		for(auto i = 0u; i < materials.size(); ++i)
		{
			const auto pMaterial = pScene->mMaterials[i];
			ParseMaterial(materials[i], pMaterial);
		}

		// �s�v�ɂȂ����̂ŃN���A
		pScene = nullptr;

		// ����I��
		return true;
	}
}
