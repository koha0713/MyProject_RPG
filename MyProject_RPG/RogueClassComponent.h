#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Rogue
 */
class RogueClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Rogue;
	}

	const char* GetClassName()
		const override
	{
		return "Rogue";
	}

	const char* GetComponentName()
		const override
	{
		return "RogueClassComponent";
	}

	PlayerActionDefinition
		GetActionDefinition(
			PlayerActionType type)
		const override;

protected:

	std::vector<AnimationID>
		BuildNormalAttackSequence()
		const override
	{
		// 高速攻撃を仮使用。
		return
		{
			AnimationID::AttackFast
		};
	}
};

PlayerActionDefinition
RogueClassComponent::GetActionDefinition(
	PlayerActionType type) const
{
	PlayerActionDefinition definition =
		PlayerClassComponent::
		GetActionDefinition(
			type);

	switch (type)
	{
	case PlayerActionType::NormalAttack:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		definition.Animation =
			AnimationID::Attack;

		break;
	}
	case PlayerActionType::Skill1:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;
		definition.Cost.MP = 8;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		// 背後判定実装前の仮倍率。
		damage.PowerMultiplier =
			1.5f;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		definition.Animation =
			AnimationID::AttackFast;

		break;
	}
	case PlayerActionType::Skill2:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;
		definition.Cost.MP = 10;

		definition.TargetMode =
			TargetingMode::Grid;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				4,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::EmptyCell;

		// Teleport処理は後で専用実装。
		definition.Animation =
			AnimationID::Roll;

		break;
	}
	case PlayerActionType::Ultimate:
	{
		definition.Enabled = true;

		definition.Cost.AP = 4;
		definition.Cost.MP = 25;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			2.5f;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		definition.Animation =
			AnimationID::Attack3;

		break;
	}

	default:
		break;
	}

	return definition;
}