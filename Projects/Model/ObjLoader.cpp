#include "ObjLoader.h"
#include "File.h"
#include "FileInput.h"

#include <boost/algorithm/string.hpp>
#include <map>

namespace
{
	struct FaceIndex
	{
		uint32_t PositionIndex[3];
		uint32_t TexcoordIndex[3];
		uint32_t NormalIndex[3];
	};

	struct LoadMesh
	{
		std::wstring MaterialName;
		std::vector<DirectX::XMFLOAT3> Position;
		std::vector<DirectX::XMFLOAT2> Texcoord;
		//std::vector<DirectX::XMFLOAT3> Normal;
		std::vector<FaceIndex> Faces;
	};

	std::vector<LoadMesh> s_LoadMesh;
	LoadMesh s_TempMesh;
}

namespace ObjLoader
{
	Model LoadFile(std::wstring_view name)
	{
		File::Exists(name);
		const auto& path = File::LoadPath(name);

		// ファイルを開く
		FileInput file;
		file.Open(path.AbsolutePath);

		// クリア
		s_LoadMesh.clear();

		while(!file.EndOfFile())
		{
			// 1行読み込む
			const std::wstring& line = file.ReadLine();

			// コメントの行を無視
			if(line.empty() || line.at(0) == L'#')
			{ continue; }

			// 行split
			std::vector<std::wstring> splitLine;
			boost::algorithm::split(splitLine, line, boost::is_space());

			std::wstring header = splitLine.at(0);
			splitLine.erase(splitLine.begin());

			// マテリアル
			if(header == L"mtllib")
			{
				if(s_TempMesh.MaterialName.size() > 0)
				{
					s_LoadMesh.emplace_back(s_TempMesh);
					// 書き込み先初期化
					s_TempMesh = {};
				}

				s_TempMesh.MaterialName = splitLine.front();
			}

			if(header == L"v")
			{
				DirectX::XMFLOAT3 pos;
				pos.x = std::stof(splitLine.at(0));
				pos.y = std::stof(splitLine.at(1));
				pos.z = std::stof(splitLine.at(2));
				s_TempMesh.Position.emplace_back(pos);
			}

			if(header == L"vt")
			{
				DirectX::XMFLOAT2 texcoord;
				texcoord.x = std::stof(splitLine.at(0));
				texcoord.y = std::stof(splitLine.at(1));
				s_TempMesh.Texcoord.emplace_back(texcoord);
			}

			//if(header == L"vn")
			//{
			//	DirectX::XMFLOAT3 normal;
			//	normal.x = std::stof(splitLine.at(0));
			//	normal.y = std::stof(splitLine.at(1));
			//	normal.z = std::stof(splitLine.at(2));
			//	s_TempMesh.Normal.emplace_back(normal);
			//}

			if(header == L"f")
			{
				// まだ三角形頂点以外に対応していない
				EXPECTS(splitLine.size() == 3);

				FaceIndex face{};
				for(auto i = 0u; i < splitLine.size(); ++i)
				{
					std::vector<std::wstring> faceIndex;
					boost::algorithm::split(faceIndex, splitLine.at(i), boost::is_any_of("/"));

					gsl::at(face.PositionIndex, i) = std::stoul(faceIndex.at(0)) - 1;
					gsl::at(face.TexcoordIndex, i) = std::stoul(faceIndex.at(1)) - 1;
					gsl::at(face.NormalIndex, i) = std::stoul(faceIndex.at(2)) - 1;
				}
				s_TempMesh.Faces.emplace_back(face);
			}
		}

		s_LoadMesh.emplace_back(s_TempMesh);

		Model model;
		uint32_t iii = 0;
		for(const auto& mesh : s_LoadMesh)
		{
			ModelMesh modelMesh;
			for(auto faceIndex = 0u; faceIndex < mesh.Faces.size(); ++faceIndex)
			{
				for(auto vertexNum = 0u; vertexNum < 3; ++vertexNum)
				{
					ModelMeshVertex modelMeshVertex;
					modelMeshVertex.Position = mesh.Position.at(gsl::at(mesh.Faces.at(faceIndex).PositionIndex, vertexNum));
					modelMeshVertex.TexCoord = mesh.Texcoord.at(gsl::at(mesh.Faces.at(faceIndex).TexcoordIndex, vertexNum));
					//modelMeshVertex.Position = mesh.Position.at(mesh.Faces.at(faceIndex).PositionIndex[vertexNum]);
					//modelMeshVertex.TexCoord = mesh.Texcoord.at(mesh.Faces.at(faceIndex).TexcoordIndex[vertexNum]);
					//modelMeshVertex.Normal = mesh.Normal.at(mesh.Faces.at(faceIndex).NormalIndex[vertexNum]);
					modelMesh.Vertices.emplace_back(modelMeshVertex);
					modelMesh.Indices.emplace_back(iii);
					++iii;
				}
			}
			model.ModelMeshes.emplace_back(modelMesh);
		}

		file.Close();

		return model;
	}
}