#pragma once

/**
 * @file GridPathFinder.h
 * @brief GridMap上の経路探索を行うUtility
 */

#include <vector>

#include "GridPosition.h"

class GridMap;

/**
 * @brief Grid Path Finder
 *
 * @details
 * GridMap上でA*アルゴリズムを使用し、
 * StartからGoalまでの最短経路を探索する。
 *
 * 現在は上下左右4方向のみ移動可能で、
 * 1マスあたりの移動コストは1として扱う。
 *
 * 将来的にはTerrainMapと連携し、
 * 地形ごとの移動コストにも対応できる。
 */
class GridPathFinder
{
public:

	/**
	 * @brief StartからGoalまでの経路を探索する
	 *
	 * @param gridMap 対象GridMap
	 * @param start 開始Grid座標
	 * @param goal 目的Grid座標
	 *
	 * @return
	 * 経路が存在する場合、
	 *
	 * start → ... → goal
	 *
	 * の順にGridPositionを格納したvectorを返す。
	 *
	 * 経路が存在しない場合は空vectorを返す。
	 */
	static std::vector<GridPosition> FindPath(
		GridMap& gridMap,
		const GridPosition& start,
		const GridPosition& goal);

private:

	/**
	 * @brief A*で使用するヒューリスティック距離
	 *
	 * @details
	 * 4方向GridなのでManhattan Distanceを使用する。
	 */
	static int CalculateHeuristic(
		const GridPosition& a,
		const GridPosition& b);
};