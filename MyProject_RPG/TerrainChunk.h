#pragma once

#include <vector>

#include "GridPosition.h"
#include "TerrainMapData.h"
#include "TerrainChunkLOD.h"

/**
 * @brief Terrain描画単位
 *
 * @details
 * 複数のTerrainCellをまとめ、
 * Ground / Water / Grass等の描画単位として使用する。
 *
 * 1マス単位ではなくChunk単位にすることで、
 * 将来のFrustum Culling / LOD / Instancingに対応しやすくする。
 */
class TerrainChunk
{
public:

	static constexpr int CHUNK_SIZE =
		8;

public:

	TerrainChunk() = default;
	~TerrainChunk() = default;

	/**
	 * @brief Chunkを初期化
	 *
	 * @param chunkX Chunk座標X
	 * @param chunkY Chunk座標Y
	 */
	void Initialize(
		int chunkX,
		int chunkY);

	/**
	 * @brief Chunk内容を破棄
	 */
	void Clear();

	/**
	 * @brief TerrainCellを登録
	 *
	 * @note
	 * TerrainMapDataが本体を所有するため、
	 * pointerは非所有。
	 */
	void AddCell(
		const TerrainCellData* cell);

	/**
	 * @brief 指定GridがこのChunkに属するか
	 */
	bool Contains(
		const GridPosition& position) const;

	/**
	 * @brief Chunk左上Grid
	 */
	GridPosition GetStartGrid() const;

	/**
	 * @brief Chunk中央Grid座標
	 *
	 * LOD距離判定などで使用する。
	 */
	GridPosition GetCenterGrid() const;

	int GetChunkX() const
	{
		return m_ChunkX;
	}

	int GetChunkY() const
	{
		return m_ChunkY;
	}

	const std::vector<const TerrainCellData*>&
		GetCells() const
	{
		return m_Cells;
	}

	void SetLOD(
		TerrainChunkLOD lod)
	{
		m_LOD =
			lod;
	}

	TerrainChunkLOD GetLOD() const
	{
		return m_LOD;
	}

	bool IsVisible() const
	{
		return
			m_LOD !=
			TerrainChunkLOD::Culled;
	}

private:

	int m_ChunkX =
		0;

	int m_ChunkY =
		0;

	/**
	 * @brief このChunkに属するTerrainCell
	 *
	 * TerrainMapData所有。
	 */
	std::vector<const TerrainCellData*>
		m_Cells;

	/**
	 * @brief 現在の描画LOD
	 */
	TerrainChunkLOD
		m_LOD =
		TerrainChunkLOD::Near;
};