#pragma once

/**
 * @file GridRangeCalculator.h
 * @brief Grid上の移動・攻撃可能範囲を計算する
 */

#include <vector>

#include "GridPosition.h"

class GridMap;

class GridRangeCalculator
{
public:

	/**
	 * @brief 移動可能範囲を取得する
	 *
	 * @param gridMap GridMap
	 * @param start 開始地点
	 * @param movePoints 使用可能MovePoint
	 */
	static std::vector<GridPosition>
		CalculateMoveRange(
			const GridMap& gridMap,
			const GridPosition& start,
			int movePoints);

	/**
	 * @brief 上下左右の攻撃範囲を取得する
	 */
	static std::vector<GridPosition>
		CalculateAttackRange(
			const GridMap& gridMap,
			const GridPosition& center,
			int attackRange = 1);
};