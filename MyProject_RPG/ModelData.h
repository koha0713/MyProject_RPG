#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "CommonType.h"
#include "Texture.h"
#include "MeshBuffer.h"
#include "AnimationData.h"
/**
 * @brief モデルデータを保持する構造体
 */

/**
 * @brief モデル頂点情報
 */
struct VertexData {
	Vector3 Position = Vector3{0.0f,0.0f,0.0f};            // 頂点の座標
	Vector3 Normal = Vector3{0.0f,0.0f,0.0f};              // 法線ベクトル
	Vector2 TexCoord = Vector2{0.0f,0.0f};            // テクスチャ座標
	int BoneIndex[4] = {0,0,0,0};            // ボーンインデックス（最大4つ）
	float BoneWeight[4] = {0.0f,0.0f,0.0f,0.0f};         // 各ボーンのウェイト値
	int BoneCount = 0;             // 影響を与えるボーン数
};

/**
 * @brief モデルのマテリアル情報
 */
struct MaterialData
{
	Vector4 Diffuse = 
		Vector4(1.0f,1.0f,1.0f,1.0f);             // 拡散反射色
	Vector4 Ambient = 
		Vector4(0.0f,0.0f,0.0f,1.0f);             // 環境光の色
	Vector4 Specular = 
		Vector4(0.0f,0.0f,0.0f,1.0f);            // 鏡面反射色
	std::string TexturePath;		// テクスチャのパス
	std::shared_ptr<Texture> Texture;  // テクスチャ
};

/**
 * @brief モデルのメッシュ情報
 * @details 1つのMeshBufferと、
 *			使用するマテリアルのインデックスを保持する
 */
struct MeshData
{
	std::vector<VertexData> Vertices;
	std::vector<uint32_t> Indices;

	std::shared_ptr<MeshBuffer> Mesh;

	unsigned int MaterialIndex = 0;

	/**
	 * @brief このMeshが所属しているNodeのIndex
	 *
	 * SkeletonData::BonesのIndexと共通化する。
	 * -1なら所属Node不明。
	 */
	int NodeIndex = -1;

	/**
	 * @brief BoneWeightによるSkinningを行うMeshか
	 */
	bool HasSkinning = false;

	void CreateGpuResources();
};

/**
 * @brief モデルのボーンが頂点へ与えるウェイト情報
 */
struct BoneWeight
{
	std::string BoneName;
	std::string MeshName;

	float Weight = 0.0f;
	int VertexIndex = 0;
};

/**
 * @brief 1Bone分のデータ
 */
struct BoneData
{
	std::string Name;

	int ParentIndex = -1;

	/**
	 * @brief Bind Pose時のLocal Transform
	 */
	Matrix4x4 LocalTransform =
		Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

	/**
	 * @brief Skinning用Offset Matrix
	 *
	 * Skinning対象でないNodeではIdentityのまま。
	 */
	Matrix4x4 OffsetMatrix =
		Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
};

/**
 * @brief Modelが使用するSkeleton情報
 */
struct SkeletonData
{
	std::vector<BoneData> Bones;

	// Bone名からIndexを取得するためのMap
	std::unordered_map<std::string, int>
		BoneMap;
	/**
	 * @brief Scene Root Transformの逆行列
	 * BoneのGlobalTransformをモデル空間へ戻すために使用する。
	 */
	Matrix4x4 GlobalInverseTransform =
		Matrix4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
};

/**
 * @brief モデルデータ
 * @details Assimpファイルから読み込んだモデルデータを保持する構造体
 */
class ModelData
{
public:
	//====================
	// 型エイリアス
	//====================
	using Materials = std::vector<MaterialData>;
	using Meshes = std::vector<MeshData>;
public:
	const Materials& GetMaterials() const 
	{
		return m_Materials; 
	}
	const Meshes& GetMeshes() const
	{
		return m_Meshes;
	}
	const SkeletonData& GetSkeleton() const
	{
		return m_Skeleton;
	}
	Materials& GetMaterials()
	{
		return m_Materials;
	}
	Meshes& GetMeshes()
	{
		return m_Meshes;
	}
	SkeletonData& GetSkeleton()
	{
		return m_Skeleton;
	}
	/*const  SkeletonData& GetSkeleton() const
	{
		return m_Skeleton;
	}*/
	float GetScaleBase() const
	{
		return m_ScaleBase;
	}
	void SetScaleBase(float scale)
	{
		m_ScaleBase = scale;
	}

	/**
	 * @brief Modelファイル自身が持つAnimation一覧
	 */
	std::vector<AnimationClip>&
		GetAnimations()
	{
		return m_Animations;
	}

	const std::vector<AnimationClip>&
		GetAnimations() const
	{
		return m_Animations;
	}

	bool CreateGpuResources(
		ID3D11Device* device);

private:
	//====================
	// メンバ変数
	//====================
	Materials m_Materials; // マテリアル情報の配列
	Meshes m_Meshes;       // メッシュ情報の配列
	SkeletonData m_Skeleton; // スケルトン情報
	
	//====================
	// Animation Resource
	//====================
	std::vector<AnimationClip>
		m_Animations;

	float m_ScaleBase = 1.0f; // モデルのスケール基準値
};
