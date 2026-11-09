#pragma once

/**
 * @file EffectDefinition.h
 * @brief Action Effectの定義
 */

#include "EffectType.h"

 /**
  * @brief Effect Definition
  *
  * @details
  * ActionがTargetへ与える効果を定義する。
  *
  * 現段階では、
  *
  * ・Damage
  * ・Heal
  * ・Knockback
  *
  * を中心に扱う。
  *
  * 将来的にBuff / Debuff / StatusEffectへ拡張可能。
  */
struct EffectDefinition
{
	/**
	 * @brief Effect種別
	 */
	EffectType Type =
		EffectType::None;

	/**
	 * @brief 基本値
	 *
	 * DamageならDamage値、
	 * Healなら回復量として使用する。
	 */
	int Power =
		0;

	/**
	 * @brief Knockback距離
	 */
	int KnockbackDistance =
		0;

	/**
	 * @brief Source自身にも適用するか
	 */
	bool AffectSelf =
		false;

	/**
	 * @brief Allyへ適用可能か
	 */
	bool AffectAllies =
		false;

	/**
	 * @brief Enemyへ適用可能か
	 */
	bool AffectEnemies =
		true;
};