#pragma once

/**
 * @file TargetingSystem.h
 * @brief Actionの対象指定可能Gridを計算するSystem
 */

#include <vector>

#include "GridPosition.h"
#include "GridPattern.h"
#include "TargetingRules.h"

class GameObject;
class GridMap;

/**
 * @brief Targeting System
 *
 * @details
 * GridPatternとTargetingRulesから、
 * 実際にTargetとして指定可能なGridを計算する。
 *
 * DamageやEffect適用は担当しない。
 */
class TargetingSystem
{
public:

	/**
	 * @brief Target可能Grid一覧を取得
	 */
	static std::vector<GridPosition>
		GetTargetableCells(
			const GridMap& map,
			const GameObject* source,
			const GridPosition& origin,
			const GridPattern& pattern,
			const TargetingRules& rules);

	/**
	 * @brief 指定GridがTarget可能か
	 */
	static bool IsTargetableCell(
		const GridMap& map,
		const GameObject* source,
		const GridPosition& origin,
		const GridPosition& target,
		const GridPattern& pattern,
		const TargetingRules& rules);

private:

	/**
	 * @brief Target種別がAllowedTargetsに一致するか
	 */
	static bool IsValidTargetType(
		const GridMap& map,
		const GameObject* source,
		const GridPosition& target,
		const TargetingRules& rules);

	/**
	 * @brief 射線が通っているか
	 */
	static bool HasLineOfSight(
		const GridMap& map,
		const GridPosition& start,
		const GridPosition& end,
		const TargetingRules& rules);

	/**
	 * @brief 指定Gridが壁・通行不可Cellか
	 */
	static bool IsWallCell(
		const GridMap& map,
		const GridPosition& position);

	/**
	 * @brief GridPattern内にTargetが含まれるか
	 */
	static bool IsInsidePattern(
		const GridPosition& origin,
		const GridPosition& target,
		const GridPattern& pattern);
};