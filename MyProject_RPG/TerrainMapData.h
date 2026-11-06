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
    // 地形種類
    TerrainType Type = TerrainType::Grass;

    // Grid上の位置
    GridPosition Position{ 0, 0 };

    // 道路の接続情報
    uint8_t RoadConnections = 0;

    /// <summary>
    /// 指定方向に道路が接続しているか
    /// </summary>
    bool HasRoad(RoadConnection direction) const
    {
        return (RoadConnections &
            static_cast<uint8_t>(direction)) != 0;
    }
};

/**
 * @brief Field全体のTerrain情報
 */
struct TerrainMapData
{
	std::vector<TerrainCellData>
		Cells;

	void Clear()
	{
		Cells.clear();
	}
};