#pragma once

#include <vector>
#include <memory>

#include "WaterRenderSettings.h"

#include "Component.h"
#include "TerrainMapData.h"
#include "TerrainChunk.h"
#include "TerrainChunkMesh.h"
#include "ModelData.h"
#include "WaterRenderData.h"
#include "TerrainRenderSettings.h"

class Texture;

/**
 * @brief Field Terrain描画Component
 *
 * @details
 * TerrainMapDataをChunkへ分割し、
 * Chunk単位のMeshとしてRendererへ渡す。
 *
 * Gameplay用GridMapには関与しない。
 */
class TerrainRendererComponent :
	public Component
{
public:

	TerrainRendererComponent() = default;
	~TerrainRendererComponent() override = default;

	//====================
	// Lifecycle
	//====================

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	void DrawDebugUI() override;

	//====================
	// Terrain
	//====================

	void SetTerrainData(
		const TerrainMapData* terrainData)
	{
		m_TerrainData =
			terrainData;
	}

	/**
	 * @brief 1GridのWorldサイズ設定
	 */
	void SetCellSize(
		float cellSize)
	{
		m_CellSize =
			cellSize;
	}

	/**
	 * @brief Terrain Meshを構築
	 */
	bool BuildTerrain();

	const char*
		GetComponentName() const override
	{
		return
			"TerrainRendererComponent";
	}

	/**
	 * @brief Terrain LOD判定に使用するCamera位置
	 */
	void SetCameraPosition(
		const Vector3& position)
	{
		m_CameraPosition =
			position;
	}

	/**
	 * @brief Water Normal Map設定
	 */
	void SetWaterNormalMaps(
		const std::shared_ptr<Texture>& normalA,
		const std::shared_ptr<Texture>& normalB)
	{
		m_WaterNormalMapA =
			normalA;

		m_WaterNormalMapB =
			normalB;
	}

	/**
	 * @brief Terrain Textureを設定
	 */
	void SetTerrainTextures(
		const std::shared_ptr<Texture>& grass,
		const std::shared_ptr<Texture>& dirt,
		const std::shared_ptr<Texture>& rock,
		const std::shared_ptr<Texture>& sand)
	{
		m_GrassTexture =
			grass;

		m_DirtTexture =
			dirt;

		m_RockTexture =
			rock;

		m_SandTexture =
			sand;
	}

	float GetTerrainTextureIndex(
		TerrainType type) const;

private:

	/**
	 * @brief Chunk生成
	 */
	bool BuildChunks();

	/**
	 * @brief CPU Ground Mesh生成
	 */
	bool BuildGroundMeshes();

	/**
	 * @brief GPU Buffer生成
	 */
	bool BuildGpuResources();

	/**
	 * @brief 1Chunk分のGround Mesh生成
	 */
	void BuildGroundMesh(
		const TerrainChunk& chunk,
		TerrainGroundChunkMesh& outMesh);

	/**
	 * @brief Water Mesh生成
	 */
	bool BuildWaterMeshes();

	/**
	 * @brief 1Chunk分のWater Mesh生成
	 */
	void BuildWaterMesh(
		const TerrainChunk& chunk,
		TerrainWaterChunkMesh& outMesh);

	/**
	 * @brief RiverBed Meshを生成
	 *
	 * @return 1つ以上生成できた場合true
	 */
	bool BuildRiverBedMeshes();

	/**
	 * @brief 1Chunk分のRiverBed Meshを生成
	 */
	void BuildRiverBedMesh(
		const TerrainChunk& chunk,
		TerrainChunkMesh& outMesh);

	/**
	 * @brief Groundとして描画する地形か
	 */
	static bool IsGroundTerrain(
		TerrainType type);

	/**
	 * @brief Camera距離からChunk LODを更新
	 */
	void UpdateChunkLOD();

	/**
	 * @brief Water Cell単位の水底高さを計算する
	 *
	 * @details
	 * 岸から離れるほど深くし、
	 * 固定ノイズで小さな凹凸を加える。
	 */
	float CalculateRiverBedCellHeight(
		const TerrainCellData& cell) const;

	/**
	 * @brief 水底頂点の高さを計算する
	 *
	 * @details
	 * 頂点を共有する周囲最大4つのWater Cellの高さを平均し、
	 * Cell境界の段差を滑らかにする。
	 */
	float CalculateRiverBedVertexHeight(
		int vertexGridX,
		int vertexGridY) const;

	/**
	 * @brief 指定Water Cellから岸までのおおよその距離を取得する
	 */
	int CalculateDistanceFromShore(
		const GridPosition& position,
		int maxDistance) const;

	/**
	 * @brief 任意のGrid空間位置における水底高さを計算
	 *
	 * @param gridX Grid空間X
	 * @param gridY Grid空間Y
	 */
	float CalculateRiverBedHeight(
		float gridX,
		float gridY) const;

	/**
	 * @brief 水底頂点の法線を計算する
	 *
	 * @details
	 * Grid空間上の前後左右の高さ差から
	 * X方向・Z方向の傾きを求め、
	 * 外積で法線を計算する。
	 */
	Vector3 CalculateRiverBedNormal(
		float gridX,
		float gridY) const;

private:

	/**
	 * @brief FieldScene側所有
	 */
	const TerrainMapData*
		m_TerrainData =
		nullptr;

	/**
	 * @brief Grid1マスのWorldサイズ
	 */
	float m_CellSize =
		1.0f;

	/**
	 * @brief Terrain Chunk
	 */
	std::vector<TerrainChunk>
		m_Chunks;

	/**
	 * @brief ChunkごとのGround Mesh
	 */
	std::vector<TerrainGroundChunkMesh>
		m_GroundMeshes;

	/**
	 * @brief Ground共通Material
	 *
	 * 現段階ではGrass色一色。
	 * 後でTerrain Textureへ置き換える。
	 */
	MaterialData
		m_GroundMaterial;

	/**
	 * @brief Chunkごとの水面Mesh
	 *
	 * Water専用Vertexを使用し、
	 * 頂点ごとに水深情報を保持する。
	 */
	std::vector<TerrainWaterChunkMesh>
		m_WaterMeshes;

	Vector3 m_CameraPosition
	{
		0.0f,
		0.0f,
		0.0f
	};

	//====================
	// LOD Distance
	//====================

	float m_NearDistance =
		20.0f;

	float m_MiddleDistance =
		40.0f;

	float m_FarDistance =
		70.0f;

	/**
	 * @brief Water Animation用経過時間
	 */
	float m_WaterTime =
		0.0f;

	/**
	 * @brief Water Flow速度
	 */
	float m_WaterFlowSpeed =
		0.15f;

	/**
	 * @brief Water用Normal Map
	 */
	std::shared_ptr<Texture>
		m_WaterNormalMapA;

	std::shared_ptr<Texture>
		m_WaterNormalMapB;

	/**
	 * @brief Water描画設定
	 */
	WaterRenderSettings
		m_WaterSettings;

	/**
	 * @brief Chunkごとの川底Mesh
	 */
	std::vector<TerrainChunkMesh>
		m_RiverBedMeshes;

	/**
	 * @brief 川底用Material
	 */
	MaterialData
		m_RiverBedMaterial;

	/**
	 * @brief TerrainGroundTexture
	 */
	std::shared_ptr<Texture>
		m_GrassTexture;

	std::shared_ptr<Texture>
		m_DirtTexture;

	std::shared_ptr<Texture>
		m_RockTexture;

	std::shared_ptr<Texture>
		m_SandTexture;

	TerrainRenderSettings
		m_TerrainSettings;

};