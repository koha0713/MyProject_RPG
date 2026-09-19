#pragma once

#include <vector>

#include "GridPosition.h"

/**
 * @brief 地形種類
 */
enum class TerrainType
{
	Grass,
	Dirt,
	Rock,
	Water
};

/**
 * @brief 1マス分のTerrain情報
 */
struct TerrainCellData
{
	// 地形種類
	TerrainType Type =
		TerrainType::Grass;

	// Grid上の位置
	GridPosition Position
	{
		0,
		0
	};
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