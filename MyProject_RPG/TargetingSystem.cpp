#include "TargetingSystem.h"

#include <cstdlib>

#include "GameObject.h"
#include "GridMap.h"

//=====================================================
// Targetable Cells
//=====================================================
std::vector<GridPosition>
TargetingSystem::GetTargetableCells(
	const GridMap& map,
	const GameObject* source,
	const GridPosition& origin,
	const GridPattern& pattern,
	const TargetingRules& rules)
{
	std::vector<GridPosition> result;

	if (!pattern.IsValid())
	{
		return result;
	}

	const auto& offsets =
		pattern.GetOffsets();

	result.reserve(
		offsets.size());

	for (const GridOffset& offset :
		offsets)
	{
		const GridPosition target
		{
			origin.X + offset.X,
			origin.Y + offset.Y
		};

		// Map外の座標はTarget候補にしない。
		if (!map.IsInside(
			target))
		{
			continue;
		}

		if (!IsTargetableCell(
			map,
			source,
			origin,
			target,
			pattern,
			rules))
		{
			continue;
		}

		result.push_back(
			target);
	}

	return result;
}

//=====================================================
// Targetable Cell
//=====================================================
bool TargetingSystem::IsTargetableCell(
	const GridMap& map,
	const GameObject* source,
	const GridPosition& origin,
	const GridPosition& target,
	const GridPattern& pattern,
	const TargetingRules& rules)
{
	//=================================================
	// Map範囲
	//=================================================

	if (!map.IsInside(
		target))
	{
		return false;
	}

	//=================================================
	// Pattern
	//=================================================

	if (!IsInsidePattern(
		origin,
		target,
		pattern))
	{
		return false;
	}

	//=================================================
	// Target Type
	//=================================================

	if (!IsValidTargetType(
		map,
		source,
		target,
		rules))
	{
		return false;
	}

	//=================================================
	// Line Of Sight
	//=================================================

	if (rules.LineOfSight ==
		LineOfSightType::Direct)
	{
		if (!HasLineOfSight(
			map,
			origin,
			target,
			rules))
		{
			return false;
		}
	}

	return true;
}

//=====================================================
// Target Type
//=====================================================
bool TargetingSystem::IsValidTargetType(
	const GridMap& map,
	const GameObject* source,
	const GridPosition& target,
	const TargetingRules& rules)
{
	// const GridMapから取得するため、
	// Occupantもconst GameObject*として扱う。
	const GameObject* occupant =
		map.GetOccupant(
			target);

	//=================================================
	// Empty Cell
	//=================================================

	if (!occupant)
	{
		return HasTargetType(
			rules.AllowedTargets,
			TargetType::EmptyCell);
	}

	//=================================================
	// Self
	//=================================================

	if (occupant ==
		source)
	{
		return HasTargetType(
			rules.AllowedTargets,
			TargetType::Self);
	}

	//=================================================
	// Ally / Enemy
	//=================================================

	if (!source)
	{
		return false;
	}

	const Tag sourceTag =
		source->GetTag();

	const Tag targetTag =
		occupant->GetTag();

	// 同じTagなら味方として扱う。
	if (sourceTag ==
		targetTag)
	{
		return HasTargetType(
			rules.AllowedTargets,
			TargetType::Ally);
	}

	// Player <-> Enemy の組み合わせを敵として扱う。
	if ((sourceTag == Tag::Player &&
		targetTag == Tag::Enemy) ||
		(sourceTag == Tag::Enemy &&
			targetTag == Tag::Player))
	{
		return HasTargetType(
			rules.AllowedTargets,
			TargetType::Enemy);
	}

	return false;
}

//=====================================================
// Pattern
//=====================================================

bool TargetingSystem::IsInsidePattern(
	const GridPosition& origin,
	const GridPosition& target,
	const GridPattern& pattern)
{
	if (!pattern.IsValid())
	{
		return false;
	}

	const int relativeX =
		target.X -
		origin.X;

	const int relativeY =
		target.Y -
		origin.Y;

	const GridOffset targetOffset
	{
		relativeX,
		relativeY
	};

	const auto& offsets =
		pattern.GetOffsets();

	for (const GridOffset& offset :
		offsets)
	{
		if (offset ==
			targetOffset)
		{
			return true;
		}
	}

	return false;
}

//=====================================================
// Line Of Sight
//=====================================================
bool TargetingSystem::HasLineOfSight(
	const GridMap& map,
	const GridPosition& start,
	const GridPosition& end,
	const TargetingRules& rules)
{
	int x0 =
		start.X;

	int y0 =
		start.Y;

	const int x1 =
		end.X;

	const int y1 =
		end.Y;

	const int dx =
		std::abs(
			x1 - x0);

	const int sx =
		x0 < x1
		? 1
		: -1;

	const int dy =
		-std::abs(
			y1 - y0);

	const int sy =
		y0 < y1
		? 1
		: -1;

	int error =
		dx +
		dy;

	while (true)
	{
		//=============================================
		// 次のGridへ進む
		//=============================================

		const int error2 =
			2 * error;

		if (error2 >=
			dy)
		{
			error +=
				dy;

			x0 +=
				sx;
		}

		if (error2 <=
			dx)
		{
			error +=
				dx;

			y0 +=
				sy;
		}

		//=============================================
		// Target到達
		//=============================================
		//
		// Target自身は遮蔽判定しない。
		//
		// EnemyがTarget地点に存在するのは正常なため。
		//=============================================

		if (x0 == x1 &&
			y0 == y1)
		{
			break;
		}

		const GridPosition current
		{
			x0,
			y0
		};

		//=============================================
		// Map外
		//=============================================

		if (!map.IsInside(
			current))
		{
			return false;
		}

		//=============================================
		// Wall
		//=============================================

		if (rules.WallsBlock &&
			!rules.PierceWalls &&
			IsWallCell(
				map,
				current))
		{
			return false;
		}

		//=============================================
		// Unit
		//=============================================

		const GameObject* occupant =
			map.GetOccupant(
				current);

		if (occupant &&
			rules.UnitsBlock &&
			!rules.PierceUnits)
		{
			return false;
		}
	}

	return true;
}

//=====================================================
// Wall Cell
//=====================================================
bool TargetingSystem::IsWallCell(
	const GridMap& map,
	const GridPosition& position)
{
	//=================================================
	// Map範囲
	//=================================================

	if (!map.IsInside(
		position))
	{
		// Map外は壁として扱う。
		return true;
	}

	//=================================================
	// Cell取得
	//=================================================

	const GridCell* cell =
		map.GetCell(
			position);

	if (!cell)
	{
		// Cell取得失敗も安全側で壁扱い。
		return true;
	}

	//=================================================
	// Walkable判定
	//=================================================
	//
	// Occupantはここでは判定しない。
	//
	// Unit遮蔽はHasLineOfSight()側の
	// UnitsBlock / PierceUnitsで別に判定する。
	//=================================================

	return !cell->IsWalkable();
}