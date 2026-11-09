#pragma once

/**
 * @file TargetingRules.h
 * @brief Actionの対象指定・射線・貫通ルール定義
 */

#include <cstdint>

 /**
  * @brief Targetとして指定可能な対象種別
  *
  * @details
  * bit flagとして扱う。
  *
  * 例：
  *
  * Enemy | EmptyCell
  *
  * のように複数指定可能。
  */
enum class TargetType : uint8_t
{
	None =
	0,

	Self =
	1 << 0,

	Ally =
	1 << 1,

	Enemy =
	1 << 2,

	EmptyCell =
	1 << 3
};

/**
 * @brief TargetType OR演算
 */
inline TargetType operator|(
	TargetType lhs,
	TargetType rhs)
{
	return static_cast<TargetType>(
		static_cast<uint8_t>(lhs) |
		static_cast<uint8_t>(rhs));
}

/**
 * @brief TargetType AND演算
 */
inline TargetType operator&(
	TargetType lhs,
	TargetType rhs)
{
	return static_cast<TargetType>(
		static_cast<uint8_t>(lhs) &
		static_cast<uint8_t>(rhs));
}

/**
 * @brief Flagを持っているか
 */
inline bool HasTargetType(
	TargetType value,
	TargetType flag)
{
	return
		(static_cast<uint8_t>(
			value & flag) != 0);
}

/**
 * @brief 射線計算方式
 *
 * @details
 * 現段階ではGrid上の直線判定のみ想定。
 *
 * 将来的に、
 *
 * ・Arc
 * ・Projectile
 * ・特殊Trajectory
 *
 * などへ拡張可能。
 */
enum class LineOfSightType : uint8_t
{
	/**
	 * @brief 射線判定なし
	 *
	 * 壁越しでもTarget指定可能。
	 */
	None,

	/**
	 * @brief 通常の直線射線
	 */
	Direct
};

/**
 * @brief Targeting Rule
 *
 * @details
 * GridPatternとは独立して、
 *
 * ・誰を狙えるか
 * ・射線が必要か
 * ・何に遮られるか
 * ・何を貫通できるか
 *
 * を定義する。
 */
struct TargetingRules
{
	//=================================================
	// Target Type
	//=================================================

	/**
	 * @brief 指定可能Target
	 *
	 * 例：
	 *
	 * Enemy
	 *
	 * Ally | Self
	 *
	 * Enemy | EmptyCell
	 */
	TargetType AllowedTargets =
		TargetType::Enemy;

	//=================================================
	// Line Of Sight
	//=================================================

	/**
	 * @brief 射線判定方式
	 */
	LineOfSightType LineOfSight =
		LineOfSightType::None;

	//=================================================
	// Block
	//=================================================

	/**
	 * @brief 壁が射線を遮るか
	 */
	bool WallsBlock =
		true;

	/**
	 * @brief Characterが射線を遮るか
	 *
	 * @details
	 * Ranger通常攻撃などで使用。
	 */
	bool UnitsBlock =
		false;

	//=================================================
	// Pierce
	//=================================================

	/**
	 * @brief Characterを貫通できるか
	 *
	 * @details
	 * 貫通射撃などで使用。
	 */
	bool PierceUnits =
		false;

	/**
	 * @brief 壁を貫通できるか
	 *
	 * @details
	 * 特殊魔法などで使用。
	 */
	bool PierceWalls =
		false;

	//=================================================
	// Path Behavior
	//=================================================

	/**
	 * @brief 最初に衝突したUnitでEffectを止めるか
	 *
	 * @details
	 * 通常の矢などはtrue。
	 *
	 * 貫通射撃ではfalse。
	 */
	bool StopOnFirstUnit =
		true;

	/**
	 * @brief 最初に衝突した壁でEffectを止めるか
	 *
	 * @details
	 * 通常Projectileではtrue。
	 */
	bool StopOnFirstWall =
		true;
};