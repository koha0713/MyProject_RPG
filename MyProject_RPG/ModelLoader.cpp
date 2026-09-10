#include "ModelLoader.h"
#include "CommonType.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "assimp/material.h"

#include <iostream>
#include <cassert>
#include <filesystem>

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
 * @brief Assimpから取得したTextureパスを実ファイルへ解決する
 *
 * @details
 * 配布FBXでは、作成者PC上の絶対パスや
 * 不正な相対パスが保存されている場合がある。
 *
 * 以下の順で探索する。
 * 1. 指定パスそのまま
 * 2. モデルディレクトリ + 指定パス
 * 3. モデルディレクトリ + ファイル名のみ
 */
	std::string ResolveTexturePath(
		const std::filesystem::path& modelDirectory,
		const std::string& texturePath)
	{
		if (texturePath.empty())
		{
			return {};
		}

		const std::filesystem::path originalPath =
			texturePath;

		//====================
		// 1. 指定パスそのまま
		//====================

		if (std::filesystem::exists(originalPath))
		{
			return originalPath
				.lexically_normal()
				.string();
		}

		//====================
		// 2. モデルディレクトリ + 指定パス
		//====================

		const std::filesystem::path relativePath =
			modelDirectory /
			originalPath;

		if (std::filesystem::exists(relativePath))
		{
			return relativePath
				.lexically_normal()
				.string();
		}

		//====================
		// 3. ファイル名だけ取り出して探索
		//====================

		const std::filesystem::path fileName =
			originalPath.filename();

		const std::filesystem::path sameDirectoryPath =
			modelDirectory /
			fileName;

		if (std::filesystem::exists(sameDirectoryPath))
		{
			return sameDirectoryPath
				.lexically_normal()
				.string();
		}

		//====================
		// 4. モデルディレクトリ/Textures/ファイル名
		//====================

		const std::filesystem::path textureDirectoryPath =
			modelDirectory /
			"Textures" /
			fileName;

		if (std::filesystem::exists(
			textureDirectoryPath))
		{
			return textureDirectoryPath
				.lexically_normal()
				.string();
		}

		// 見つからなかった
		return {};
	}

	/**
	 * @brief Assimp MaterialをMaterialDataへ変換
	 *
	 * @param aiMaterial Assimp Material
	 * @param modelDirectory Modelファイルが存在するディレクトリ
	 */
	MaterialData LoadMaterial(
		const aiMaterial* aiMaterial,
		const std::filesystem::path& modelDirectory)
	{
		MaterialData result{};

		aiColor4D color{};

		//====================
		// Diffuse
		//====================

		if (AI_SUCCESS ==
			aiGetMaterialColor(
				aiMaterial,
				AI_MATKEY_COLOR_DIFFUSE,
				&color))
		{
			result.Diffuse =
				Vector4(
					color.r,
					color.g,
					color.b,
					color.a);
		}

		//====================
		// Ambient
		//====================

		if (AI_SUCCESS ==
			aiGetMaterialColor(
				aiMaterial,
				AI_MATKEY_COLOR_AMBIENT,
				&color))
		{
			result.Ambient =
				Vector4(
					color.r,
					color.g,
					color.b,
					color.a);
		}

		//====================
		// Specular
		//====================

		if (AI_SUCCESS ==
			aiGetMaterialColor(
				aiMaterial,
				AI_MATKEY_COLOR_SPECULAR,
				&color))
		{
			result.Specular =
				Vector4(
					color.r,
					color.g,
					color.b,
					color.a);
		}

		//====================
		// Diffuse Texture
		//====================

		if (aiMaterial->GetTextureCount(
			aiTextureType_DIFFUSE) > 0)
		{
			aiString aiTexturePath;

			if (AI_SUCCESS ==
				aiMaterial->GetTexture(
					aiTextureType_DIFFUSE,
					0,
					&aiTexturePath))
			{
				//========================================
				// AssimpがFBXから取得したTextureパス確認
				//========================================
				OutputDebugStringA(
					"[ModelLoader] Assimp texture path: ");

				OutputDebugStringA(
					aiTexturePath.C_Str());

				OutputDebugStringA("\n");

				const std::string texturePath =
					aiTexturePath.C_Str();

				OutputDebugStringA(
					"[ModelLoader] Assimp texture path: ");

				OutputDebugStringA(
					aiTexturePath.C_Str());

				OutputDebugStringA("\n");

				// Embedded Textureは現段階では未対応
				if (!texturePath.empty() &&
					texturePath[0] != '*')
				{
					result.TexturePath =
						ResolveTexturePath(
							modelDirectory,
							texturePath);

					if (result.TexturePath.empty())
					{
						OutputDebugStringA(
							"[ModelLoader] texture not found: ");

						OutputDebugStringA(
							texturePath.c_str());

						OutputDebugStringA("\n");
					}
				}
			}
		}



		return result;
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
		aiProcess_SortByPType | // プリミティブタイプごとにソート
		aiProcess_FlipWindingOrder;	// 頂点の順序を反転（右手系→左手系）

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

	//====================
	// Modelディレクトリ
	//====================

	const std::filesystem::path
		modelDirectory =
		std::filesystem::path(filePath)
		.parent_path();

	//====================
	// Material読み込み
	//====================

	for (unsigned int i = 0;
		i < scene->mNumMaterials;
		++i)
	{
		MaterialData material =
			LoadMaterial(
				scene->mMaterials[i],
				modelDirectory);

		std::string message =
			"[ModelLoader] Material[" +
			std::to_string(i) +
			"] Texture: " +
			material.TexturePath +
			"\n";

		OutputDebugStringA(
			message.c_str());

		model->GetMaterials().
			emplace_back(
				std::move(material));
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
			else
			{
				vertex.TexCoord =
					Vector2(
						0.0f,
						0.0f);
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

		//========================================
		// Meshが使用するMaterial番号を確認
		//========================================

		std::string message =
			"[ModelLoader] Mesh MaterialIndex: " +
			std::to_string(
				meshData.MaterialIndex) +
			"\n";

		OutputDebugStringA(
			message.c_str());

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
