#pragma once

/**
 * @file EffectType.h
 * @brief Action Effectの種類
 */

#include <cstdint>

 /**
  * @brief Effectの種類
  */
enum class EffectType : uint8_t
{
	None,

	// ダメージ
	Damage,

	// 回復
	Heal,

	// ノックバック
	Knockback,

	// 移動 / Teleport
	Move,

	// Buff
	Buff,

	// Debuff
	Debuff
};