#pragma once

#include <vector>
#include <cstdint>

#include "GridPosition.h"

/**
 * @brief 地形種類
 */
enum class TerrainType
{
	Grass,
	Dirt,
	Rock,
	Sand,
	Water,
	Forest
};

/**
 * @brief 道路の接続方向
 *
 * @details
 * RoadはTerrainTypeとしてではなく、
 * 地形上に重ねるOverlay情報として管理する。
 */
enum class RoadConnection : uint8_t
{
	None = 0,

	Up = 1 << 0,
	Right = 1 << 1,
	Down = 1 << 2,
	Left = 1 << 3
};

/**
 * @brief 1マス分のTerrain情報
 */
struct TerrainCellData
{
	//=================================================
	// Basic Terrain
	//=================================================

	/**
	 * @brief 地形種類
	 */
	TerrainType Type =
		TerrainType::Grass;

	/**
	 * @brief Grid上の位置
	 */
	GridPosition Position
	{
		0,
		0
	};

	//=================================================
	// Road
	//=================================================

	/**
	 * @brief 道路の接続Bit
	 */
	uint8_t RoadConnections =
		0;

	/**
	 * @brief 指定方向に道路が接続しているか
	 */
	bool HasRoad(
		RoadConnection direction) const
	{
		return
			(RoadConnections &
				static_cast<uint8_t>(
					direction)) != 0;
	}

	/**
	 * @brief 道路が1方向以上存在するか
	 */
	bool HasRoad() const
	{
		return
			RoadConnections !=
			static_cast<uint8_t>(
				RoadConnection::None);
	}

	//=================================================
	// Visual
	//=================================================

	/**
	 * @brief 見た目生成用の固定Seed
	 *
	 * @details
	 * Grass・Rock・Treeなどの配置を
	 * 毎回同じ結果にするために使用する。
	 *
	 * Gameplayには使用しない。
	 */
	uint32_t VisualSeed =
		0;

	//=================================================
	// Water
	//=================================================

	/**
	 * @brief 水の流れるX方向
	 *
	 * Grid座標系基準。
	 */
	float WaterFlowX =
		1.0f;

	/**
	 * @brief 水の流れるY方向
	 */
	float WaterFlowY =
		0.0f;
};

/**
 * @brief Field全体のTerrain情報
 *
 * @details
 * Gameplay用GridMapとは分離し、
 * Fieldの地形・道路・描画生成情報を保持する。
 */
struct TerrainMapData
{
	std::vector<TerrainCellData>
		Cells;

	/**
	 * @brief Terrain情報をクリア
	 */
	void Clear()
	{
		Cells.clear();
	}

	/**
	 * @brief Cell数取得
	 */
	size_t GetCellCount() const
	{
		return
			Cells.size();
	}

	/**
	 * @brief 指定GridのTerrainCellを検索
	 *
	 * @note
	 * 現段階では線形検索。
	 * Terrain生成時やChunk構築時の使用を想定する。
	 *
	 * 毎Frame大量に呼ぶようになった場合は
	 * 2次元配列 / HashMap / Index化へ変更する。
	 */
	TerrainCellData* FindCell(
		const GridPosition& position)
	{
		for (TerrainCellData& cell :
			Cells)
		{
			if (cell.Position.X ==
				position.X &&
				cell.Position.Y ==
				position.Y)
			{
				return &cell;
			}
		}

		return nullptr;
	}

	/**
	 * @brief const版Cell検索
	 */
	const TerrainCellData* FindCell(
		const GridPosition& position) const
	{
		for (const TerrainCellData& cell :
			Cells)
		{
			if (cell.Position.X ==
				position.X &&
				cell.Position.Y ==
				position.Y)
			{
				return &cell;
			}
		}

		return nullptr;
	}
};