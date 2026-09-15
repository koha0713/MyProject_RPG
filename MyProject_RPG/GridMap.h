#pragma once

/**
 * @file GridMap.h
 * @brief Field全体のGridを管理する
 */

#include <vector>

#include "CommonType.h"
#include "GridCell.h"
#include "GridPosition.h"

class GameObject;

/**
 * @brief Grid Map
 * @details
 * FieldScene全体で共有されるGrid情報。
 * Grid座標とWorld座標の変換、
 * 通行判定、GameObjectの占有状態を管理する。
 */
class GridMap
{
public:

	GridMap() = default;
	~GridMap() = default;

	/**
	 * @brief Grid生成
	 *
	 * @param width Grid横幅
	 * @param height Grid縦幅
	 * @param cellSize 1マスのWorldサイズ
	 */
	bool Initialize(
		int width,
		int height,
		float cellSize);

	void Finalize();

	//====================
	// Coordinate
	//====================

	/**
	 * @brief Grid座標からWorld座標へ変換
	 */
	Vector3 GridToWorld(
		const GridPosition& position) const;

	/**
	 * @brief World座標からGrid座標へ変換
	 */
	GridPosition WorldToGrid(
		const Vector3& position) const;

	/**
	 * @brief Grid範囲内か
	 */
	bool IsInside(
		const GridPosition& position) const;

	//====================
	// Cell
	//====================

	GridCell* GetCell(
		const GridPosition& position);

	const GridCell* GetCell(
		const GridPosition& position) const;

	/**
	 * @brief 移動可能なCellか
	 * @details
	 * 範囲内
	 * + Walkable
	 * + Occupantなし
	 *
	 * の場合のみtrue。
	 */
	bool CanMoveTo(
		const GridPosition& position) const;

	void SetCellType(
		const GridPosition& position,
		GridCellType type);

	//====================
	// Occupant
	//====================

	bool SetOccupant(
		const GridPosition& position,
		GameObject* gameObject);

	void ClearOccupant(
		const GridPosition& position,
		GameObject* gameObject = nullptr);

	GameObject* GetOccupant(
		const GridPosition& position);

	const GameObject* GetOccupant(
		const GridPosition& position) const;

	//====================
	// Getter
	//====================

	int GetWidth() const
	{
		return m_Width;
	}

	int GetHeight() const
	{
		return m_Height;
	}

	float GetCellSize() const
	{
		return m_CellSize;
	}

private:

	/**
	 * @brief 2次元Grid座標をvector indexへ変換
	 */
	size_t ToIndex(
		const GridPosition& position) const;

private:

	int m_Width =
		0;

	int m_Height =
		0;

	float m_CellSize =
		1.0f;

	std::vector<GridCell>
		m_Cells;
};