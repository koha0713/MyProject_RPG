#pragma once

/**
 * @file GridPosition.h
 * @brief Grid上の整数座標
 */

 /**
  * @brief Grid Position
  *
  * @details
  * World座標とは分離して保持する。
  *
  * X : Gridの横方向
  * Y : Gridの縦方向
  *
  * World上では
  *
  * Grid X -> World X
  * Grid Y -> World Z
  *
  * として使用する。
  */
struct GridPosition
{
	int X = 0;
	int Y = 0;

	bool operator==(
		const GridPosition& other) const
	{
		return
			X == other.X &&
			Y == other.Y;
	}

	bool operator!=(
		const GridPosition& other) const
	{
		return !(*this == other);
	}

	GridPosition operator+(
		const GridPosition& other) const
	{
		return GridPosition
		{
			X + other.X,
			Y + other.Y
		};
	}
};