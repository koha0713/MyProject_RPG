#pragma once

#include "PlayerClassComponent.h"

/**
 * @brief Monk
 */
class MonkClassComponent :
	public PlayerClassComponent
{
public:

	PlayerClassType GetClassType()
		const override
	{
		return PlayerClassType::Monk;
	}

	const char* GetClassName()
		const override
	{
		return "Monk";
	}

	const char* GetComponentName()
		const override
	{
		return "MonkClassComponent";
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
		return
		{
			AnimationID::Punch
		};
	}
};

PlayerActionDefinition
MonkClassComponent::GetActionDefinition(
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
			AnimationID::Punch;

		break;
	}
	case PlayerActionType::Skill1:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;
		definition.Cost.MP = 5;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		// 2Hit•ª
		for (int i = 0; i < 2; ++i)
		{
			EffectDefinition damage;

			damage.Type =
				EffectType::Damage;

			damage.UseSourceAttackPower =
				true;

			damage.PowerMultiplier =
				0.65f;

			damage.AffectEnemies =
				true;

			definition.Effects.push_back(
				damage);
		}

		definition.Animation =
			AnimationID::AttackFast;

		break;
	}

	case PlayerActionType::Skill2:
	{
		definition.Enabled = true;

		definition.Cost.AP = 2;
		definition.Cost.MP = 5;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateDiamond(
				3,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		EffectDefinition damage;

		damage.Type =
			EffectType::Damage;

		damage.UseSourceAttackPower =
			true;

		damage.PowerMultiplier =
			1.2f;

		damage.AffectEnemies =
			true;

		definition.Effects.push_back(
			damage);

		// Ú‹ßˆÚ“®‚ÍŒã‚Åê—pˆ—‚ð’Ç‰ÁB
		definition.Animation =
			AnimationID::Attack2;

		break;
	}
	case PlayerActionType::Ultimate:
	{
		definition.Enabled = true;

		definition.Cost.AP = 4;
		definition.Cost.MP = 20;

		definition.TargetMode =
			TargetingMode::Unit;

		definition.TargetPattern =
			GridPattern::CreateCross(
				1,
				false);

		definition.Targeting.AllowedTargets =
			TargetType::Enemy;

		// ‰¼‚Å5Hit
		for (int i = 0; i < 5; ++i)
		{
			EffectDefinition damage;

			damage.Type =
				EffectType::Damage;

			damage.UseSourceAttackPower =
				true;

			damage.PowerMultiplier =
				0.55f;

			damage.AffectEnemies =
				true;

			definition.Effects.push_back(
				damage);
		}

		definition.Animation =
			AnimationID::Attack3;

		break;
	}

	default:
		break;
	}

	return definition;
}