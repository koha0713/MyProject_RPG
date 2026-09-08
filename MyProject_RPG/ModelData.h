#pragma once
#include <string>
#include <memory>

#include "CommonType.h"
#include "Texture.h"
#include "MeshBuffer.h"
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
	unsigned int MaterialIndex = 0; // インデックス数
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
 * @brief ボーン情報
 */
struct BoneData
{
	std::string BoneName;        // ボーン名
	std::string MeshName;  // ボーンが属するメッシュ名
	std::string ArmatureName; // ボーンが属するアーマチュア名

	Matrix4x4 Matrix;             // ボーンの変換行列
	Matrix4x4 AnimationMatrix;    // ボーンのアニメーション行列
	Matrix4x4 OffsetMatrix;       // ボーンのオフセット行列

	int Index = 0;                  // ボーンのインデックス

	std::vector<BoneWeight> Weights; // ボーンが影響を与える頂点のウェイト情報
};

/**
 * @brief スケルトン情報
 */
struct SkeletonData
{
	std::vector<BoneData> Bones; // ボーン情報の配列
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
	float GetScaleBase() const
	{
		return m_ScaleBase;
	}
	void SetScaleBase(float scale)
	{
		m_ScaleBase = scale;
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

	float m_ScaleBase = 1.0f; // モデルのスケール基準値
};
