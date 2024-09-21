#include "ObjLoader.h"
#include "File.h"
#include "FileInput.h"

#include <deque>
#include <boost/algorithm/string.hpp>

namespace
{
	constexpr uint32_t VERTEX_NUM = 3;

	struct FaceIndex
	{
		std::vector<uint32_t> PositionIndex;
		std::vector<uint32_t> TexcoordIndex;
		std::vector<uint32_t> NormalIndex;
	};

	struct LoadMesh
	{
		std::wstring MaterialName;
		std::vector<DirectX::XMFLOAT3> Position;
		std::vector<DirectX::XMFLOAT2> Texcoord;
		std::vector<DirectX::XMFLOAT3> Normal;
		std::vector<FaceIndex> Faces;
	};
}

namespace ObjLoader
{
	bool LoadModel(std::vector<ModelMesh>& vec, std::wstring_view fileName);
	bool LoadMaterial(std::vector<ModelMaterial>& vec, std::wstring_view fileName, std::wstring_view filePath);

	bool LoadFile(Model& model, std::wstring_view fileName, std::wstring_view filePath)
	{
		bool flag = LoadModel(model.ModelMeshes, filePath.data() + std::wstring(fileName.data()));
		if(!flag) { return false; }

		//flag = LoadMaterial(model.ModelMaterials, model.ModelMeshes.at(0).MaterialName.c_str(), filePath.data());
		//if(!flag) { return false; }

		return true;
	}

	bool LoadModel(std::vector<ModelMesh>& vec, std::wstring_view fileName)
	{
		File::Exists(fileName);
		const auto& path = File::LoadPath(fileName);

		if(path.Extension != L".obj")
		{ return false; }

		// ファイルを開く
		FileInput file;
		file.Open(path.AbsolutePath);

		// ロード用
		LoadMesh s_TempMesh;

		while(!file.EndOfFile())
		{
			// 1行読み込む
			std::wstring line = file.ReadLine();

			// コメントの行を無視
			if(line.empty() || line.front() == L'#')
			{ continue; }

			// 行split
			std::deque<std::wstring> splitLine;
			boost::algorithm::split(splitLine, line, boost::is_space(), boost::token_compress_on);

			// ヘッダー
			std::wstring header = splitLine.front();
			// 先頭を除去
			splitLine.pop_front();
			// 末尾に""があればpop
			if(splitLine.back() == L"")
			{ splitLine.pop_back(); }

			// マテリアル
			if(header == L"mtllib")
			{
				s_TempMesh.MaterialName = splitLine.front();
				continue;
			}

			if(header == L"v")
			{
				DirectX::XMFLOAT3 pos;
				pos.x = std::stof(splitLine.at(0));
				pos.y = std::stof(splitLine.at(1));
				pos.z = std::stof(splitLine.at(2));
				s_TempMesh.Position.emplace_back(pos);
				continue;
			}

			if(header == L"vt")
			{
				DirectX::XMFLOAT2 texcoord;
				texcoord.x = std::stof(splitLine.at(0));
				texcoord.y = 1 - std::stof(splitLine.at(1));	// UVのV反転
				s_TempMesh.Texcoord.emplace_back(texcoord);
				continue;
			}

			if(header == L"vn")
			{
				DirectX::XMFLOAT3 normal;
				normal.x = std::stof(splitLine.at(0));
				normal.y = std::stof(splitLine.at(1));
				normal.z = std::stof(splitLine.at(2));
				s_TempMesh.Normal.emplace_back(normal);
				continue;
			}

			if(header == L"f")
			{
				// 4頂点以上なら複数のFaceを作る
				for(auto faceCount = 0u; faceCount <= splitLine.size() - VERTEX_NUM; ++faceCount)
				{
					// Face1個分
					FaceIndex face{};
					for(auto i = 0u; i < VERTEX_NUM; ++i)
					{
						std::vector<std::wstring> faceIndex;
						const uint32_t connectVertexIndex = (i == 0) ? (0) : (i + faceCount);
						boost::algorithm::split(faceIndex, splitLine.at(connectVertexIndex), boost::is_any_of("/"));

						constexpr uint32_t OFFSET = 1;

						// positionインデックス
						face.PositionIndex.emplace_back(std::stoul(faceIndex.at(0)) - OFFSET);
						// texcoordインデックス
						if(faceIndex.at(1) != L"")
							face.TexcoordIndex.emplace_back(std::stoul(faceIndex.at(1)) - OFFSET);
						// normalインデックス
						face.NormalIndex.emplace_back(std::stoul(faceIndex.at(2)) - OFFSET);
					}
					s_TempMesh.Faces.emplace_back(face);
				}
				continue;
			}
		}

		file.Close();

		// 参照
		auto& modelMesh = vec.emplace_back();

		// マテリアル名
		const auto& mesh = s_TempMesh;
		//modelMesh.MaterialName = mesh.MaterialName;

		uint32_t indices = 0;
		for(auto faceIndex = 0u; faceIndex < mesh.Faces.size(); ++faceIndex)
		{
			const auto meshFace = mesh.Faces.at(faceIndex);
			for(auto vertexNum = 0u; vertexNum < VERTEX_NUM; ++vertexNum)
			{
				// 頂点の読み込み
				{
					ModelMeshVertex modelMeshVertex;
					modelMeshVertex.Position = mesh.Position.at(meshFace.PositionIndex.at(vertexNum));

					if(meshFace.TexcoordIndex.size() > 0)
						modelMeshVertex.TexCoord = mesh.Texcoord.at(meshFace.TexcoordIndex.at(vertexNum));

					if(meshFace.NormalIndex.size() > 0)
						modelMeshVertex.Normal = mesh.Normal.at(mesh.Faces.at(faceIndex).NormalIndex.at(vertexNum));

					modelMesh.Vertices.emplace_back(modelMeshVertex);
				}

				// インデックスの設定
				{
					modelMesh.Indices.emplace_back(indices);
					++indices;
				}
			}
		}

		return true;
	}

	bool LoadMaterial(std::vector<ModelMaterial>& vec, std::wstring_view fileName, std::wstring_view filePath)
	{
		const auto FILEPATH = std::wstring(filePath.data()) + std::wstring(fileName.data());

		File::Exists(FILEPATH);
		const auto& path = File::LoadPath(FILEPATH);

		if(path.Extension != L".mtl")
		{ return false; }

		// ファイルを開く
		FileInput file;
		file.Open(path.AbsolutePath);

		// ロード用
		LoadMesh s_TempMesh;

		auto& material = vec.emplace_back();

		while(!file.EndOfFile())
		{
			// 1行読み込む
			std::wstring line = file.ReadLine();

			// コメントの行を無視
			if(line.empty() || line.front() == L'#')
			{ continue; }

			// 行split
			std::deque<std::wstring> splitLine;
			boost::algorithm::split(splitLine, line, boost::is_space(), boost::token_compress_on);

			// 先頭にスペースがある場合
			if(splitLine.front() == L"")
			{ splitLine.pop_front(); }
			// ヘッダー
			std::wstring header = splitLine.front();
			// 先頭を除去
			splitLine.pop_front();
			// 末尾に""があればpop
			if(splitLine.back() == L"")
			{ splitLine.pop_back(); }

			if(header == L"Ka")
			{
				material.Ambient.x = std::stof(splitLine.at(0));
				material.Ambient.y = std::stof(splitLine.at(1));
				material.Ambient.z = std::stof(splitLine.at(2));
			}

			if(header == L"Kd")
			{
				material.Diffuse.x = std::stof(splitLine.at(0));
				material.Diffuse.y = std::stof(splitLine.at(1));
				material.Diffuse.z = std::stof(splitLine.at(2));
			}

			if(header == L"Ks")
			{
				material.Specular.x = std::stof(splitLine.at(0));
				material.Specular.y = std::stof(splitLine.at(1));
				material.Specular.z = std::stof(splitLine.at(2));
			}

			if(header == L"Ns")
			{
				material.Shininess = std::stof(splitLine.at(0));
			}

			//if(header == L"d")
			//{
			//	material.Alpha = std::stof(splitLine.at(0));
			//}
		}

		file.Close();

		return true;
	}
}