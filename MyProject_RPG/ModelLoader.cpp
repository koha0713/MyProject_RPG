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
	 * @details
	 * 配布FBXでは、作成者PC上の絶対パスや
	 * 不正な相対パスが保存されている場合がある。
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

	/**
	 * @brief Assimp AnimationをAnimationClipへ変換
	 * @param aiAnimation Assimp Animation
	 * @return AnimationClip
	 */
	AnimationClip ConvertAnimation(
		const aiAnimation* aiAnimation)
	{
		AnimationClip clip{};

		if (!aiAnimation)
		{
			return clip;
		}

		//====================
		// 基本情報
		//====================

		clip.Name =
			aiAnimation->mName.C_Str();

		clip.Duration =
			aiAnimation->mDuration;

		clip.TicksPerSecond =
			aiAnimation->mTicksPerSecond;

		//====================
		// Bone / Node Animation
		//====================

		clip.BoneAnimations.reserve(
			aiAnimation->mNumChannels);

		for (unsigned int channelIndex = 0;
			channelIndex < aiAnimation->mNumChannels;
			++channelIndex)
		{
			const aiNodeAnim* channel =
				aiAnimation->mChannels[
					channelIndex];

			if (!channel)
			{
				continue;
			}

			BoneAnimation boneAnimation{};

			boneAnimation.BoneName =
				channel->mNodeName.C_Str();

			//========================================
			// Position
			//========================================

			boneAnimation.PositionKeys.reserve(
				channel->mNumPositionKeys);

			for (unsigned int keyIndex = 0;
				keyIndex < channel->mNumPositionKeys;
				++keyIndex)
			{
				const aiVectorKey& aiKey =
					channel->mPositionKeys[
						keyIndex];

				VectorKeyFrame key{};

				key.Time =
					aiKey.mTime;

				key.Value =
					Vector3(
						aiKey.mValue.x,
						aiKey.mValue.y,
						aiKey.mValue.z);

				boneAnimation.PositionKeys.
					emplace_back(
						std::move(key));
			}

			//========================================
			// Rotation
			//========================================

			boneAnimation.RotationKeys.reserve(
				channel->mNumRotationKeys);

			for (unsigned int keyIndex = 0;
				keyIndex < channel->mNumRotationKeys;
				++keyIndex)
			{
				const aiQuatKey& aiKey =
					channel->mRotationKeys[
						keyIndex];

				QuaternionKeyFrame key{};

				key.Time =
					aiKey.mTime;

				// Assimp Quaternion:
				// w, x, y, z
				//
				// SimpleMath Quaternion ctor:
				// x, y, z, w
				key.Value =
					Quaternion(
						aiKey.mValue.x,
						aiKey.mValue.y,
						aiKey.mValue.z,
						aiKey.mValue.w);

				boneAnimation.RotationKeys.
					emplace_back(
						std::move(key));
			}

			//========================================
			// Scale
			//========================================

			boneAnimation.ScaleKeys.reserve(
				channel->mNumScalingKeys);

			for (unsigned int keyIndex = 0;
				keyIndex < channel->mNumScalingKeys;
				++keyIndex)
			{
				const aiVectorKey& aiKey =
					channel->mScalingKeys[
						keyIndex];

				VectorKeyFrame key{};

				key.Time =
					aiKey.mTime;

				key.Value =
					Vector3(
						aiKey.mValue.x,
						aiKey.mValue.y,
						aiKey.mValue.z);

				boneAnimation.ScaleKeys.
					emplace_back(
						std::move(key));
			}

			clip.BoneAnimations.
				emplace_back(
					std::move(
						boneAnimation));
		}

		return clip;
	}

	/**
	 * @brief aiNode階層からSkeletonを構築する
	 * @param node 現在のNode
	 * @param parentIndex 親Bone番号
	 * @param skeleton 出力Skeleton
	 */
	void BuildSkeleton(
		const aiNode* node,
		int parentIndex,
		SkeletonData& skeleton)
	{
		if (!node)
		{
			return;
		}

		//====================
		// Bone登録
		//====================

		BoneData bone{};

		bone.Name =
			node->mName.C_Str();

		bone.ParentIndex =
			parentIndex;

		bone.LocalTransform =
			ConvertMatrix(
				node->mTransformation);

		const int currentIndex =
			static_cast<int>(
				skeleton.Bones.size());

		skeleton.Bones.emplace_back(
			std::move(bone));

		// Bone名からIndexを引けるようにする
		skeleton.BoneMap[
			skeleton.Bones[currentIndex].Name] =
			currentIndex;

			//====================
			// 子Node
			//====================

			for (unsigned int i = 0;
				i < node->mNumChildren;
				++i)
			{
				BuildSkeleton(
					node->mChildren[i],
					currentIndex,
					skeleton);
			}
	}

	/**
	 * @brief Meshが持つBoneのOffsetMatrixをSkeletonへ登録する
	 */
	void LoadBoneOffsetMatrices(
		const aiMesh* aiMesh,
		SkeletonData& skeleton)
	{
		if (!aiMesh)
		{
			return;
		}

		for (unsigned int boneIndex = 0;
			boneIndex < aiMesh->mNumBones;
			++boneIndex)
		{
			const aiBone* aiBone =
				aiMesh->mBones[boneIndex];

			if (!aiBone)
			{
				continue;
			}

			const std::string boneName =
				aiBone->mName.C_Str();

			const auto it =
				skeleton.BoneMap.find(
					boneName);

			if (it ==
				skeleton.BoneMap.end())
			{
				continue;
			}

			const int skeletonIndex =
				it->second;

			skeleton.Bones[
				skeletonIndex].
				OffsetMatrix =
					ConvertMatrix(
						aiBone->mOffsetMatrix);
		}
	}

	/**
	 * @brief VertexへBoneIndex / BoneWeightを追加する
	 */
	void AddBoneWeight(
		VertexData& vertex,
		int boneIndex,
		float weight)
	{
		// Weightがない場合は無視
		if (weight <= 0.0f)
		{
			return;
		}

		//====================
		// 空きSlotへ追加
		//====================

		if (vertex.BoneCount < 4)
		{
			const int index =
				vertex.BoneCount;

			vertex.BoneIndex[index] =
				boneIndex;

			vertex.BoneWeight[index] =
				weight;

			++vertex.BoneCount;

			return;
		}

		//====================
		// 既に4Boneある場合
		//====================
		//
		// 一番Weightが小さいBoneを探し、
		// 今回のWeightの方が大きければ置換する。
		//

		int minIndex = 0;

		for (int i = 1;
			i < 4;
			++i)
		{
			if (vertex.BoneWeight[i] <
				vertex.BoneWeight[minIndex])
			{
				minIndex = i;
			}
		}

		if (weight >
			vertex.BoneWeight[minIndex])
		{
			vertex.BoneIndex[minIndex] =
				boneIndex;

			vertex.BoneWeight[minIndex] =
				weight;
		}
	}

	/**
	 * @brief VertexのBoneWeight合計を1.0へ正規化する
	 */
	void NormalizeBoneWeights(
		VertexData& vertex)
	{
		float totalWeight = 0.0f;

		for (int i = 0;
			i < vertex.BoneCount;
			++i)
		{
			totalWeight +=
				vertex.BoneWeight[i];
		}

		if (totalWeight <= 0.0f)
		{
			return;
		}

		for (int i = 0;
			i < vertex.BoneCount;
			++i)
		{
			vertex.BoneWeight[i] /=
				totalWeight;
		}
	}

	/**
	 * @brief aiMeshのBoneWeightをVertexDataへ設定する
	 */
	void LoadVertexBoneWeights(
		const aiMesh* aiMesh,
		const SkeletonData& skeleton,
		std::vector<VertexData>& vertices)
	{
		if (!aiMesh)
		{
			return;
		}

		//====================
		// Bone
		//====================

		for (unsigned int boneIndex = 0;
			boneIndex < aiMesh->mNumBones;
			++boneIndex)
		{
			const aiBone* aiBone =
				aiMesh->mBones[boneIndex];

			if (!aiBone)
			{
				continue;
			}

			const std::string boneName =
				aiBone->mName.C_Str();

			const auto it =
				skeleton.BoneMap.find(
					boneName);

			if (it ==
				skeleton.BoneMap.end())
			{
				continue;
			}

			const int skeletonBoneIndex =
				it->second;

			//====================
			// Vertex Weight
			//====================

			for (unsigned int weightIndex = 0;
				weightIndex < aiBone->mNumWeights;
				++weightIndex)
			{
				const aiVertexWeight& aiWeight =
					aiBone->mWeights[
						weightIndex];

				const unsigned int vertexIndex =
					aiWeight.mVertexId;

				if (vertexIndex >=
					vertices.size())
				{
					continue;
				}

				AddBoneWeight(
					vertices[vertexIndex],
					skeletonBoneIndex,
					aiWeight.mWeight);
			}
		}

		//====================
		// Weight正規化
		//====================

		for (auto& vertex : vertices)
		{
			NormalizeBoneWeights(
				vertex);
		}
	}

	/**
	 * @brief Matrixの逆行列を取得
	 */
	Matrix4x4 InverseMatrix(
		const Matrix4x4& matrix)
	{
		using namespace DirectX;

		const XMMATRIX xmMatrix =
			XMLoadFloat4x4(
				&matrix);

		const XMMATRIX inverseMatrix =
			XMMatrixInverse(
				nullptr,
				xmMatrix);

		Matrix4x4 result;

		XMStoreFloat4x4(
			&result,
			inverseMatrix);

		return result;
	}

	/**
 * @brief Assimp Node階層からMeshとNodeの対応を構築する
 *
 * @param node 現在Node
 * @param skeleton Node一覧
 * @param meshNodeIndices Scene MeshIndex → Skeleton NodeIndex
 */
	void BuildMeshNodeMap(
		const aiNode* node,
		const SkeletonData& skeleton,
		std::vector<int>& meshNodeIndices)
	{
		if (!node)
		{
			return;
		}

		//====================
		// Node Index取得
		//====================

		const auto nodeIt =
			skeleton.BoneMap.find(
				node->mName.C_Str());

		int nodeIndex = -1;

		if (nodeIt != skeleton.BoneMap.end())
		{
			nodeIndex =
				nodeIt->second;
		}

		//====================
		// このNodeが持つMeshを登録
		//====================

		for (unsigned int i = 0;
			i < node->mNumMeshes;
			++i)
		{
			const unsigned int meshIndex =
				node->mMeshes[i];

			if (meshIndex <
				meshNodeIndices.size())
			{
				meshNodeIndices[meshIndex] =
					nodeIndex;
			}
		}

		//====================
		// 子Node
		//====================

		for (unsigned int i = 0;
			i < node->mNumChildren;
			++i)
		{
			BuildMeshNodeMap(
				node->mChildren[i],
				skeleton,
				meshNodeIndices);
		}
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

	//=====================================================
	// Skeleton構築
	//=====================================================

	SkeletonData& skeleton =
		model->GetSkeleton();

	skeleton.GlobalInverseTransform =
		InverseMatrix(
			ConvertMatrix(
				scene->mRootNode->mTransformation));

	BuildSkeleton(
		scene->mRootNode,
		-1,
		skeleton);

	//=====================================================
// Mesh → Node 対応
//=====================================================

	std::vector<int> meshNodeIndices(
		scene->mNumMeshes,
		-1);

	BuildMeshNodeMap(
		scene->mRootNode,
		skeleton,
		meshNodeIndices);

	for (size_t i = 0;
		i < skeleton.Bones.size();
		++i)
	{
		const auto& bone =
			skeleton.Bones[i];

		std::string message =
			"[Skeleton] [" +
			std::to_string(i) +
			"] " +
			bone.Name +
			"\n";

		OutputDebugStringA(
			message.c_str());
	}

	//=====================
	// メッシュの読み込み
	//=====================
	for(unsigned int i = 0;
		i < scene->mNumMeshes; ++i)
	{
		const aiMesh* aiMesh =
			scene->mMeshes[i];
		//========================================
		// Mesh情報のデバッグ出力
		//========================================

		std::string meshInfo =
			"[ModelLoader] Mesh[" +
			std::to_string(i) +
			"] Name: " +
			aiMesh->mName.C_Str() +
			" Bones: " +
			std::to_string(aiMesh->mNumBones) +
			" Vertices: " +
			std::to_string(aiMesh->mNumVertices) +
			" MaterialIndex: " +
			std::to_string(aiMesh->mMaterialIndex) +
			"\n";

		OutputDebugStringA(
			meshInfo.c_str());


		//====================
		// Bone OffsetMatrix
		//====================
		LoadBoneOffsetMatrices(
			aiMesh,
			model->GetSkeleton());
		
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

		//=================================================
		// Bone Weight
		//=================================================
		LoadVertexBoneWeights(
			aiMesh,
			model->GetSkeleton(),
			vertices);
		//========================================
		// Skinning対象Vertex数確認
		//========================================

		size_t skinnedVertexCount = 0;

		for (const auto& vertex : vertices)
		{
			float totalWeight = 0.0f;

			for (int bone = 0;
				bone < 4;
				++bone)
			{
				totalWeight +=
					vertex.BoneWeight[bone];
			}

			if (totalWeight > 0.0001f)
			{
				++skinnedVertexCount;
			}
		}

		std::string weightMessage =
			"[ModelLoader] Mesh[" +
			std::to_string(i) +
			"] SkinnedVertices: " +
			std::to_string(skinnedVertexCount) +
			" / " +
			std::to_string(vertices.size()) +
			"\n";

		OutputDebugStringA(
			weightMessage.c_str());

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

		//=====================================================
		// MeshData作成
		//=====================================================

		MeshData meshData;

		//=====================================================
		// Mesh → Node
		//=====================================================
		//
		// aiNode::mMeshes から取得したNodeIndexを保持する。
		// SkinningされないMeshでも、Animation中に所属Nodeの
		// Transformへ追従させるために使用する。
		//
		meshData.NodeIndex =
			meshNodeIndices[i];

		//=====================================================
		// Skinning判定
		//=====================================================
		//
		// aiBoneを1つ以上持つMeshはGPU Skinning対象。
		// Boneを持たないMeshはNode Transformで描画する。
		//
		meshData.HasSkinning =
			aiMesh->mNumBones > 0;

		//=====================================================
		// CPU MeshData
		//=====================================================

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

	//=====================================================
	// Animation読み込み
	//=====================================================

	if (scene->HasAnimations())
	{
		model->GetAnimations().reserve(
			scene->mNumAnimations);

		for (unsigned int i = 0;
			i < scene->mNumAnimations;
			++i)
		{
			const aiAnimation* aiAnimation =
				scene->mAnimations[i];

			if (!aiAnimation)
			{
				continue;
			}

			model->GetAnimations().
				emplace_back(
					ConvertAnimation(
						aiAnimation));
		}
	}

	return model;
}

/**
 * @brief AnimationだけをFBXから読み込む
 */
bool ModelLoader::LoadAnimations(
	const std::string& filePath,
	std::vector<AnimationClip>& outAnimations)
{
	// 呼び出し前の内容を残さない
	outAnimations.clear();

	if (filePath.empty())
	{
		return false;
	}

	Assimp::Importer importer;

	//====================
	// Animationファイル読み込み
	//====================
	// Animation情報だけが目的なので、
	// Model用のTriangulateなどは基本的に不要。
	const aiScene* scene =
		importer.ReadFile(
			filePath,
			0);

	if (!scene)
	{
		OutputDebugStringA(
			"[ModelLoader::LoadAnimations] Assimp load failed.\n");

		OutputDebugStringA(
			importer.GetErrorString());

		OutputDebugStringA("\n");

		return false;
	}

	//====================
	// Animation確認
	//====================

	if (!scene->HasAnimations() ||
		scene->mNumAnimations == 0)
	{
		OutputDebugStringA(
			"[ModelLoader::LoadAnimations] No animations found.\n");

		return false;
	}

	outAnimations.reserve(
		scene->mNumAnimations);

	//====================
	// Animation変換
	//====================

	for (unsigned int i = 0;
		i < scene->mNumAnimations;
		++i)
	{
		const aiAnimation* aiAnimation =
			scene->mAnimations[i];

		if (!aiAnimation)
		{
			continue;
		}

		AnimationClip clip =
			ConvertAnimation(
				aiAnimation);

		outAnimations.emplace_back(
			std::move(clip));
	}

	//====================
	// Debug
	//====================

	for (size_t i = 0;
		i < outAnimations.size();
		++i)
	{
		const AnimationClip& clip =
			outAnimations[i];

		std::string message =
			"[ModelLoader] Animation[" +
			std::to_string(i) +
			"] Name: " +
			(clip.Name.empty()
				? "(Unnamed)"
				: clip.Name) +
			" Duration: " +
			std::to_string(
				clip.Duration) +
			" TPS: " +
			std::to_string(
				clip.TicksPerSecond) +
			" Channels: " +
			std::to_string(
				clip.BoneAnimations.size()) +
			"\n";

		OutputDebugStringA(
			message.c_str());
	}

	return !outAnimations.empty();
}