#pragma once

/**
 * @file GridPattern.h
 * @brief Grid上のTarget / Effect形状を定義する
 */

#include <cstdint>
#include <vector>

 /**
  * @brief 数字マスク上のCell種別
  *
  * @details
  * Patternの「形」だけを表す。
  *
  * 壁貫通・Unit貫通・射線などは
  * TargetingRules側で管理する。
  */
enum class PatternCell : uint8_t
{
	None = 0,

	Effect = 1,

	Origin = 9
};

/**
 * @brief Pattern原点からの相対Grid座標
 */
struct GridOffset
{
	int X = 0;

	int Y = 0;

	bool operator==(
		const GridOffset& other) const
	{
		return
			X == other.X &&
			Y == other.Y;
	}
};

/**
 * @brief Patternの向き
 *
 * @details
 * 基本PatternはNorth向きとして作成する。
 *
 * North:
 *   Y-
 *
 * East:
 *   X+
 *
 * South:
 *   Y+
 *
 * West:
 *   X-
 */
enum class GridDirection : uint8_t
{
	North,

	East,

	South,

	West
};

/**
 * @brief Grid Pattern
 *
 * @details
 * 編集時には数字マスクを使用できる。
 *
 * 例：
 *
 * 0 0 1 0 0
 * 0 0 1 0 0
 * 1 1 9 1 1
 * 0 0 1 0 0
 * 0 0 1 0 0
 *
 * 内部ではOriginからの相対座標へ変換する。
 *
 * そのため実行時に毎回Mask全体を走査する必要がない。
 */
class GridPattern
{
public:

	GridPattern() = default;

	//=================================================
	// Custom Mask
	//=================================================

	/**
	 * @brief 数字MaskからPatternを生成
	 *
	 * @param width Mask横幅
	 * @param height Mask縦幅
	 * @param cells
	 * 0 = None
	 * 1 = Effect
	 * 9 = Origin
	 *
	 * @param rotateWithFacing
	 * Character方向に応じて回転するか
	 */
	static GridPattern FromMask(
		int width,
		int height,
		const std::vector<int>& cells,
		bool rotateWithFacing = false);

	//=================================================
	// Common Pattern
	//=================================================

	/**
	 * @brief Manhattan距離による菱形
	 */
	static GridPattern CreateDiamond(
		int range,
		bool includeOrigin = false);

	/**
	 * @brief 十字型
	 */
	static GridPattern CreateCross(
		int range,
		bool includeOrigin = false);

	/**
	 * @brief 正方形
	 */
	static GridPattern CreateSquare(
		int radius,
		bool includeOrigin = false);

	/**
	 * @brief 前方直線
	 *
	 * @details
	 * 基準はNorth方向。
	 * 通常はFacingに合わせて回転させる。
	 */
	static GridPattern CreateLine(
		int length,
		bool includeOrigin = false);

	//=================================================
	// Data
	//=================================================

	/**
	 * @brief 基準方向のOffset取得
	 */
	const std::vector<GridOffset>&
		GetOffsets() const
	{
		return m_Offsets;
	}

	/**
	 * @brief 指定方向へ回転したOffsetを取得
	 *
	 * @note
	 * rotateWithFacing=falseの場合は
	 * 元のPatternをそのまま返す。
	 */
	std::vector<GridOffset>
		GetOffsets(
			GridDirection direction) const;

	bool IsRotateWithFacing() const
	{
		return m_RotateWithFacing;
	}

	bool IsValid() const
	{
		return m_Valid;
	}

	bool Empty() const
	{
		return m_Offsets.empty();
	}

private:

	/**
	 * @brief Offsetを90度単位で回転
	 */
	static GridOffset RotateOffset(
		const GridOffset& offset,
		GridDirection direction);

private:

	std::vector<GridOffset>
		m_Offsets;

	bool
		m_RotateWithFacing =
		false;

	bool
		m_Valid =
		false;
};