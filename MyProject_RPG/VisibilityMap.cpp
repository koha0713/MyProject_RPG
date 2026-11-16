#include "VisibilityMap.h"

#include <algorithm>
#include <limits>

//=====================================================
// Initialize
//=====================================================

bool VisibilityMap::Initialize(
	const TerrainMapData& terrainData)
{
	Clear();

	if (terrainData.Cells.empty())
	{
		return false;
	}

	//=================================================
	// Terrain範囲取得
	//=================================================

	int maxX =
		-1;

	int maxY =
		-1;

	for (const TerrainCellData& cell :
		terrainData.Cells)
	{
		// 負のGrid座標は現在のField仕様では対象外
		if (cell.Position.X < 0 ||
			cell.Position.Y < 0)
		{
			continue;
		}

		maxX =
			(std::max)(
				maxX,
				cell.Position.X);

		maxY =
			(std::max)(
				maxY,
				cell.Position.Y);
	}

	if (maxX < 0 ||
		maxY < 0)
	{
		return false;
	}

	m_Width =
		maxX + 1;

	m_Height =
		maxY + 1;

	//=================================================
	// 全Cellを未探索状態で生成
	//=================================================

	const size_t cellCount =
		static_cast<size_t>(
			m_Width) *
		static_cast<size_t>(
			m_Height);

	m_States.assign(
		cellCount,
		VisibilityState::Unexplored);

	return true;
}

//=====================================================
// Clear
//=====================================================

void VisibilityMap::Clear()
{
	m_Width =
		0;

	m_Height =
		0;

	m_States.clear();
}

//=====================================================
// Visibility Update
//=====================================================

void VisibilityMap::
ClearCurrentVisibility()
{
	for (VisibilityState& state :
		m_States)
	{
		// 一度Visibleになった場所は
		// 未探索には戻さない。
		if (state ==
			VisibilityState::Visible)
		{
			state =
				VisibilityState::Explored;
		}
	}
}

void VisibilityMap::SetVisible(
	const GridPosition& position)
{
	if (!IsInside(position))
	{
		return;
	}

	m_States[
		ToIndex(position)] =
		VisibilityState::Visible;
}

void VisibilityMap::RevealCircle(
	const GridPosition& center,
	int radius)
{
	if (radius < 0)
	{
		return;
	}

	//=================================================
	// 距離比較ではsqrtを使用しない
	//=================================================
	//
	// distance <= radius
	//
	// は
	//
	// distanceSq <= radiusSq
	//
	// で判定できるため、
	// Gridごとのsqrtを回避する。
	//=================================================

	const int radiusSq =
		radius *
		radius;

	for (int offsetY = -radius;
		offsetY <= radius;
		++offsetY)
	{
		for (int offsetX = -radius;
			offsetX <= radius;
			++offsetX)
		{
			const int distanceSq =
				offsetX *
				offsetX +
				offsetY *
				offsetY;

			if (distanceSq >
				radiusSq)
			{
				continue;
			}

			const GridPosition position
			{
				center.X +
					offsetX,

				center.Y +
					offsetY
			};

			SetVisible(
				position);
		}
	}
}

//=====================================================
// Query
//=====================================================

VisibilityState
VisibilityMap::GetState(
	const GridPosition& position) const
{
	if (!IsInside(position))
	{
		// Map外は探索不能なので
		// 未探索として扱う。
		return
			VisibilityState::Unexplored;
	}

	return
		m_States[
			ToIndex(position)];
}

bool VisibilityMap::IsInside(
	const GridPosition& position) const
{
	if (position.X < 0 ||
		position.Y < 0)
	{
		return false;
	}

	if (position.X >=
		m_Width ||
		position.Y >=
		m_Height)
	{
		return false;
	}

	return true;
}

bool VisibilityMap::IsVisible(
	const GridPosition& position) const
{
	return
		GetState(position) ==
		VisibilityState::Visible;
}

bool VisibilityMap::IsExplored(
	const GridPosition& position) const
{
	const VisibilityState state =
		GetState(position);

	// 現在Visibleな場所も、
	// 「探索済み」という意味ではtrue。
	return
		state ==
		VisibilityState::Explored ||
		state ==
		VisibilityState::Visible;
}

bool VisibilityMap::
HasBeenExplored(
	const GridPosition& position) const
{
	if (!IsInside(position))
	{
		return false;
	}

	const VisibilityState state =
		GetState(position);

	return
		state !=
		VisibilityState::Unexplored;
}

//=====================================================
// Index
//=====================================================

size_t VisibilityMap::ToIndex(
	const GridPosition& position) const
{
	return
		static_cast<size_t>(
			position.Y) *
		static_cast<size_t>(
			m_Width) +
		static_cast<size_t>(
			position.X);
}