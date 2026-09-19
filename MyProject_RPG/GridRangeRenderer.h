#pragma once

/**
 * @file GridRangeRenderer.h
 * @brief Grid上の範囲表示を行うRenderer
 */

#include <vector>

#include "CommonType.h"
#include "GridPosition.h"

class GridMap;

/**
 * @brief Grid範囲表示
 */
class GridRangeRenderer
{
public:

	/**
	 * @brief 指定されたGrid Cell群を半透明面として描画する
	 */
	static void DrawCells(
		const GridMap& gridMap,
		const std::vector<GridPosition>& cells,
		const Color& color,
		float heightOffset = 0.03f);
};