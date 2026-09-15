#pragma once

/**
 * @file GridCell.h
 * @brief Gridの1マス分の情報
 */

class GameObject;

/**
 * @brief Grid Cell Type
 */
enum class GridCellType
{
	/**
	 * @brief 通行可能
	 */
	Walkable,

	/**
	 * @brief 通行不可
	 */
	Blocked
};


/**
 * @brief Gridの1Cell
 *
 * @details
 * β版では、
 *
 * ・通行可能か
 * ・GameObjectに占有されているか
 *
 * のみ管理する。
 *
 * 地形コスト等は後から追加する。
 */
struct GridCell
{
	GridCellType Type =
		GridCellType::Walkable;

	/**
	 * @brief 現在このCellを占有しているGameObject
	 *
	 * @note
	 * GameObjectの所有権は持たない。
	 */
	GameObject* Occupant =
		nullptr;

	bool IsWalkable() const
	{
		return
			Type ==
			GridCellType::Walkable;
	}

	bool IsOccupied() const
	{
		return
			Occupant != nullptr;
	}
};