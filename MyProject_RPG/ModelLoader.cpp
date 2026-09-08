#include "ModelLoader.h"
#include "CommonType.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/material.h"

#include <iostream>
#include <cassert>

#if _MSC_VER >= 1920
#ifdef _DEBUG
#pragma comment(lib, "assimp/x64/Debug/assimp-vc142-mtd.lib")
#else
#pragma comment(lib, "assimp/x64/Release/assimp-vc142-mt.lib")
#endif
#elif _MSC_VER >= 1910
#ifdef _DEBUG
#pragma comment(lib, "assimp/x64/Debug/assimp-vc141-mtd.lib")
#endif
#endif

namespace
{
	/**
	 * @brief Assimp行列をDirectXの行列に変換する
	 */
	Matrix4x4 ConvertMatrix(
		const aiMatrix4x4& aimatrix)
	{
		Matrix4x4 rst = {
	   aimatrix.a1,aimatrix.b1,aimatrix.c1,aimatrix.d1,
	   aimatrix.a2,aimatrix.b2,aimatrix.c2,aimatrix.d2,
	   aimatrix.a3,aimatrix.b3,aimatrix.c3,aimatrix.d3,
	   aimatrix.a4,aimatrix.b4,aimatrix.c4,aimatrix.d4
		};

		return rst;
	}

	/**
	 * @brief AssimpのMaterialをMaterialDataに変換する
	 */
	MaterialData LoadMaterial(const aiMaterial* aimaterial)
	{
		MaterialData rst;
		aiColor4D col;
		if (AI_SUCCESS ==
			aiGetMaterialColor(
				aimaterial,
				AI_MATKEY_COLOR_DIFFUSE,
				&col))
		{
			rst.Diffuse = Vector4(
				col.r, col.g, col.b, col.a);
		}

		if (AI_SUCCESS ==
			aiGetMaterialColor(
				aimaterial,
				AI_MATKEY_COLOR_AMBIENT,
				&col))
		{
			rst.Ambient =
				Vector4(
					col.r,
					col.g,
					col.b,
					col.a);
		}

		if (AI_SUCCESS ==
			aiGetMaterialColor(
				aimaterial,
				AI_MATKEY_COLOR_SPECULAR,
				&col))
		{
			rst.Specular =
				Vector4(
					col.r,
					col.g,
					col.b,
					col.a);
		}

		return rst;
	}
}

/**
 * @brief モデルデータを読み込む
 * @param filePath モデルファイル名
 * @return 読み込んだモデルデータの共有ポインタ
 * @details Assimpを使用してモデルデータを読み込み、
 *			ModelData構造体に変換する
 */
std::shared_ptr<ModelData> ModelLoader::LoadModel(
	const std::string& filePath,
	float scaleBase,
	bool flip,
	bool simple)
{
	auto model = 
		std::make_shared<ModelData>();

	Assimp::Importer importer;	// Assimpのインポーターを作成

	//====================
	// 読み込みオプションの設定
	//====================
	unsigned int flags =
		aiProcess_Triangulate |	// 三角形化
		aiProcess_JoinIdenticalVertices | // 同一頂点の結合
		aiProcess_GenSmoothNormals | // スムーズ法線の生成
		aiProcess_SortByPType; // プリミティブタイプごとにソート

	if (flip)
	{
		flags |= aiProcess_FlipUVs; // UV反転
	}
	if(simple)
	{
		// ノードを一つにまとめる　アニメーション情報は無視される
		flags |= aiProcess_PreTransformVertices;
	}

	// ====================
	// モデルの読み込み
	// ====================
	const aiScene* scene = 
		importer.ReadFile(filePath, flags);
	
	if (!scene ||
		!scene->mRootNode) {
		return nullptr;
	}

	model->SetScaleBase(scaleBase);

	//=====================
	// マテリアルの読み込み
	//=====================
	for(unsigned int i = 0;
		i < scene->mNumMaterials ; ++i)
	{
		MaterialData material =
			LoadMaterial(scene->mMaterials[i]);
		model->GetMaterials().push_back(material);
	}

	//=====================
	// メッシュの読み込み
	//=====================
	for(unsigned int i = 0;
		i < scene->mNumMeshes; ++i)
	{
		const aiMesh* aiMesh =
			scene->mMeshes[i];
		
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;

		//=====================
		// 頂点情報の読み込み
		//=====================
		vertices.resize(aiMesh->mNumVertices);

		for (unsigned int v = 0;
			v < aiMesh->mNumVertices;
			++v)
		{
			VertexData vertex{};

			//====================
			// Position
			//====================

			vertex.Position =
			{
				aiMesh->mVertices[v].x,
				aiMesh->mVertices[v].y,
				aiMesh->mVertices[v].z
			};

			//====================
			// Normal
			//====================

			if (aiMesh->HasNormals())
			{
				vertex.Normal =
				{
					aiMesh->mNormals[v].x,
					aiMesh->mNormals[v].y,
					aiMesh->mNormals[v].z
				};
			}

			//====================
			// UV
			//====================

			if (aiMesh->HasTextureCoords(0))
			{
				vertex.TexCoord =
				{
					aiMesh->mTextureCoords[0][v].x,
					aiMesh->mTextureCoords[0][v].y
				};
			}

			vertices[v] = vertex;
		}

		//=====================
		// インデックス情報の読み込み
		//=====================
		for (unsigned int f = 0;
			f < aiMesh->mNumFaces; ++f)
		{
			const aiFace& face =
				aiMesh->mFaces[f];

			for(unsigned int index = 0;
				index < face.mNumIndices; ++index)
			{
				indices.push_back(
					face.mIndices[index]);
			}
		}

		//=====================
		// GPU メッシュバッファの作成
		//=====================
		MeshData meshData;

		// CPUデータとして保持する
		meshData.Vertices =
			std::move(vertices);

		meshData.Indices =
			std::move(indices);

		meshData.MaterialIndex =
			aiMesh->mMaterialIndex;

		model->GetMeshes().emplace_back(
			std::move(meshData));
	}

	//=====================
	// 予定：スケルトン情報の読み込み
	//=====================
	/*
	 * 内容
	 * ：aiMesh->mBonesを使用して、ボーン情報を読み込み、
	 *   aiNodeの階層構造を使用して、ボーンの親子関係を構築する
	 */

	return model;
}
