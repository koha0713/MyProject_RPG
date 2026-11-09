#pragma once

/**
 * @file EffectSystem.h
 * @brief Action Effectを適用するSystem
 */

#include "GridPosition.h"
#include "EffectDefinition.h"

class GameObject;
class GridMap;

/**
 * @brief Effect System
 *
 * @details
 * TargetingSystemで決定された対象に対して、
 *
 * ・Damage
 * ・Heal
 * ・Knockback
 *
 * などのEffectを適用する。
 */
class EffectSystem
{
public:

	/**
	 * @brief 単一TargetへEffectを適用
	 */
	static bool ApplyEffect(
		GridMap& map,
		GameObject* source,
		GameObject* target,
		const EffectDefinition& effect);

	/**
	 * @brief Grid上のTargetへEffectを適用
	 *
	 * @details
	 * Occupantが存在する場合のみ適用。
	 */
	static bool ApplyEffectToCell(
		GridMap& map,
		GameObject* source,
		const GridPosition& targetPosition,
		const EffectDefinition& effect);

private:

	/**
	 * @brief Effect対象として有効か
	 */
	static bool CanAffectTarget(
		GameObject* source,
		GameObject* target,
		const EffectDefinition& effect);

	/**
	 * @brief Damage
	 */
	static bool ApplyDamage(
		GameObject* source,
		GameObject* target,
		const EffectDefinition& effect);

	/**
	 * @brief Heal
	 */
	static bool ApplyHeal(
		GameObject* source,
		GameObject* target,
		const EffectDefinition& effect);

	/**
	 * @brief Knockback
	 */
	static bool ApplyKnockback(
		GridMap& map,
		GameObject* source,
		GameObject* target,
		const EffectDefinition& effect);
};