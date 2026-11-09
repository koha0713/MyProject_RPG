#pragma once

/**
 * @file PlayerActionDefinition.h
 * @brief Player Actionの定義
 */
#include <vector>
#include "PlayerActionType.h"
#include "GridPattern.h"
#include "TargetingRules.h"
#include "EffectDefinition.h"
#include "AnimationData.h"

 /**
  * @brief Action選択後のTarget指定方式
  */
enum class TargetingMode : uint8_t
{
	/**
	 * @brief Target指定不要
	 */
	None,

	/**
	 * @brief Gridを指定
	 */
	Grid,

	/**
	 * @brief Characterを指定
	 */
	Unit,

	/**
	 * @brief 自分自身を対象
	 */
	Self,

	/**
	 * @brief 方向を指定
	 */
	Direction
};

 /**
  * @brief Action Cost
  */
struct PlayerActionCost
{
	int AP =
		0;

	int MP =
		0;
};

/**
 * @brief Player Action Definition
 *
 * @details
 * Actionごとの、
 *
 * ・AP / MP Cost
 * ・Target選択範囲
 * ・Targeting Rule
 * ・Effect範囲
 *
 * をまとめて保持する。
 */
struct PlayerActionDefinition
{
	//=================================================
	// Basic
	//=================================================

	PlayerActionType Type =
		PlayerActionType::Move;

	/**
	 * @brief 現在使用可能か
	 */
	bool Enabled =
		true;

	//=================================================
	// Cost
	//=================================================

	PlayerActionCost Cost;

	//=================================================
	// Target Selection
	//=================================================

	/**
	 * @brief Target選択可能範囲
	 *
	 * @details
	 * Player位置をOriginとするPattern。
	 *
	 * 例：
	 *
	 * Warrior通常攻撃
	 * → Cross(1)
	 *
	 * Ranger通常攻撃
	 * → Diamond(5)
	 *
	 * Fireball
	 * → Diamond(5)
	 */
	GridPattern TargetPattern;

	/**
	 * @brief Target指定ルール
	 *
	 * ・敵
	 * ・味方
	 * ・空きマス
	 * ・射線
	 * ・貫通
	 *
	 * などを管理する。
	 */
	TargetingRules Targeting;

	TargetingMode TargetMode =
		TargetingMode::None;

	//=================================================
	// Animation
	//=================================================

	/**
	 * @brief Action実行時に使用するAnimation
	 *
	 * NoneならAnimationを再生しない。
	 */
	AnimationID Animation =
		AnimationID::None;

	/**
	 * @brief Animation Loop
	 */
	bool AnimationLoop =
		false;

	/**
	 * @brief Blend時間
	 */
	float AnimationBlendTime =
		0.10f;

	//=================================================
	// Effect
	//=================================================

	/**
	 * @brief EffectPatternを使用するか
	 *
	 * false:
	 * 選択Targetそのものへ効果を与える。
	 *
	 * true:
	 * 選択地点をOriginとしてEffectPatternを展開する。
	 */
	bool HasEffectPattern =
		false;

	/**
	 * @brief 効果範囲
	 *
	 * @details
	 * Fireballや旋風斬りなど、
	 * 複数Gridへ効果を適用するActionで使用する。
	 */
	GridPattern EffectPattern;

	/**
	 * @brief Actionが持つEffect一覧
	 *
	 * 例：
	 *
	 * Shield Bash
	 * → Damage
	 * → Knockback
	 *
	 * のように複数Effectを保持できる。
	 */
	std::vector<EffectDefinition>
		Effects;
};

