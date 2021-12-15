#include "ObjLoader.h"
#include "File.h"
#include "FileInput.h"

#include <boost/algorithm/string.hpp>
#include <deque>

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
	Model LoadFile(std::wstring_view name)
	{
		File::Exists(name);
		const auto& path = File::LoadPath(name);

		EXPECTS(path.Extension == L".obj");

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
				texcoord.y = 1 - std::stof(splitLine.at(1));
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

		// 出力用
		Model model;

		const auto& mesh = s_TempMesh;
		ModelMesh modelMesh;
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

					//if(meshFace.NormalIndex.size() > 0)
					//	modelMeshVertex.Normal = mesh.Normal.at(mesh.Faces.at(faceIndex).NormalIndex[vertexNum]);

					modelMesh.Vertices.emplace_back(modelMeshVertex);
				}

				// インデックスの設定
				{
					modelMesh.Indices.emplace_back(indices);
					++indices;
				}
			}
		}
		model.ModelMeshes.emplace_back(modelMesh);

		return model;
	}
}