#include "GridPattern.h"

#include <algorithm>
#include <cstdlib>

//=====================================================
// From Mask
//=====================================================

GridPattern GridPattern::FromMask(
	int width,
	int height,
	const std::vector<int>& cells,
	bool rotateWithFacing)
{
	GridPattern pattern;

	//=================================================
	// Validation
	//=================================================

	if (width <= 0 ||
		height <= 0)
	{
		return pattern;
	}

	const size_t expectedSize =
		static_cast<size_t>(
			width) *
		static_cast<size_t>(
			height);

	if (cells.size() !=
		expectedSize)
	{
		return pattern;
	}

	//=================================================
	// Origin検索
	//=================================================

	int originX =
		0;

	int originY =
		0;

	int originCount =
		0;

	for (int y = 0;
		y < height;
		++y)
	{
		for (int x = 0;
			x < width;
			++x)
		{
			const size_t index =
				static_cast<size_t>(
					y * width + x);

			if (cells[index] ==
				static_cast<int>(
					PatternCell::Origin))
			{
				originX =
					x;

				originY =
					y;

				++originCount;
			}
		}
	}

	// Originは必ず1個。
	if (originCount != 1)
	{
		return pattern;
	}

	//=================================================
	// Effect Cell -> Relative Offset
	//=================================================

	for (int y = 0;
		y < height;
		++y)
	{
		for (int x = 0;
			x < width;
			++x)
		{
			const size_t index =
				static_cast<size_t>(
					y * width + x);

			const int value =
				cells[index];

			//=========================================
			// 未知の値は禁止
			//=========================================

			if (value !=
				static_cast<int>(
					PatternCell::None) &&
				value !=
				static_cast<int>(
					PatternCell::Effect) &&
				value !=
				static_cast<int>(
					PatternCell::Origin))
			{
				return GridPattern();
			}

			if (value !=
				static_cast<int>(
					PatternCell::Effect))
			{
				continue;
			}

			GridOffset offset;

			offset.X =
				x -
				originX;

			offset.Y =
				y -
				originY;

			pattern.m_Offsets.
				push_back(
					offset);
		}
	}

	pattern.m_RotateWithFacing =
		rotateWithFacing;

	pattern.m_Valid =
		true;

	return pattern;
}

//=====================================================
// Diamond
//=====================================================

GridPattern GridPattern::CreateDiamond(
	int range,
	bool includeOrigin)
{
	GridPattern pattern;

	if (range < 0)
	{
		return pattern;
	}

	for (int y = -range;
		y <= range;
		++y)
	{
		for (int x = -range;
			x <= range;
			++x)
		{
			const int distance =
				std::abs(x) +
				std::abs(y);

			if (distance >
				range)
			{
				continue;
			}

			if (!includeOrigin &&
				x == 0 &&
				y == 0)
			{
				continue;
			}

			pattern.m_Offsets.
				push_back(
					GridOffset{
						x,
						y
					});
		}
	}

	pattern.m_RotateWithFacing =
		false;

	pattern.m_Valid =
		true;

	return pattern;
}

//=====================================================
// Cross
//=====================================================

GridPattern GridPattern::CreateCross(
	int range,
	bool includeOrigin)
{
	GridPattern pattern;

	if (range < 0)
	{
		return pattern;
	}

	if (includeOrigin)
	{
		pattern.m_Offsets.
			push_back(
				GridOffset{
					0,
					0
				});
	}

	for (int distance = 1;
		distance <= range;
		++distance)
	{
		// North
		pattern.m_Offsets.
			push_back(
				GridOffset{
					0,
					-distance
				});

		// East
		pattern.m_Offsets.
			push_back(
				GridOffset{
					distance,
					0
				});

		// South
		pattern.m_Offsets.
			push_back(
				GridOffset{
					0,
					distance
				});

		// West
		pattern.m_Offsets.
			push_back(
				GridOffset{
					-distance,
					0
				});
	}

	pattern.m_RotateWithFacing =
		false;

	pattern.m_Valid =
		true;

	return pattern;
}

//=====================================================
// Square
//=====================================================

GridPattern GridPattern::CreateSquare(
	int radius,
	bool includeOrigin)
{
	GridPattern pattern;

	if (radius < 0)
	{
		return pattern;
	}

	for (int y = -radius;
		y <= radius;
		++y)
	{
		for (int x = -radius;
			x <= radius;
			++x)
		{
			if (!includeOrigin &&
				x == 0 &&
				y == 0)
			{
				continue;
			}

			pattern.m_Offsets.
				push_back(
					GridOffset{
						x,
						y
					});
		}
	}

	pattern.m_RotateWithFacing =
		false;

	pattern.m_Valid =
		true;

	return pattern;
}

//=====================================================
// Line
//=====================================================

GridPattern GridPattern::CreateLine(
	int length,
	bool includeOrigin)
{
	GridPattern pattern;

	if (length < 0)
	{
		return pattern;
	}

	//=================================================
	// Origin
	//=================================================

	if (includeOrigin)
	{
		pattern.m_Offsets.
			push_back(
				GridOffset{
					0,
					0
				});
	}

	//=================================================
	// North方向を基準Patternとして作成
	//=================================================

	for (int distance = 1;
		distance <= length;
		++distance)
	{
		pattern.m_Offsets.
			push_back(
				GridOffset{
					0,
					-distance
				});
	}

	// Characterの向きに合わせて回転する。
	pattern.m_RotateWithFacing =
		true;

	pattern.m_Valid =
		true;

	return pattern;
}

//=====================================================
// Get Rotated Offsets
//=====================================================

std::vector<GridOffset>
GridPattern::GetOffsets(
	GridDirection direction) const
{
	if (!m_Valid)
	{
		return {};
	}

	// 回転不要Patternならそのまま返す。
	if (!m_RotateWithFacing ||
		direction ==
		GridDirection::North)
	{
		return m_Offsets;
	}

	std::vector<GridOffset>
		result;

	result.reserve(
		m_Offsets.size());

	for (const GridOffset& offset :
		m_Offsets)
	{
		result.push_back(
			RotateOffset(
				offset,
				direction));
	}

	return result;
}

//=====================================================
// Rotate Offset
//=====================================================

GridOffset GridPattern::RotateOffset(
	const GridOffset& offset,
	GridDirection direction)
{
	switch (direction)
	{
	case GridDirection::North:

		return offset;

	case GridDirection::East:

		// 時計回り90度
		//
		// (0, -1)
		//     ↓
		// (1, 0)
		return GridOffset{
			-offset.Y,
			offset.X
		};

	case GridDirection::South:

		// 180度
		return GridOffset{
			-offset.X,
			-offset.Y
		};

	case GridDirection::West:

		// 反時計回り90度
		return GridOffset{
			offset.Y,
			-offset.X
		};

	default:

		return offset;
	}
}