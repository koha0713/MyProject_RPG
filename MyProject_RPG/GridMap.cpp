#include "GridMap.h"

#include <cmath>

bool GridMap::Initialize(
	int width,
	int height,
	float cellSize)
{
	if (width <= 0 ||
		height <= 0 ||
		cellSize <= 0.0f)
	{
		return false;
	}

	m_Width =
		width;

	m_Height =
		height;

	m_CellSize =
		cellSize;

	m_Cells.clear();

	m_Cells.resize(
		static_cast<size_t>(
			m_Width * m_Height));

	return true;
}

void GridMap::Finalize()
{
	m_Cells.clear();

	m_Width =
		0;

	m_Height =
		0;

	m_CellSize =
		1.0f;
}

//=====================================================
// Coordinate
//=====================================================

Vector3 GridMap::GridToWorld(
	const GridPosition& position) const
{
	// Gridの中心へGameObjectを配置する。
	//
	// 今回は
	//
	// Grid X -> World X
	// Grid Y -> World Z
	//
	// とする。

	return Vector3(
		static_cast<float>(
			position.X) *
		m_CellSize,

		0.0f,

		static_cast<float>(
			position.Y) *
		m_CellSize);
}

GridPosition GridMap::WorldToGrid(
	const Vector3& position) const
{
	GridPosition result;

	// 最も近いGrid座標へ変換する。
	result.X =
		static_cast<int>(
			std::round(
				position.x /
				m_CellSize));

	result.Y =
		static_cast<int>(
			std::round(
				position.z /
				m_CellSize));

	return result;
}

bool GridMap::IsInside(
	const GridPosition& position) const
{
	return
		position.X >= 0 &&
		position.Y >= 0 &&
		position.X < m_Width &&
		position.Y < m_Height;
}

//=====================================================
// Cell
//=====================================================

GridCell* GridMap::GetCell(
	const GridPosition& position)
{
	if (!IsInside(
		position))
	{
		return nullptr;
	}

	const size_t index =
		ToIndex(
			position);

	// 念のためvectorの範囲も確認する
	if (index >=
		m_Cells.size())
	{
		return nullptr;
	}

	return
		&m_Cells[index];
}

const GridCell* GridMap::GetCell(
	const GridPosition& position) const
{
	if (!IsInside(
		position))
	{
		return nullptr;
	}

	const size_t index =
		ToIndex(
			position);

	if (index >=
		m_Cells.size())
	{
		return nullptr;
	}

	return
		&m_Cells[index];
}

bool GridMap::CanMoveTo(
	const GridPosition& position) const
{
	//=================================================
	// Cell取得
	//=================================================

	const GridCell* cell =
		GetCell(
			position);

	if (!cell)
	{
		OutputDebugStringA(
			"[GridMap] CanMoveTo FAILED: Cell is null\n");

		return false;
	}

	//=================================================
	// Walkable確認
	//=================================================

	if (!cell->IsWalkable())
	{
		OutputDebugStringA(
			"[GridMap] CanMoveTo FAILED: Cell is Blocked\n");

		return false;
	}

	//=================================================
	// Occupant確認
	//=================================================

	if (cell->IsOccupied())
	{
		OutputDebugStringA(
			"[GridMap] CanMoveTo FAILED: Cell is Occupied\n");

		return false;
	}

	OutputDebugStringA(
		"[GridMap] CanMoveTo SUCCESS\n");

	return true;
}

void GridMap::SetCellType(
	const GridPosition& position,
	GridCellType type)
{
	GridCell* cell =
		GetCell(
			position);

	if (!cell)
	{
		return;
	}

	cell->Type =
		type;
}

//=====================================================
// Occupant
//=====================================================

bool GridMap::SetOccupant(
	const GridPosition& position,
	GameObject* gameObject)
{
	if (!gameObject)
	{
		return false;
	}

	GridCell* cell =
		GetCell(
			position);

	if (!cell)
	{
		return false;
	}

	// すでに別GameObjectが存在している場合は配置不可
	if (cell->Occupant &&
		cell->Occupant !=
		gameObject)
	{
		return false;
	}

	cell->Occupant =
		gameObject;

	return true;
}

void GridMap::ClearOccupant(
	const GridPosition& position,
	GameObject* gameObject)
{
	GridCell* cell =
		GetCell(
			position);

	if (!cell)
	{
		return;
	}

	// gameObject指定なしなら無条件Clear
	if (!gameObject)
	{
		cell->Occupant =
			nullptr;

		return;
	}

	// 指定GameObject自身の場合だけClear
	if (cell->Occupant ==
		gameObject)
	{
		cell->Occupant =
			nullptr;
	}
}

GameObject* GridMap::GetOccupant(
	const GridPosition& position)
{
	GridCell* cell =
		GetCell(
			position);

	if (!cell)
	{
		return nullptr;
	}

	return cell->Occupant;
}

const GameObject* GridMap::GetOccupant(
	const GridPosition& position) const
{
	const GridCell* cell =
		GetCell(
			position);

	if (!cell)
	{
		return nullptr;
	}

	return cell->Occupant;
}

//=====================================================
// Internal
//=====================================================

size_t GridMap::ToIndex(
	const GridPosition& position) const
{
	return
		static_cast<size_t>(
			position.Y *
			m_Width +
			position.X);
}