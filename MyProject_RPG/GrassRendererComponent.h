#pragma once

#include "Component.h"
#include "GrassRenderData.h"
#include "TerrainMapData.h"
#include "TerrainChunk.h"

#include <memory>
#include <vector>

class Texture;
class MeshBuffer;

/**
 * @brief 草の生成・描画Component
 *
 * Terrain Chunk単位でInstanceを管理し、
 * GPU Instancingでまとめて描画する。
 */
class GrassRendererComponent :
	public Component
{
public:

	GrassRendererComponent() =
		default;

	~GrassRendererComponent() override =
		default;

	void Initialize() override;

	void Finalize() override;

	void Update(
		uint64_t delta) override;

	void Draw() override;

	/**
	 * @brief Terrain Chunkから草データを生成
	 */
	bool BuildGrass(
		const std::vector<TerrainChunk>& chunks);

	/**
	 * @brief 草Textureを設定
	 */
	void SetGrassTexture(
		const std::shared_ptr<Texture>& texture)
	{
		m_GrassTexture =
			texture;
	}

	/**
	 * @brief Camera World Position設定
	 *
	 * 草LODに使用する。
	 */
	void SetCameraPosition(
		const Vector3& position)
	{
		m_CameraPosition =
			position;
	}

	void SetCellSize(
		float cellSize)
	{
		m_CellSize =
			cellSize;
	}

	void SetTerrainData(
		const TerrainMapData* terrainData)
	{
		m_TerrainData =
			terrainData;
	}

	/**
	 * @brief Terrain LODから草の描画率を取得する
	 *
	 * @return 0.0～1.0
	 */
	float GetGrassLodRatio(
		TerrainChunkLOD lod) const;

private:

	/**
	 * @brief Cross Billboard用Base Mesh生成
	 */
	bool BuildGrassMesh();

	/**
	 * @brief 1Chunk分のGrass Instanceを生成
	 */
	void BuildGrassChunk(
		const TerrainChunk& chunk,
		GrassChunkRenderData& outChunk);

	/**
	 * @brief Instance Buffer生成
	 */
	bool BuildInstanceBuffer(
		GrassChunkRenderData& chunk);

	/**
	 * @brief Terrainごとの草生成設定
	 */
	GrassSpawnSetting GetGrassSpawnSetting(
		TerrainType type) const;

	/**
	 * @brief 固定乱数Seed生成
	 */
	static uint32_t MakeSeed(
		int gridX,
		int gridY,
		int index);

	/**
	 * @brief Seedから0～1の固定乱数生成
	 */
	static float Random01(
		uint32_t& seed);

	/**
	 * @brief 指定Terrainに草を生成できるか判定
	 */
	bool CanSpawnGrass(
		TerrainType type) const;

	bool CanSpawnGrassAt(
		const GridPosition& position) const;

private:

	std::shared_ptr<MeshBuffer>
		m_GrassMesh;

	std::shared_ptr<Texture>
		m_GrassTexture;

	std::vector<GrassChunkRenderData>
		m_GrassChunks;

	Vector3 m_CameraPosition =
		Vector3(
			0.0f,
			0.0f,
			0.0f);

	/**
	 * @brief Shaderへ送る時間
	 */
	float m_Time =
		0.0f;

	/**
	 * @brief 最初は0で静止表示を確認する
	 */
	float m_WindStrength =
		0.1f;

	float m_WindSpeed =
		0.5f;

	Vector2 m_WindDirection =
		Vector2(
			1.0f,
			0.25f);

	float m_CellSize =
		1.0f;

	const TerrainMapData*
		m_TerrainData =
		nullptr;

	/**
	 * @brief Terrain Chunkへの非所有参照
	 *
	 * GrassのLOD判定に使用する。
	 * TerrainRenderer側がGrassRendererより長く生存する前提。
	 */
	const std::vector<TerrainChunk>*
		m_TerrainChunks =
		nullptr;
};